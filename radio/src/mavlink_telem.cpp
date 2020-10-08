/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/*
 * (c) www.olliw.eu, OlliW, OlliW42
 */

#include "opentx.h"

//extern Fifo<uint8_t, MAVLINK_TELEM_TX_FIFO_SIZE> mavlinkTelemTxFifo;
//extern Fifo<uint8_t, MAVLINK_TELEM_RX_FIFO_SIZE> mavlinkTelemRxFifo;

MavlinkTelem mavlinkTelem;

// -- TASK handlers --
// tasks can be set directly with SETTASK()
// some tasks don't need immediate execution, or need reliable request
// this is what these handlers are for
// they push the task to a fifo, and also allow to set number of retries and retry rates

void MavlinkTelem::push_task(uint8_t idx, uint32_t task)
{
  struct Task t = {.task = task, .idx = idx};
  _taskFifo.push(t);
}

void MavlinkTelem::pop_and_set_task(void)
{
  struct Task t;
  if (_taskFifo.pop(t)) SETTASK(t.idx, t.task);
}

// -- REQUEST handlers --

void MavlinkTelem::set_request(uint8_t idx, uint32_t task, uint8_t retry, tmr10ms_t rate)
{
  push_task(idx, task);

  _request_is_waiting[idx] |= task;

  if (retry == 0) return; //well, if there would be another pending we would not kill it

  int8_t empty_i = -1;

  // first check if request is already pending, at the same time find free slot, to avoid having to loop twice
  for (uint16_t i = 0; i < REQUESTLIST_MAX; i++) {
    // TODO: should we modify the retry & rate of the pending task?
    if ((_requestList[i].idx == idx) && (_requestList[i].task == task)) return; // already pending, we can get out of here
    if ((empty_i < 0) && !_requestList[i].task) empty_i = i; // free slot
  }

  // if not already pending, add it
  if (empty_i < 0) return; // no free slot

  _requestList[empty_i].task = task;
  _requestList[empty_i].idx = idx;
  _requestList[empty_i].retry = retry;
  _requestList[empty_i].tlast = get_tmr10ms();
  _requestList[empty_i].trate = rate;
}

void MavlinkTelem::clear_request(uint8_t idx, uint32_t task)
{
  for (uint16_t i = 0; i < REQUESTLIST_MAX; i++) {
    if ((_requestList[i].idx == idx) && (_requestList[i].task == task)) {
      _requestList[i].task = 0;
      _request_is_waiting[idx] &=~ task;
    }
  }
}

//what happens if a clear never comes?
// well, this is what retry = UINT8_MAX says, right

void MavlinkTelem::do_requests(void)
{
  tmr10ms_t tnow = get_tmr10ms();

  for (uint16_t i = 0; i < TASKIDX_MAX; i++) _request_is_waiting[i] = 0;

  for (uint16_t i = 0; i < REQUESTLIST_MAX; i++) {
    if (!_requestList[i].task) continue;

    _request_is_waiting[_requestList[i].idx] |= _requestList[i].task;

    if ((tnow - _requestList[i].tlast) >= _requestList[i].trate) {
      push_task(_requestList[i].idx, _requestList[i].task);
      _requestList[i].tlast = get_tmr10ms();
      if (_requestList[i].retry < UINT8_MAX) {
        if (_requestList[i].retry) _requestList[i].retry--;
        if (!_requestList[i].retry) _requestList[i].task = 0; // clear request
      }
    }
  }

  if ((tnow - _taskFifo_tlast) > 6) { // 60 ms decimation
    _taskFifo_tlast = tnow;
    // change this, so that it skips tasks with 0, this would allow an easy means to clear tasks also in the Fifo
    if (!_taskFifo.isEmpty()) pop_and_set_task();
  }
}

// -- MAVLink stuff --

bool MavlinkTelem::isInVersionV2(void)
{
  return (_status.flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) ? false : true;
}

void MavlinkTelem::setOutVersionV2(void)
{
  _status.flags &=~ MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
}

void MavlinkTelem::setOutVersionV1(void)
{
  _status.flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
}

// -- Generate MAVLink messages --
// these should never be called directly, should only by called by the task handler

void MavlinkTelem::_generateCmdLong(
    uint8_t tsystem, uint8_t tcomponent, uint16_t cmd,
    float p1, float p2, float p3, float p4, float p5, float p6, float p7)
{
  setOutVersionV2();
  mavlink_msg_command_long_pack(
      _my_sysid, _my_compid, &_msg_out,
      tsystem, tcomponent, cmd, 0, p1, p2, p3, p4, p5, p6, p7
      );
  _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}

void MavlinkTelem::generateHeartbeat(uint8_t base_mode, uint32_t custom_mode, uint8_t system_status)
{
  setOutVersionV2();
  mavlink_msg_heartbeat_pack(
      _my_sysid, _my_compid, &_msg_out,
      MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, base_mode, custom_mode, system_status
      );
  _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}

void MavlinkTelem::generateParamRequestList(uint8_t tsystem, uint8_t tcomponent)
{
  setOutVersionV2();
  mavlink_msg_param_request_list_pack(
      _my_sysid, _my_compid, &_msg_out,
      tsystem, tcomponent
      );
  _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}

void MavlinkTelem::generateParamRequestRead(uint8_t tsystem, uint8_t tcomponent, const char* param_name)
{
char param_id[16];

  strncpy(param_id, param_name, 16);
  setOutVersionV2();
  mavlink_msg_param_request_read_pack(
      _my_sysid, _my_compid, &_msg_out,
      tsystem, tcomponent, param_id, -1
      );
  _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}

void MavlinkTelem::generateRequestDataStream(
    uint8_t tsystem, uint8_t tcomponent, uint8_t data_stream, uint16_t rate_hz, uint8_t startstop)
{
  setOutVersionV2();
  mavlink_msg_request_data_stream_pack(
      _my_sysid, _my_compid, &_msg_out,
      tsystem, tcomponent, data_stream, rate_hz, startstop
      );
  _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}

//ArduPilot: ignores param7
void MavlinkTelem::generateCmdSetMessageInterval(uint8_t tsystem, uint8_t tcomponent, uint8_t msgid, int32_t period_us, uint8_t startstop)
{
  _generateCmdLong(tsystem, tcomponent, MAV_CMD_SET_MESSAGE_INTERVAL, msgid, (startstop) ? period_us : -1.0f);
}

// -- Task handlers --

// -- Handle incoming MAVLink messages, which are for the Gimbal --

// -- Handle incoming MAVLink messages, which are for the Gimbal Manager --

// -- Handle incoming MAVLink messages, which are for the Camera --

// -- Handle incoming MAVLink messages, which are for the Autopilot --

// -- Main handler for incoming MAVLink messages --

void MavlinkTelem::handleMessage(void)
{
  if (_msg.sysid == 0) return; //this can't be anything meaningful

  // autodetect sys id, and handle autopilot connecting
  if (!isSystemIdValid() || (autopilot.compid == 0)) {
    if (_msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
      mavlink_heartbeat_t payload;
      mavlink_msg_heartbeat_decode(&_msg, &payload);
      if ((_msg.compid == MAV_COMP_ID_AUTOPILOT1) || (payload.autopilot != MAV_AUTOPILOT_INVALID)) {
        _sysid = _msg.sysid;
        autopilottype = payload.autopilot;
        vehicletype = payload.type;
        _resetAutopilot();
        autopilot.compid = _msg.compid;
        autopilot.requests_triggered = 1; //we need to postpone and schedule them
      }
    }
    if (!isSystemIdValid()) return;
  }

  // discoverers
  // somewhat inefficient, lots of heartbeat decodes, we probably want a separate heartbeat handler

  if ((camera.compid == 0) && (_msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)) {
    mavlink_heartbeat_t payload;
    mavlink_msg_heartbeat_decode(&_msg, &payload);
    if ( (payload.autopilot == MAV_AUTOPILOT_INVALID) &&
       ( (payload.type == MAV_TYPE_CAMERA) ||
         ((_msg.compid >= MAV_COMP_ID_CAMERA) && (_msg.compid <= MAV_COMP_ID_CAMERA6)) ) ) {
      _resetCamera();
      camera.compid = _msg.compid;
      camera.requests_triggered = 1; //we schedule them
    }
  }

  if ((gimbal.compid == 0) && (_msg.msgid == MAVLINK_MSG_ID_HEARTBEAT)) {
    mavlink_heartbeat_t payload;
    mavlink_msg_heartbeat_decode(&_msg, &payload);
    if ( (payload.autopilot == MAV_AUTOPILOT_INVALID) &&
       ( (payload.type == MAV_TYPE_GIMBAL) ||
         ((_msg.compid == MAV_COMP_ID_GIMBAL) ||
         ((_msg.compid >= MAV_COMP_ID_GIMBAL2) && (_msg.compid <= MAV_COMP_ID_GIMBAL6))) ) ) {
      _resetGimbalAndGimbalClient();
      gimbal.compid = _msg.compid;
      gimbal.is_initialized = true; //no startup requests, so true
    }
  }

  if ((gimbalmanager.compid == 0) && (gimbal.compid > 0) && (_msg.msgid == MAVLINK_MSG_ID_STORM32_GIMBAL_MANAGER_STATUS)) {
    mavlink_storm32_gimbal_manager_status_t payload;
    mavlink_msg_storm32_gimbal_manager_status_decode(&_msg, &payload);
    if (payload.gimbal_device_id == gimbal.compid) { //this is the gimbal's gimbal manager
      _resetGimbalClient();
      gimbalmanager.compid = _msg.compid;
      gimbalmanagerOut.device_flags = payload.device_flags;
      gimbalmanagerOut.manager_flags = payload.manager_flags;
      gimbalmanager.requests_triggered = 1; //we schedule them
    }
  }

  // reset receiving timeout, but ignore RADIO_STATUS
  if (_msg.msgid != MAVLINK_MSG_ID_RADIO_STATUS) {
    _is_receiving = MAVLINK_TELEM_RECEIVING_TIMEOUT;
  }

  // MAVLINK
  //if (msgFifo_enabled) msgRxFifo.push(_msg);

  // MAVSDK
  // also try to convert the MAVLink messages to FrSky sensors

  // RADIO_STATUS is somewhat tricky, this may need doing it better if there are more sources of it
  // SiK comes as vehicle 51, comp 68!
  // it must NOT be rated as _is_recieving!
  if (_msg.msgid == MAVLINK_MSG_ID_RADIO_STATUS) {
    mavlink_radio_status_t payload;
    mavlink_msg_radio_status_decode(&_msg, &payload);
    radio.rssi = payload.rssi;
    radio.remrssi = payload.remrssi;
    radio.noise = payload.noise;
    radio.remnoise = payload.remnoise;
    radio.is_receiving = MAVLINK_TELEM_RADIO_RECEIVING_TIMEOUT;
    if (g_model.mavlinkMimicSensors) {
      int32_t r = (payload.rssi == UINT8_MAX) ? 0 : payload.rssi;
      setTelemetryValue(PROTOCOL_TELEMETRY_FRSKY_SPORT, RSSI_ID, 0, 1, r, UNIT_DB, 0);
      //#if defined(MULTIMODULE)
      //{ TX_RSSI_ID, TX_RSSI_ID, 0, ZSTR_TX_RSSI   , UNIT_DB , 0 },
      //{ TX_LQI_ID , TX_LQI_ID,  0, ZSTR_TX_QUALITY, UNIT_RAW, 0 },
    }
    return;
  }

  if (_msg.sysid != _sysid) return; //this is not from our system

  // handle messages coming from autopilot
  if (autopilot.compid && (_msg.compid == autopilot.compid)) {
    handleMessageAutopilot();
  }
  if (camera.compid && (_msg.compid == camera.compid)) {
    handleMessageCamera();
  }
  if (gimbal.compid && (_msg.compid == gimbal.compid)) {
    handleMessageGimbal();
  }
  if (gimbalmanager.compid && (_msg.compid == gimbalmanager.compid)) {
    handleMessageGimbalClient();
  }
}

// -- Main task handler --

void MavlinkTelem::doTask(void)
{
  tmr10ms_t tnow = get_tmr10ms();

  bool tick_1Hz = false;

  if ((tnow - _my_heartbeat_tlast) > 100) { //1 sec
    _my_heartbeat_tlast = tnow;
    SETTASK(TASK_ME, TASK_SENDMYHEARTBEAT);

    msg_rx_persec = _msg_rx_persec_cnt;
    _msg_rx_persec_cnt = 0;
    bytes_rx_persec = _bytes_rx_persec_cnt;
    _bytes_rx_persec_cnt = 0;

    tick_1Hz = true;
  }

  if (!isSystemIdValid()) return;

  // trigger startup requests

  // we need to wait until at least one heartbeat was send out before requesting data streams
  if (autopilot.compid && autopilot.requests_triggered) {
    if (tick_1Hz) autopilot.requests_triggered++;
    if (autopilot.requests_triggered > 3) { // wait for 3 heartbeats
      autopilot.requests_triggered = 0;
      setAutopilotStartupRequests();
    }
  }

  // we wait until at least one heartbeat was send out, and autopilot requests have been done
  if (camera.compid && camera.requests_triggered && !autopilot.requests_triggered) {
    if (tick_1Hz) camera.requests_triggered++;
    if (camera.requests_triggered > 1) { // wait for the next heartbeat
      camera.requests_triggered = 0;
      setCameraStartupRequests();
    }
  }

  // we wait until at least one heartbeat was send out, and autopilot requests have been done
  if (gimbal.compid && gimbal.requests_triggered && !autopilot.requests_triggered) {
    if (tick_1Hz) gimbal.requests_triggered++;
    if (gimbal.requests_triggered > 1) { // wait for the next heartbeat
      gimbal.requests_triggered = 0;
      setGimbalStartupRequests();
    }
  }
  if (gimbalmanager.compid && gimbalmanager.requests_triggered && !autopilot.requests_triggered) {
    if (tick_1Hz) gimbalmanager.requests_triggered++;
    if (gimbalmanager.requests_triggered > 1) { // wait for the next heartbeat
      gimbalmanager.requests_triggered = 0;
      setGimbalClientStartupRequests();
    }
  }

  if (!autopilot.is_initialized) autopilot.is_initialized = (autopilot.requests_waiting_mask == 0); 
  
  if (!camera.is_initialized) camera.is_initialized = (camera.requests_waiting_mask == 0); 
  
  if (!gimbal.is_initialized) gimbal.is_initialized = (gimbal.requests_waiting_mask == 0); 
  
  if (!gimbalmanager.is_initialized) gimbalmanager.is_initialized = (gimbalmanager.requests_waiting_mask == 0); 
  
  // handle pending requests
  do_requests();

  // do rc override
  // ArduPilot has a DAMED BUG!!!
  // per MAVLink spec 0 and UNIT16_MAX should not be considered for channels >= 8, but it doesn't do it for 0
  // but we can hope that it handles 0 for the higher channels
  if (g_model.mavlinkRcOverride && param.SYSID_MYGCS >= 0) {
    if ((tnow - _rcoverride_tlast) >= 5) { //50 ms
      _rcoverride_tlast = tnow;
      for (uint8_t i = 0; i < 8; i++) {
        /* would this be the right way to figure out which output is actually active ??
        MixData * md;
        if (i < MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh == i) {
          int value = channelOutputs[i] + 2 * PPM_CH_CENTER(i) - 2 * PPM_CENTER;
          _tovr_chan_raw[i] = value;
        }
        else {
          _tovr_chan_raw[i] = UINT16_MAX;
        }*/
        // the first four channels may not be ordered like with transmitter!!
        int value = channelOutputs[i]/2 + PPM_CH_CENTER(i);
        _tovr_chan_raw[i] = value;
      }
      for (uint8_t i = 8; i < 18; i++) { 
        _tovr_chan_raw[i] = 0; 
      }
      SETTASK(TASK_AUTOPILOT, TASK_SENDMSG_RC_CHANNELS_OVERRIDE);
    }
  }

  // handle pending tasks
  // do only one task per loop
  if ((_txcount == 0) && TASK_IS_PENDING()) {
    //TASK_ME
    if (_task[TASK_ME] & TASK_SENDMYHEARTBEAT) {
      RESETTASK(TASK_ME,TASK_SENDMYHEARTBEAT);
      uint8_t base_mode = MAV_MODE_PREFLIGHT | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_SAFETY_ARMED;
      uint8_t system_status = MAV_STATE_UNINIT | MAV_STATE_ACTIVE;
      uint32_t custom_mode = 0;
      generateHeartbeat(base_mode, custom_mode, system_status);
      return; //do only one per loop
    }

    //other TASKS
    if (doTaskAutopilot()) return;
    if (doTaskGimbalAndGimbalClient()) return;
    if (doTaskCamera()) return;
    if (doTaskAutopilotLowPriority()) return;
    if (doTaskCameraLowPriority()) return;
  }
}

// -- Wakeup call from OpenTx --
// this is the single and main entry point

void MavlinkTelem::wakeup()
{
  // handle configuration change
  if ((_interface_enabled != g_model.mavlinkEnabled) || (_interface_config != g_model.mavlinkConfig)) { // a change occurred
    mavlinkTelemDeInit();
    _interface_enabled = g_model.mavlinkEnabled;
    _interface_config = g_model.mavlinkConfig;
    if (_interface_enabled) {
      switch (_interface_config) {
        case CONFIG_UART_A_115200: 
          mavlinkTelemInit('A', 57600); 
          break;
        case CONFIG_UART_A_57600: 
          mavlinkTelemInit('A', 115200); 
          break;
        case CONFIG_UART_A_38400: 
          mavlinkTelemInit('A', 38400); 
          break;
        case CONFIG_UART_A_19200: 
          mavlinkTelemInit('A', 19200); 
          break;
        default: 
          mavlinkTelemDeInit(); // should never happen
      }
    }
    _reset();
  }

  if (!_interface_enabled) return;

  // look for incoming messages, also do statistics
  uint32_t available = mavlinkTelemAvailable();
  if (available > 128) available = 128; //limit how much we read at once, shouldn't ever trigger
  for (uint32_t i = 0; i < available; i++) {
    uint8_t c;
    mavlinkTelemGetc(&c);
    _bytes_rx_persec_cnt++;
    if (mavlink_parse_char(MAVLINK_COMM_0, c, &_msg, &_status)) {
      // check for lost messages by analyzing seq
      if (_seq_rx_last >= 0) {
        uint16_t seq = _msg.seq;
        if (seq < _seq_rx_last) seq += 256;
        _seq_rx_last++;
        if (seq > _seq_rx_last) msg_rx_lost += (seq - _seq_rx_last);
      }
      _seq_rx_last = _msg.seq;
      handleMessage();
      msg_rx_count++;
      _msg_rx_persec_cnt++;
      if (g_model.mavlinkMimicSensors) telemetryStreaming = 2*TELEMETRY_TIMEOUT10ms; // 2 seconds
    }
  }

  // receiving timeouts
  if (_is_receiving) {
    _is_receiving--;
    if (!_is_receiving) _reset(); //this also resets is_receiving of all other components
  }
  if (radio.is_receiving) {
    radio.is_receiving--;
    if (!radio.is_receiving) _resetRadio();
  }
  if (autopilot.is_receiving) {
    autopilot.is_receiving--;
    if (!autopilot.is_receiving) _resetAutopilot();
  }
  if (gimbal.is_receiving) {
    gimbal.is_receiving--;
    if (!gimbal.is_receiving) _resetGimbalAndGimbalClient();
  }
  if (gimbalmanager.is_receiving) {
    gimbalmanager.is_receiving--;
    if (!gimbalmanager.is_receiving) _resetGimbalClient();
  }
  if (camera.is_receiving) {
    camera.is_receiving--;
    if (!camera.is_receiving) _resetCamera();
  }

  // do tasks
  doTask();

  // send out any pending messages
  if (_txcount) {
    if (mavlinkTelemPutBuf(_txbuf, _txcount)) {
      _txcount = 0;
    }
  }
}

// -- Resets --

void MavlinkTelem::_resetRadio(void)
{
  radio.is_receiving = 0;

  radio.rssi = UINT8_MAX;
  radio.remrssi = UINT8_MAX;
  radio.noise = 0;
  radio.remnoise = 0;
}

void MavlinkTelem::_reset(void)
{
  mavlink_reset_channel_status(MAVLINK_COMM_0);

  _my_sysid = MAVLINK_TELEM_MY_SYSID;
  _my_compid = MAVLINK_TELEM_MY_COMPID;

  msg_rx_count = 0;
  msg_rx_lost = 0;
  msg_rx_persec = 0;
  bytes_rx_persec = 0;
  _msg_rx_persec_cnt = 0;
  _bytes_rx_persec_cnt = 0;
  _seq_rx_last = -1;

  _sysid = 0;
  autopilottype = MAV_AUTOPILOT_GENERIC; //TODO: shouldn't these be in _resetAutopilot() ??
  vehicletype = MAV_TYPE_GENERIC;
  flightmode = 0;

  for (uint16_t i = 0; i < TASKIDX_MAX; i++) _task[i] = 0;
  _taskFifo.clear();
  _taskFifo_tlast = 0;
  for (uint16_t i = 0; i < REQUESTLIST_MAX; i++) _requestList[i].task = 0;

  _resetRadio();
  _resetAutopilot();
  _resetGimbalAndGimbalClient();
  _resetCamera();

  // MAVLINK
  //msgRxFifo.clear();
  //msgFifo_enabled = false;
}
