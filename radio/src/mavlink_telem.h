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


#define MAVLINK_COMM_NUM_BUFFERS      1 // 4
#define MAVLINK_MAX_SIGNING_STREAMS   1 // 16

#include "thirdparty/Mavlink/out/mavlink_types.h"
// checking for lost frames by analyzing seq won't work if we use common and not ardupilotmega
#include "thirdparty/Mavlink/out/opentx/mavlink.h"


#define MAVLINK_TELEM_MY_SYSID        254 //MissionPlanner is 255, QGroundControl is 255
#define MAVLINK_TELEM_MY_COMPID       (MAV_COMP_ID_MISSIONPLANNER + 4) //191 is companion, 194 is free

// wakeup() is currently called every 10 ms
// if one is changing this, timing needs to be adapted
#define MAVLINK_TELEM_RECEIVING_TIMEOUT                 330 // 3.3 secs
#define MAVLINK_TELEM_RADIO_RECEIVING_TIMEOUT           330 // 3.3 secs
#define MAVLINK_TELEM_GIMBALMANAGER_RECEIVING_TIMEOUT   330 // 3.3 secs ATTENTION: a GM may emit at slow rate

//COMMENT:
//  except of where noted functions/structs use units of the MAVLink message
//  the mavsdk caller/setter functions however use native units, and deg whenever possible

class MavlinkTelem
{
  public:
    MavlinkTelem() { _reset(); } // constructor

    enum ConfigUartEnum {
      CONFIG_UART_A_115200 = 0,
      CONFIG_UART_A_57600 = 1,
      CONFIG_UART_A_38400 = 2,
      CONFIG_UART_A_19200 = 3
    };

    void wakeup();

    // SOME MAVLINK stuff

    bool isInVersionV2(void); // not used
    void setOutVersionV2(void);
    void setOutVersionV1(void);

    // GENERATE MAVLink messages

    void _generateCmdLong(uint8_t tsystem, uint8_t tcomponent, uint16_t cmd, float p1=0.0f, float p2=0.0f, float p3=0.0f, float p4=0.0f, float p5=0.0f, float p6=0.0f, float p7=0.0f);
    void generateHeartbeat(uint8_t base_mode, uint32_t custom_mode, uint8_t system_status);
    void generateParamRequestList(uint8_t tsystem, uint8_t tcomponent);
    void generateParamRequestRead(uint8_t tsystem, uint8_t tcomponent, const char* param_name);
    // autopilot
    void generateRequestDataStream(uint8_t tsystem, uint8_t tcomponent, uint8_t data_stream, uint16_t rate_hz, uint8_t startstop);
    void generateCmdSetMessageInterval(uint8_t tsystem, uint8_t tcomponent, uint8_t msgid, int32_t period_us, uint8_t startstop);
    void generateCmdDoSetMode(uint8_t tsystem, uint8_t tcomponent, MAV_MODE base_mode, uint32_t custom_mode);
    void generateCmdNavTakeoff(uint8_t tsystem, uint8_t tcomponent, float alt_m, bool hor_nav_by_pilot);
    void generateCmdDoChangeSpeed(uint8_t tsystem, uint8_t tcomponent, float speed_mps, uint16_t speed_type, bool relative);
    void generateMissionItemInt(uint8_t tsystem, uint8_t tcomponent, uint8_t frame, uint16_t cmd, uint8_t current, int32_t lat, int32_t lon, float alt_m);
    void generateSetPositionTargetGlobalInt(uint8_t tsystem, uint8_t tcomponent, uint8_t frame, uint16_t type_mask, int32_t lat, int32_t lon, float alt, float vx, float vy, float vz, float yaw_rad, float yaw_rad_rate);
    void generateCmdConditionYaw(uint8_t tsystem, uint8_t tcomponent, float yaw_deg, float yaw_deg_rate, int8_t dir, bool rel);
    void generateRcChannelsOverride(uint8_t sysid, uint8_t tsystem, uint8_t tcomponent, uint16_t* chan_raw);
    // camera
    void generateCmdRequestCameraInformation(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdRequestCameraSettings(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdRequestStorageInformation(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdRequestCameraCapturesStatus(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdSetCameraMode(uint8_t tsystem, uint8_t tcomponent, uint8_t mode);
    void generateCmdImageStartCapture(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdVideoStartCapture(uint8_t tsystem, uint8_t tcomponent);
    void generateCmdVideoStopCapture(uint8_t tsystem, uint8_t tcomponent);
    // gimbal & gimbalmanager
    void generateCmdDoMountConfigure(uint8_t tsystem, uint8_t tcomponent, uint8_t mode);
    void generateCmdDoMountControl(uint8_t tsystem, uint8_t tcomponent, float pitch_deg, float yaw_deg, uint8_t mode);
    void generateCmdRequestGimbalDeviceInformation(uint8_t tsystem, uint8_t tcomponent);
    void generateStorm32GimbalDeviceControl(uint8_t tsystem, uint8_t tcomponent, float pitch_deg, float yaw_deg, uint16_t flags);
    void generateCmdRequestStorm32GimbalManagerInformation(uint8_t tsystem, uint8_t tcomponent);
    void generateStorm32GimbalManagerControl(uint8_t tsystem, uint8_t tcomponent, uint8_t gimbal_device_id, float pitch_deg, float yaw_deg, uint16_t device_flags, uint16_t manager_flags);
    void generateStorm32GimbalManagerControlPitchYaw(uint8_t tsystem, uint8_t tcomponent, uint8_t gimbal_device_id, float pitch_deg, float yaw_deg, uint16_t device_flags, uint16_t manager_flags);
    void generateCmdStorm32DoGimbalManagerControlPitchYaw(uint8_t tsystem, uint8_t tcomponent, uint8_t gimbal_device_id, float pitch_deg, float yaw_deg, uint16_t device_flags, uint16_t manager_flags);

    // TASK AND MESSAGE HANDLERS

    bool isSystemIdValid(void)
    {
      return (_sysid > 0);
    }

    bool doTaskAutopilot(void);
    bool doTaskAutopilotLowPriority(void);
    bool doTaskCamera(void);
    bool doTaskCameraLowPriority(void);
    bool doTaskGimbalAndGimbalClient(void);
    void doTask(void);

    void handleMessageAutopilot(void);
    void handleMessageCamera(void);
    void handleMessageGimbal(void);
    void handleMessageGimbalClient(void);
    void handleMessage(void);

    void setAutopilotStartupRequests(void);
    void setCameraStartupRequests(void);
    void setGimbalStartupRequests(void);
    void setGimbalClientStartupRequests(void);

    // TASKS

    #define TASKIDX_MAX       8

    #define SETTASK(idx,x)    {_task[idx] |= (x);}
    #define RESETTASK(idx,x)  {_task[idx] &=~ (x);}
    bool TASK_IS_PENDING()    {for(uint16_t i=0; i<TASKIDX_MAX; i++){ if (_task[i] > 0) return true;} return false;}

    // REQUESTS

    #define REQUESTLIST_MAX   32

    // the widget background task is called at 50ms, = 576 bytes max at 115200 bps
    // FIXME: we need something better here!!!
    // use two COMM'S ??!?!??!?
    // 16 is sufficient for a rate of 43 msg/s or 1350 bytes/s, 8 was NOT!
    //Fifo<mavlink_message_t, 2> msgRxFifo;  // HUGE!
    //bool msgFifo_enabled = false;

    // MAVSDK GENERAL

    bool isReceiving(void)
    {
      return (_is_receiving > 0);
    }

    uint8_t autopilottype = MAV_AUTOPILOT_GENERIC;
    uint8_t vehicletype = MAV_TYPE_GENERIC;
    uint8_t flightmode = 0;

    struct Radio {
      uint16_t is_receiving;
      uint8_t rssi;
      uint8_t remrssi;
      uint8_t noise;
      uint8_t remnoise;
    };
    struct Radio radio;

    struct Comp { // not all fields are relevant for/used by all components
      uint8_t compid;
      uint16_t is_receiving;
      //heartbeat
      uint8_t system_status;
      uint32_t custom_mode;
      bool is_armed;
      bool is_standby;
      bool is_critical;
      bool prearm_ok;
      uint8_t updated;
      //for initializing, if it expects some required messages to be received
      uint8_t requests_triggered;
      uint8_t requests_waiting_mask;
      bool is_initialized;
    };
    struct Comp autopilot;
    struct Comp gimbal;
    struct Comp camera;
    struct Comp gimbalmanager; //it's not exactly a component, can be the autopilot or the companion or the gimbal

    // MAVSDK AUTOPILOT

    struct Att {
      float roll_rad; // rad
      float pitch_rad; // rad
      float yaw_rad; // rad
      uint8_t updated;
    };
    struct Att att;

    struct Gps {
      uint8_t fix;
      uint8_t sat; // UINT8_MAX if unknown
      uint16_t hdop; // UINT16_MAX if unknown
      uint16_t vdop; // UINT16_MAX if unknown
      int32_t lat; // (WGS84), in degrees * 1E7*/
      int32_t lon; // (WGS84), in degrees * 1E7*/
      int32_t alt_mm; // (AMSL, NOT WGS84), in meters * 1000
      uint16_t vel_cmps; // m/s * 100, UINT16_MAX if unknown
      uint16_t cog_cdeg; // degrees * 100, 0.0..359.99 degrees, UINT16_MAX if unknown
      uint8_t updated;
    };
    struct Gps gps1;
    struct Gps gps2;
    uint8_t gps_instancemask;

    struct GlobalPositionInt {
      int32_t lat; // in degrees * 1E7*/
      int32_t lon; // in degrees * 1E7*/
      int32_t alt_mm; // (MSL), in mm
      int32_t relative_alt_mm; // in mm
      int16_t vx_cmps; // (Latitude, positive north), in cm/s
      int16_t vy_cmps; // (Longitude, positive east), in cm/s
      int16_t vz_cmps; // (Altitude, positive down), in cm/s
      uint16_t hdg_cdeg; // degrees * 100, 0.0..359.99 degrees, UINT16_MAX if unknown
      uint8_t updated;
    };
    struct GlobalPositionInt gposition;

    struct Vfr {
      float airspd_mps; // m/s
      float groundspd_mps; // m/s
      float alt_m; // (MSL), m   ?? is this really MSL ?? it can't I think, appears to be above home
      float climbrate_mps; // m/s
      int16_t heading_deg; // degrees (0..360, 0=north)
      uint16_t thro_pct; // percent, 0 to 100
      uint8_t updated;
    };
    struct Vfr vfr;

    struct Bat {
      int32_t charge_consumed_mAh; // mAh, -1 if not known
      int32_t energy_consumed_hJ; // 0.1 kJ, -1 if not known
      int16_t temperature_cC; // centi-degrees C°, INT16_MAX if not known
      uint32_t voltage_mV; // mV
      int16_t current_cA; // 10*mA, -1 if not known
      int8_t remaining_pct; //(0%: 0, 100%: 100), -1 if not known
      int8_t cellcount; //-1 if not known
      uint8_t updated;
    };
    struct Bat bat1;
    struct Bat bat2;
    uint8_t bat_instancemask;

    struct StatusText {
      Fifo<mavlink_statustext_t, 4> fifo;
      uint8_t updated;
    };
    struct StatusText statustext;

    enum MavApEkfFlags {
      MAVAP_EKF_ATTITUDE = 1,
      MAVAP_EKF_VELOCITY_HORIZ = 2,
      MAVAP_EKF_VELOCITY_VERT = 4,
      MAVAP_EKF_POS_HORIZ_REL = 8,
      MAVAP_EKF_POS_HORIZ_ABS = 16,
      MAVAP_EKF_POS_VERT_ABS = 32,
      MAVAP_EKF_POS_VERT_AGL = 64,
      MAVAP_EKF_CONST_POS_MODE = 128,
      MAVAP_EKF_PRED_POS_HORIZ_REL = 256,
      MAVAP_EKF_PRED_POS_HORIZ_ABS = 512,
    };

    struct Ekf {
      //comment: we don't really need the other fields in the EKF message
      uint16_t flags;
      uint8_t updated;
    };
    struct Ekf ekf;

    // this is very flight stack dependent
    struct Parameters {
      int16_t number;     //we use -1 to indicate it wasn't obtained
      int32_t BATT_CAPACITY;  //type int32 //we use -1 to indicate it wasn't obtained
      int32_t BATT2_CAPACITY; //type int32 //we use -1 to indicate it wasn't obtained
      float WPNAV_SPEED;    //type = float //we use NAN to indicate it wasn't obtained
      float WPNAV_ACCEL;    //type = float //we use NAN to indicate it wasn't obtained
      float WPNAV_ACCEL_Z;  //type = float //we use NAN to indicate it wasn't obtained
      int16_t SYSID_MYGCS;  //we use -1 to indicate it wasn't obtained
    };
    struct Parameters param;

    // AP: not armed -> filt_status.flags.horiz_pos_abs || filt_status.flags.pred_horiz_pos_abs
    //     armed -> filt_status.flags.horiz_pos_abs && !filt_status.flags.const_pos_mode
    bool apPositionOk(void)
    {
      return (ekf.flags & MAVAP_EKF_POS_HORIZ_ABS) && (ekf.flags & MAVAP_EKF_VELOCITY_HORIZ);
    }

    //some tasks need some additional data
    char _prr_param_id[16+1];

    uint8_t _tcsm_base_mode;
    uint32_t _tcsm_custom_mode;
    float _tcnt_alt_m;
    float _tccs_speed_mps;
    uint8_t _tccs_speed_type;
    uint8_t _tmii_frame;
    uint16_t _tmii_cmd;
    uint8_t _tmii_current;
    int32_t _tmii_lat, _tmii_lon;
    float _tmii_alt_m;
    uint8_t _t_coordinate_frame;
    uint16_t _t_type_mask;
    int32_t _t_lat, _t_lon;
    float _t_alt, _t_vx, _t_vy, _t_vz, _t_yaw_rad, _t_yaw_rad_rate;
    float _tccy_yaw_deg;
    int8_t _tccy_dir;
    bool _tccy_relative;
    float _tact_takeoff_alt_m;
    float _tacf_takeoff_alt_m;
    uint16_t _tovr_chan_raw[18];

    //convenience task wrapper for some tasks
    void setTaskParamRequestList(void)
    {
      SETTASK(TASK_AUTOPILOT, TASK_SENDMSG_PARAM_REQUEST_LIST);
    }
    void setTaskParamRequestRead(const char* pname)
    {
      strncpy(_prr_param_id, pname, 16);
      SETTASK(TASK_AUTOPILOT, TASK_SENDMSG_PARAM_REQUEST_READ);
    }

    void apSetFlightMode(uint32_t ap_flight_mode);
    void apSetGroundSpeed(float speed);
    void apSimpleGotoPosAlt(int32_t lat, int32_t lon, float alt);
    void apGotoPosAltYawDeg(int32_t lat, int32_t lon, float alt, float yaw);
    void apGotoPosAltVel(int32_t lat, int32_t lon, float alt, float vx, float vy, float vz);
    void apSetYawDeg(float yaw, bool relative); //note, we can enter negative yaw here, sign determines direction

    void apRequestBanner(void)
    {
      SETTASK(TASK_AP, TASK_AP_REQUESTBANNER);
    }
    void apArm(bool arm)
    {
      SETTASK(TASK_AP, (arm) ? TASK_AP_ARM : TASK_AP_DISARM);
    }
    void apCopterTakeOff(float alt)
    {
      _tact_takeoff_alt_m = alt;
      SETTASK(TASK_AP, TASK_AP_COPTER_TAKEOFF);
    }
    void apLand(void)
    {
      SETTASK(TASK_AP, TASK_AP_LAND);
    }
    void apCopterFlyClick(void)
    {
      SETTASK(TASK_AP, TASK_AP_COPTER_FLYCLICK);
    }
    void apCopterFlyHold(float alt)
    {
      _tacf_takeoff_alt_m = alt;
      SETTASK(TASK_AP, TASK_AP_COPTER_FLYHOLD);
    }
    void apCopterFlyPause(void)
    {
      SETTASK(TASK_AP, TASK_AP_COPTER_FLYPAUSE);
    }

    // MAVSDK CAMERA

    struct CameraInfo {
      char vendor_name[32+1];
      char model_name[32+1];
      uint32_t firmware_version;
      uint32_t flags;
      bool has_video;
      bool has_photo;
      bool has_modes;
      float total_capacity_MiB; // NAN if not known
    };
    struct CameraInfo cameraInfo;  // Info: static data

    struct CameraStatus {
      uint8_t mode;
      bool video_on;
      bool photo_on;
      float available_capacity_MiB; // NAN if not known
      uint32_t recording_time_ms;
      float battery_voltage_V; // NAN if not known
      int8_t battery_remaining_pct; //(0%: 0, 100%: 100), -1 if not known
    };
    struct CameraStatus cameraStatus; // Status: variable data

    //convenience task wrapper
    void sendCameraSetVideoMode(void)
    {
      SETTASK(TASK_CAMERA, TASK_SENDCMD_SET_CAMERA_VIDEO_MODE);
    }
    void sendCameraSetPhotoMode(void)
    {
      SETTASK(TASK_CAMERA, TASK_SENDCMD_SET_CAMERA_PHOTO_MODE);
    }
    void sendCameraStartVideo(void)
    {
      SETTASK(TASK_CAMERA, TASK_SENDCMD_VIDEO_START_CAPTURE);
    }
    void sendCameraStopVideo(void)
    {
      SETTASK(TASK_CAMERA, TASK_SENDCMD_VIDEO_STOP_CAPTURE);
    }
    void sendCameraTakePhoto(void)
    {
      SETTASK(TASK_CAMERA, TASK_SENDCMD_IMAGE_START_CAPTURE);
    }

    // MAVSDK GIMBAL & GIMBAL CLIENT

    struct GimbalAtt {
      float roll_deg;
      float pitch_deg;
      float yaw_deg_relative;
      float yaw_deg_absolute;
      uint8_t updated;
    };
    struct GimbalAtt gimbalAtt;

    //some tasks need some additional data
    uint8_t _t_gimbal_mode;
    float _t_gimbal_pitch_deg, _t_gimbal_yaw_deg;

    //convenience task wrapper
    void sendGimbalTargetingMode(uint8_t mode);
    void sendGimbalPitchYawDeg(float pitch, float yaw);

    // MAVSDK GIMBAL CLIENT only

    struct GimbalDeviceInfo {
      char vendor_name[32+1];
      char model_name[32+1];
      char custom_name[32+1];
      uint32_t firmware_version;
      uint32_t hardware_version;
      uint32_t cap_flags;
      uint8_t updated;
    };
    struct GimbalDeviceInfo gimbaldeviceInfo;

    struct Storm32GimbalManagerInfo {
      uint32_t device_cap_flags;
      uint32_t manager_cap_flags;
      uint8_t updated;
    };
    struct Storm32GimbalManagerInfo gimbalmanagerInfo;

    struct Storm32GimbalManagerStatus {
      uint8_t supervisor;
      uint16_t device_flags;
      uint16_t manager_flags;
      uint8_t profile;
      uint8_t updated;
    };
    struct Storm32GimbalManagerStatus gimbalmanagerStatus;

    //some tasks need some additional data
    float _t_storm32GD_pitch_deg, _t_storm32GD_yaw_deg;
    uint16_t _t_storm32GD_flags;
    float _t_storm32GM_pitch_deg, _t_storm32GM_yaw_deg;
    uint16_t _t_storm32GM_gdflags, _t_storm32GM_gmflags;
    float _t_storm32GM_control_pitch_deg, _t_storm32GM_control_yaw_deg;
    uint16_t _t_storm32GM_control_gdflags, _t_storm32GM_control_gmflags;
    float _t_storm32GM_cmd_pitch_deg, _t_storm32GM_cmd_yaw_deg;
    uint16_t _t_storm32GM_cmd_gdflags, _t_storm32GM_cmd_gmflags;

    //convenience task wrapper
    void sendStorm32GimbalDevicePitchYawDeg(float pitch, float yaw);
    void sendStorm32GimbalManagerPitchYawDeg(float pitch, float yaw);
    void sendStorm32GimbalManagerControlPitchYawDeg(float pitch, float yaw);
    void sendStorm32GimbalManagerCmdPitchYawDeg(float pitch, float yaw);

    struct GimbalManagerOut { //collective structure to handle gimbalmanager outgoing flags
      uint8_t mount_mode; //copy of the mount_mode
      uint16_t device_flags;
      uint16_t manager_flags;
    };
    struct GimbalManagerOut gimbalmanagerOut;

    void _refreshGimbalClientFlags(uint16_t* device_flags, uint16_t* manager_flags);
    void setStorm32GimbalClientRetract(bool enable);
    void setStorm32GimbalClientNeutral(bool enable);
    void setStorm32GimbalClientLock(bool roll_lock, bool pitch_lock, bool yaw_lock);
    void setStorm32GimbalClientFlags(uint16_t manager_flags);

    //gimbal protocol v1 vs v2
    void setStorm32GimbalProtocolV2(bool flag)
    {
      _storm32_gimbal_protocol_v2 = flag;
    }
    bool isStorm32GimbalProtocolV2(void)
    {
      return _storm32_gimbal_protocol_v2;
    }

    // SOME more MAVLINK stuff

    const mavlink_status_t* getChannelStatus(void)
    {
      return &_status;
    }

    uint32_t msg_rx_count;
    uint32_t msg_rx_persec;
    uint32_t bytes_rx_persec;
    uint32_t msg_rx_lost;

  // PROTECTED FIELDS and METHODS
  protected:

    void _reset(void);
    void _resetRadio(void);
    void _resetAutopilot(void);
    void _resetCamera(void);
    void _resetGimbalAndGimbalClient(void);
    void _resetGimbalClient(void);

    uint8_t _my_sysid = MAVLINK_TELEM_MY_SYSID;
    uint8_t _my_compid = MAVLINK_TELEM_MY_COMPID;
    tmr10ms_t _my_heartbeat_tlast;
    tmr10ms_t _rcoverride_tlast;

    uint8_t _sysid = 0; // is autodetected by inspecting the autopilot heartbeat

    uint16_t _is_receiving = 0; // is set by any arbitrary incoming MAVLink message

    // TASKS

    enum TaskIdxEnum {
      TASK_ME = 0,
      TASK_AUTOPILOT, // autopilot, non-specific
      TASK_AP,        // autopilot, ardupilot-specific
      TASK_GIMBAL,    // gimbal and gimbal client
      TASK_CAMERA,    // camera
    };

    enum TaskMaskEnum {
      // me
      TASK_SENDMYHEARTBEAT                        = 0x00000001,
      // autopilot
      TASK_SENDREQUESTDATASTREAM_RAW_SENSORS      = 0x00000001, // group 1
      TASK_SENDREQUESTDATASTREAM_EXTENDED_STATUS  = 0x00000002, // group 2
      TASK_SENDREQUESTDATASTREAM_RC_CHANNELS      = 0x00000004, // group 3
      TASK_SENDREQUESTDATASTREAM_RAW_CONTROLLER   = 0x00000008, // group 4
      TASK_SENDREQUESTDATASTREAM_POSITION         = 0x00000010, // group 6
      TASK_SENDREQUESTDATASTREAM_EXTRA1           = 0x00000020, // group 10
      TASK_SENDREQUESTDATASTREAM_EXTRA2           = 0x00000040, // group 11
      TASK_SENDREQUESTDATASTREAM_EXTRA3           = 0x00000080, // group 12
      TASK_SENDCMD_REQUEST_ATTITUDE               = 0x00000100,
      TASK_SENDCMD_REQUEST_GLOBAL_POSITION_INT    = 0x00000200,
      TASK_SENDMSG_PARAM_REQUEST_LIST             = 0x00001000,
      TASK_SENDMSG_PARAM_REQUEST_READ             = 0x00002000,

      TASK_SENDCMD_DO_SET_MODE                    = 0x00010000,
      TASK_SENDCMD_NAV_TAKEOFF                    = 0x00020000, // simple_takeoff()
      TASK_SENDCMD_DO_CHANGE_SPEED                = 0x00040000, // groundspeed(), airspeed()
      TASK_SENDMSG_MISSION_ITEM_INT               = 0x00080000, // simple_goto()
      TASK_SENDMSG_SET_POSITION_TARGET_GLOBAL_INT = 0x00100000,
      TASK_SENDCMD_CONDITION_YAW                  = 0x00200000,
      TASK_SENDMSG_RC_CHANNELS_OVERRIDE           = 0x00400000,
      // ap
      TASK_AP_REQUESTBANNER                       = 0x00000001,
      TASK_AP_ARM                                 = 0x00000002,
      TASK_AP_DISARM                              = 0x00000004,
      TASK_AP_COPTER_TAKEOFF                      = 0x00000008,
      TASK_AP_LAND                                = 0x00000010,
      TASK_AP_COPTER_FLYCLICK                     = 0x00000020,
      TASK_AP_COPTER_FLYHOLD                      = 0x00000040,
      TASK_AP_COPTER_FLYPAUSE                     = 0x00000080,

      TASK_AP_REQUESTPARAM_BATT_CAPACITY          = 0x00010000,
      TASK_AP_REQUESTPARAM_BATT2_CAPACITY         = 0x00020000,
      TASK_AP_REQUESTPARAM_WPNAV_SPEED            = 0x00040000,
      TASK_AP_REQUESTPARAM_WPNAV_ACCEL            = 0x00080000,
      TASK_AP_REQUESTPARAM_WPNAV_ACCEL_Z          = 0x00100000,
      TASK_AP_REQUESTPARAM_SYSID_MYGCS            = 0x00200000,
      // camera
      TASK_SENDREQUEST_CAMERA_INFORMATION         = 0x00000001,
      TASK_SENDREQUEST_CAMERA_SETTINGS            = 0x00000002,
      TASK_SENDREQUEST_STORAGE_INFORMATION        = 0x00000004,
      TASK_SENDREQUEST_CAMERA_CAPTURE_STATUS      = 0x00000008,
      TASK_SENDCMD_SET_CAMERA_VIDEO_MODE          = 0x00000010,
      TASK_SENDCMD_SET_CAMERA_PHOTO_MODE          = 0x00000020,
      TASK_SENDCMD_VIDEO_START_CAPTURE            = 0x00000040,
      TASK_SENDCMD_VIDEO_STOP_CAPTURE             = 0x00000080,
      TASK_SENDCMD_IMAGE_START_CAPTURE            = 0x00000100,
      // gimbal & gimbal client
      TASK_SENDCMD_DO_MOUNT_CONFIGURE             = 0x00000001, //this goes to the autopilot
      TASK_SENDCMD_DO_MOUNT_CONTROL               = 0x00000002, //this goes to the autopilot

      TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION  = 0x00000008, //this goes to the gimbal device

      TASK_SENDMSG_GIMBAL_DEVICE_CONTROL              = 0x00000010, //this goes to the storm32 gimbal device
      TASK_SENDREQUEST_GIMBAL_MANAGER_INFORMATION     = 0x00000020, //this goes to the storm32 gimbal manager
      TASK_SENDMSG_GIMBAL_MANAGER_CONTROL             = 0x00000040, //this goes to the storm32 gimbal manager
      TASK_SENDMSG_GIMBAL_MANAGER_CONTROL_PITCHYAW    = 0x00000080, //this goes to the storm32 gimbal manager
      TASK_SENDCMD_DO_GIMBAL_MANAGER_CONTROL_PITCHYAW = 0x00000100, //this goes to the storm32 gimbal manager
    };

    uint32_t _task[TASKIDX_MAX];

    struct Task {
      uint32_t task;
      uint8_t idx;
    };

    Fifo<struct Task, 32> _taskFifo; // the fifo is to further rate limit the execution of tasks
    tmr10ms_t _taskFifo_tlast;

    void push_task(uint8_t idx, uint32_t task);
    void pop_and_set_task(void);

    // REQUESTS

    enum AutopilotRequestWaitingFlags {
      AUTOPILOT_REQUESTWAITING_GPS_RAW_INT         = 0x01,
      AUTOPILOT_REQUESTWAITING_GLOBAL_POSITION_INT = 0x02,
      AUTOPILOT_REQUESTWAITING_ATTITUDE            = 0x04,
      AUTOPILOT_REQUESTWAITING_VFR_HUD             = 0x08,
      AUTOPILOT_REQUESTWAITING_EKF_STATUS_REPORT   = 0x10,
      AUTOPILOT_REQUESTWAITING_ALL                 = 0x0F,
    };

    enum CameraRequestWaitingFlags {
      CAMERA_REQUESTWAITING_CAMERA_INFORMATION    = 0x01,
      CAMERA_REQUESTWAITING_CAMERA_SETTINGS       = 0x02,
      CAMERA_REQUESTWAITING_CAMERA_CAPTURE_STATUS = 0x04,
      CAMERA_REQUESTWAITING_ALL                   = 0x07,
    };

    enum GimbalRequestWaitingFlags {
      GIMBAL_REQUESTWAITING_GIMBAL_DEVICE_INFORMATION  = 0x01,
      GIMBAL_REQUESTWAITING_GIMBAL_MANAGER_INFORMATION = 0x02,
      GIMBAL_REQUESTWAITING_ALL                        = 0x01,
      GIMBALCLIENT_REQUESTWAITING_ALL                  = 0x03,
    };

    struct Request {
      uint32_t task;
      uint8_t idx;
      uint8_t retry; // UINT8_MAX means request it for ever
      tmr10ms_t tlast;
      tmr10ms_t trate;
    };

    struct Request _requestList[REQUESTLIST_MAX];  // 0 in the task field indicates that the slot is free and unused
    uint32_t _request_is_waiting[TASKIDX_MAX];

    void set_request(uint8_t idx, uint32_t task, uint8_t retry, tmr10ms_t rate = 102);
    void clear_request(uint8_t idx, uint32_t task);
    void do_requests(void);

    // MORE MAVLINK STUFF

    uint32_t _msg_rx_persec_cnt;
    uint32_t _bytes_rx_persec_cnt;
    int16_t _seq_rx_last = -1;

    mavlink_status_t _status;
    mavlink_message_t _msg;
    mavlink_message_t _msg_out;
    uint16_t _txcount = 0;
    uint8_t _txbuf[512]; //only needs to hold one MAVLink message, thus 512 is by construction large enough

    // STUFF

    bool _interface_enabled = false;
    uint8_t _interface_config = UINT8_MAX; //to enforce change

    bool _storm32_gimbal_protocol_v2 = false;
};

extern MavlinkTelem mavlinkTelem;
