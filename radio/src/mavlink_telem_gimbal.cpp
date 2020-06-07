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


#define FPI         (3.141592653589793f) // 3.141592654f
#define FDEGTORAD   (FPI/180.0f)
#define FRADTODEG   (180.0f/FPI)

#define INCU8(x)  if ((x) < UINT8_MAX) { (x)++; }


// quaternions and euler angles
// we do not use NED (roll-pitch-yaw) to convert quaternion to Euler angles and vice versa
// we use pitch-roll-yaw instead
// when the roll angle is zero, both are equivalent, this should be the majority of cases anyhow
// also, for most gimbals pitch-roll-yaw is appropriate
// the issue with NED is the gimbal lock at pitch +-90°, but pitch +-90° is a common operation point for gimbals
// the angles we store in this lib are thus pitch-roll-yaw Euler

//NED
void calc_q_from_NED_angles_deg(float* q, float roll_deg, float pitch_deg, float yaw_deg)
{
	float cr2 = cosf(roll_deg * 0.5f * FDEGTORAD);
	float sr2 = sinf(roll_deg * 0.5f * FDEGTORAD);
	float cp2 = cosf(pitch_deg * 0.5f * FDEGTORAD);
	float sp2 = sinf(pitch_deg * 0.5f * FDEGTORAD);
	float cy2 = cosf(yaw_deg * 0.5f * FDEGTORAD);
	float sy2 = sinf(yaw_deg * 0.5f * FDEGTORAD);

	q[0] = cr2*cp2*cy2 + sr2*sp2*sy2;
	q[1] = sr2*cp2*cy2 - cr2*sp2*sy2;
	q[2] = cr2*sp2*cy2 + sr2*cp2*sy2;
	q[3] = cr2*cp2*sy2 - sr2*sp2*cy2;
}


//equal for NED and G
void calc_q_from_pitchyaw_deg(float* q, float pitch_deg, float yaw_deg)
{
	float cp2 = cosf(pitch_deg * 0.5f * FDEGTORAD);
	float sp2 = sinf(pitch_deg * 0.5f * FDEGTORAD);
	float cy2 = cosf(yaw_deg * 0.5f * FDEGTORAD);
	float sy2 = sinf(yaw_deg * 0.5f * FDEGTORAD);

	q[0] = cp2*cy2;
	q[1] = -sp2*sy2;
	q[2] = sp2*cy2;
	q[3] = cp2*sy2;
}


void calc_NED_angles_deg_from_q(float* roll_deg, float* pitch_deg, float* yaw_deg, float* q)
{
	// roll = atan2f(2.0f*(q0*q1 + q2*q3), 1.0f - 2.0f*(q1*q1 + q2*q2));   // R32 / R33 = cpsr / cpcr
	// pitch = asin(2.0f*(q0*q2 - q1*q3));                                 // -R31 = - (-sp)
	// yaw = atan2f(2.0f*(q0*q3 + q1*q2), 1.0f - 2.0f*(q2*q2 + q3*q3));    // R21 / R11 = cpsy / cpcy

    *roll_deg = atan2f( 2.0f*(q[0]*q[1] + q[2]*q[3]), 1.0f - 2.0f*(q[1]*q[1] + q[2]*q[2]) ) * FRADTODEG;

    float arg = 2.0f*(q[0]*q[2] - q[1]*q[3]);
    if (isnan(arg)) {
         *pitch_deg = 0.0f;
    } else if (arg >= 1.0f) {
        *pitch_deg = 90.0f;
    } else if (arg <= -1.0f) {
        *pitch_deg = -90.0f;
    } else {
        *pitch_deg = asinf(arg) * FRADTODEG;
    }

    *yaw_deg = atan2f( 2.0f*(q[0]*q[3] + q[1]*q[2]), 1.0f - 2.0f*(q[2]*q[2] + q[3]*q[3]) ) * FRADTODEG;
}


void calc_G_angles_deg_from_q(float* roll_deg, float* pitch_deg, float* yaw_deg, float* q)
{
    // pitch = atan2f(2.0f*(q0*q2 - q1*q3), 1.0f - 2.0f*(q1*q1 + q2*q2));  // -R31 / R33 = -(-spcr) / cpcr
	// roll = asin(2.0f*(q0*q1 + q2*q3));                                  // R32 = sr
	// yaw = atan2f(2.0f*(q0*q3 - q1*q2), 1.0f - 2.0f*(q1*q1 + q3*q3));    // -R12 / R22 = -(-crsy) / crcy

    *pitch_deg = atan2f( q[0]*q[2] - q[1]*q[3], 0.5f - q[1]*q[1] - q[2]*q[2] ) * FRADTODEG;

    float arg = 2.0f*(q[0]*q[1] + q[2]*q[3]);
    if (isnan(arg)) {
         *roll_deg = 0.0f;
    } else if (arg >= 1.0f) {
        *roll_deg = 90.0f;
    } else if (arg <= -1.0f) {
        *roll_deg = -90.0f;
    } else {
        *roll_deg = asinf(arg) * FRADTODEG;
    }

    *yaw_deg = atan2f( q[0]*q[3] - q[1]*q[2], 0.5f - q[1]*q[1] - q[3]*q[3] ) * FRADTODEG;
}



// -- Generate MAVLink messages --
// these should never be called directly, should only be called by the task handler

void MavlinkTelem::generateCmdDoMountConfigure(uint8_t tsystem, uint8_t tcomponent, uint8_t mode)
{
    _generateCmdLong(tsystem, tcomponent,
    		MAV_CMD_DO_MOUNT_CONFIGURE,
			mode, 0,0,0,0,0,0);
}


//ArduPilot: if a mount has no pan control, then this will also yaw the copter in guided mode overwriting _fixed_yaw !!
void MavlinkTelem::generateCmdDoMountControl(uint8_t tsystem, uint8_t tcomponent, float pitch_deg, float yaw_deg)
{
    _generateCmdLong(tsystem, tcomponent,
    		MAV_CMD_DO_MOUNT_CONTROL,
            pitch_deg, 0.0, yaw_deg, 0,0,0, MAV_MOUNT_MODE_MAVLINK_TARGETING);
}


void MavlinkTelem::generateGimbalDeviceSetAttitude(uint8_t tsystem, uint8_t tcomponent,
		float pitch_deg, float yaw_deg, uint16_t flags)
{
float q[4];

    if ((pitch_deg != NAN) && (yaw_deg != NAN)) {
    	calc_q_from_pitchyaw_deg(q, pitch_deg, yaw_deg);
    } else {
    	q[0] = q[1] = q[2] = q[3] = NAN;
    }

    setOutVersionV2();
    mavlink_msg_gimbal_device_set_attitude_pack(
            _sysid, _my_compid, &_msg_out, //_sys_id and not _my_sysid !!! we mimic being part of the autopilot system
			tsystem, tcomponent,
			flags,
			q,
			NAN, NAN, NAN);
    _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}


void MavlinkTelem::generateCmdRequestGimbalDeviceInformation(uint8_t tsystem, uint8_t tcomponent)
{
    _generateCmdLong(tsystem, tcomponent,
    		MAV_CMD_REQUEST_MESSAGE,
			MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION, 0,0,0,0,0,0);
}


void MavlinkTelem::generateCmdRequestGimbalManagerInformation(uint8_t tsystem, uint8_t tcomponent)
{
    _generateCmdLong(tsystem, tcomponent,
    		MAV_CMD_REQUEST_MESSAGE,
			MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, 0,0,0,0,0,0);
}


void MavlinkTelem::generateGimbalManagerSetAttitude(uint8_t tsystem, uint8_t tcomponent,
		uint8_t gimbal_device_id, float pitch_deg, float yaw_deg, uint32_t flags)
{
float q[4];

	if ((pitch_deg != NAN) && (yaw_deg != NAN)) {
		calc_q_from_pitchyaw_deg(q, pitch_deg, yaw_deg);
	} else {
		q[0] = q[1] = q[2] = q[3] = NAN;
	}

    setOutVersionV2();
    mavlink_msg_gimbal_manager_set_attitude_pack(
            _my_sysid, _my_compid, &_msg_out,
			tsystem, tcomponent,
			flags,
			gimbal_device_id,
			q,
			NAN, NAN, NAN);
    _txcount = mavlink_msg_to_send_buffer(_txbuf, &_msg_out);
}


void MavlinkTelem::generateCmdDoGimbalManagerAttitude(uint8_t tsystem, uint8_t tcomponent,
		uint8_t gimbal_device_id, float pitch_deg, float yaw_deg, uint32_t flags)
{
    _generateCmdLong(tsystem, tcomponent,
    		MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE,
    		NAN, NAN, pitch_deg, yaw_deg, flags, 0, gimbal_device_id);
}



// -- Mavsdk Convenience Task Wrapper --
// to make it easy for api_mavsdk to call functions

void MavlinkTelem::setGimbalTargetingMode(uint8_t mode)
{
    _t_gimbal_mode = mode;
    SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_MOUNT_CONFIGURE);
}


void MavlinkTelem::setGimbalPitchYawDeg(float pitch, float yaw)
{
    _t_gimbal_pitch_deg = pitch;
    _t_gimbal_yaw_deg = yaw;
    SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_MOUNT_CONTROL);
}


//we shouldn't use this
void MavlinkTelem::setGimbalDevicePitchYawDeg(float pitch, float yaw)
{
    _t_gimbaldevice_pitch_deg = pitch;
    _t_gimbaldevice_yaw_deg = yaw;
    //we assume a simple 3 axis gimbal
    // our gimbal must EXACTLY have these capabilities for now, so we fake it
    _t_gimbaldevice_flags |= GIMBAL_DEVICE_FLAGS_ROLL_LOCK | GIMBAL_DEVICE_FLAGS_PITCH_LOCK;
    _t_gimbaldevice_flags &=~ GIMBAL_DEVICE_FLAGS_YAW_LOCK;
    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_DEVICE_SET_ATTITUDE);
}


//we shouldn't use this
void MavlinkTelem::setGimbalManagerCmdPitchYawDeg(float pitch, float yaw)
{
    _t_gimbalmanager_cmd_pitch_deg = pitch;
    _t_gimbalmanager_cmd_yaw_deg = yaw;
    _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
    _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
    _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
                                    GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
    		                        GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
    SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
}


// we have introduced our own specific method for handling the gimbal manager flags, by introducing GIMBALMANAGER_MODE_XXX
// that's no breach of specification however, so perfectly fine
void MavlinkTelem::setGimbalClientMode(uint16_t mode)
{
	_gimbalclient_mode = mode;
}


void MavlinkTelem::setGimbalManagerPitchYawDeg(float pitch, float yaw)
{
uint32_t flags = gimbalmanagerStatus.flags;

    //clear GCS and RC flags as default, so we only have to set which we need
    flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
               GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
               GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);

	switch (_gimbalclient_mode) {
	case GIMBALCLIENT_MODE_NONE:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = flags;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RETRACT:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_RETRACT);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_NEUTRAL:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_GCS_NUDGE);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RC_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_RC_NUDGE);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RC_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = (flags | GIMBAL_MANAGER_FLAGS_RC_OVERRIDE);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;

	case GIMBALCLIENT_SETFLAG_GCS_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_GCS_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_SETFLAG_GCS_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_SETFLAG_RC_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_RC_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_RC_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_SETFLAG_RC_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_RC_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_RC_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_SETFLAG_CMD_OVERRIDE:
        _t_gimbalmanager_cmd_pitch_deg = pitch;
        _t_gimbalmanager_cmd_yaw_deg = yaw;
        _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NUDGE;
        _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
        SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
		break;
	case GIMBALCLIENT_SETFLAG_CMD_NUDGE:
        _t_gimbalmanager_cmd_pitch_deg = pitch;
        _t_gimbalmanager_cmd_yaw_deg = yaw;
        _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
        _t_gimbalmanager_cmd_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
        _t_gimbalmanager_cmd_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NUDGE;
        _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
        SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
		break;
    }

/*

	switch (_gimbalclient_mode) {
	case GIMBALCLIENT_MODE_NONE:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
				                           GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                           GIMBAL_MANAGER_FLAGS_RETRACT|GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RETRACT:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_RETRACT;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
                                           GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                           GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_NEUTRAL:
	    _t_gimbalmanager_setatt_pitch_deg = NAN;
	    _t_gimbalmanager_setatt_yaw_deg = NAN;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_NEUTRAL;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
                                           GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                           GIMBAL_MANAGER_FLAGS_RETRACT);
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
                                           GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                           GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE |
                                           GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                           GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_CMD:
        _t_gimbalmanager_cmd_pitch_deg = pitch;
        _t_gimbalmanager_cmd_yaw_deg = yaw;
        _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NUDGE;
        _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE | GIMBAL_MANAGER_FLAGS_GCS_NUDGE |
	    								GIMBAL_MANAGER_FLAGS_RC_OVERRIDE | GIMBAL_MANAGER_FLAGS_RC_NUDGE |
	    		                        GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
        SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
		break;

	case GIMBALCLIENT_MODE_GCS_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_GCS_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
		break;
	case GIMBALCLIENT_MODE_GCS_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_GCS_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_GCS_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RC_OVERRIDE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_RC_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_RC_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_RC_NUDGE:
	    _t_gimbalmanager_setatt_pitch_deg = pitch;
	    _t_gimbalmanager_setatt_yaw_deg = yaw;
	    _t_gimbalmanager_setatt_flags = gimbalmanagerStatus.flags;
	    _t_gimbalmanager_setatt_flags &=~ GIMBAL_MANAGER_FLAGS_RC_OVERRIDE;
  	    _t_gimbalmanager_setatt_flags |= GIMBAL_MANAGER_FLAGS_RC_NUDGE;
	    _t_gimbalmanager_setatt_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
	    SETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_CMD_OVERRIDE:
        _t_gimbalmanager_cmd_pitch_deg = pitch;
        _t_gimbalmanager_cmd_yaw_deg = yaw;
        _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
        _t_gimbalmanager_cmd_flags &=~ GIMBAL_MANAGER_FLAGS_MISSION_NUDGE;
        _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
        SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
		break;
	case GIMBALCLIENT_MODE_CMD_NUDGE:
        _t_gimbalmanager_cmd_pitch_deg = pitch;
        _t_gimbalmanager_cmd_yaw_deg = yaw;
        _t_gimbalmanager_cmd_flags = gimbalmanagerStatus.flags;
        _t_gimbalmanager_cmd_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NOTOVERRIDE;
        _t_gimbalmanager_cmd_flags |= GIMBAL_MANAGER_FLAGS_MISSION_NUDGE;
        _t_gimbalmanager_cmd_flags &=~ (GIMBAL_MANAGER_FLAGS_RETRACT | GIMBAL_MANAGER_FLAGS_NEUTRAL);
        SETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
		break;
    }
*/
}




// -- Task handlers --

bool MavlinkTelem::doTaskGimbalAndGimbalClient(void)
{
    if (!_task[TASK_GIMBAL]) return false; // no task pending

    // if there is no gimbal, then there is also no gimbal manager which needs to be served
    // so first if()-checking for gimbal, and then for gimbalmanager is ok

    if (!gimbal.compid) { _task[TASK_GIMBAL] = 0; return false; } // no gimbal

    if (_task[TASK_GIMBAL] & TASK_SENDCMD_DO_MOUNT_CONFIGURE) {
        RESETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_MOUNT_CONFIGURE);
        generateCmdDoMountConfigure(_sysid, autopilot.compid, _t_gimbal_mode);
        return true; //do only one per loop
    }
    if (_task[TASK_GIMBAL] & TASK_SENDCMD_DO_MOUNT_CONTROL) {
        RESETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_MOUNT_CONTROL);
        generateCmdDoMountControl(_sysid, autopilot.compid, _t_gimbal_pitch_deg, _t_gimbal_yaw_deg);
        return true; //do only one per loop
    }

    if (_task[TASK_GIMBAL] & TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION) {
        RESETTASK(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION);
        generateCmdRequestGimbalDeviceInformation(_sysid, gimbal.compid);
        return true; //do only one per loop
    }
    if (_task[TASK_GIMBAL] & TASK_SENDMSG_GIMBAL_DEVICE_SET_ATTITUDE) { // we shouldn't use this one!
        RESETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_DEVICE_SET_ATTITUDE);
        generateGimbalDeviceSetAttitude(_sysid, gimbal.compid,
        		_t_gimbaldevice_pitch_deg, _t_gimbaldevice_yaw_deg, _t_gimbaldevice_flags);
        return true; //do only one per loop
    }

    if (!gimbalmanager.compid) { return false; } // no gimbal manager //we should clear GM tasks, but it's not worth it, so we don't

    if (_task[TASK_GIMBAL] & TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE) {
        RESETTASK(TASK_GIMBAL, TASK_SENDMSG_GIMBAL_MANAGER_SET_ATTITUDE);
        generateGimbalManagerSetAttitude(_sysid, gimbalmanager.compid,
        		gimbal.compid,
        		_t_gimbalmanager_setatt_pitch_deg, _t_gimbalmanager_setatt_yaw_deg, _t_gimbalmanager_setatt_flags);
        return true; //do only one per loop
    }
    if (_task[TASK_GIMBAL] & TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE) {
        RESETTASK(TASK_GIMBAL, TASK_SENDCMD_DO_GIMBAL_MANAGER_ATTITUDE);
        generateCmdDoGimbalManagerAttitude(_sysid, gimbalmanager.compid,
        		gimbal.compid,
        		_t_gimbalmanager_cmd_pitch_deg, _t_gimbalmanager_cmd_yaw_deg, _t_gimbalmanager_cmd_flags);
        return true; //do only one per loop
    }
    if (_task[TASK_GIMBAL] & TASK_SENDREQUEST_GIMBAL_MANAGER_INFORMATION) {
        RESETTASK(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_MANAGER_INFORMATION);
        generateCmdRequestGimbalManagerInformation(_sysid, gimbalmanager.compid);
        return true; //do only one per loop
    }

    return false;
}



// -- Handle incoming MAVLink messages, which are for the Gimbal --

void MavlinkTelem::handleMessageGimbal(void)
{
    gimbal.is_receiving = MAVLINK_TELEM_RECEIVING_TIMEOUT; //we accept any msg from the gimbal to indicate it is alive

	switch (_msg.msgid) {

	case MAVLINK_MSG_ID_HEARTBEAT: {
		mavlink_heartbeat_t payload;
		mavlink_msg_heartbeat_decode(&_msg, &payload);
		gimbal.system_status = payload.system_status;
		gimbal.custom_mode = payload.custom_mode;
		gimbal.is_armed = (payload.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false;
        gimbal.is_standby = (payload.system_status <= MAV_STATE_STANDBY) ? true : false;
        gimbal.is_critical = (payload.system_status >= MAV_STATE_CRITICAL) ? true : false;
		gimbal.prearm_ok = (payload.custom_mode & 0x80000000) ? false : true;
        INCU8(gimbal.updated);
        //gimbal.is_receiving = MAVLINK_TELEM_RECEIVING_TIMEOUT;
		}break;

    case MAVLINK_MSG_ID_ATTITUDE: {
    	mavlink_attitude_t payload;
        mavlink_msg_attitude_decode(&_msg, &payload);
        gimbalAtt.roll_deg = payload.roll * FRADTODEG;
        gimbalAtt.pitch_deg = payload.pitch * FRADTODEG;
        gimbalAtt.yaw_deg_relative = payload.yaw * FRADTODEG;
        if (gimbalAtt.yaw_deg_absolute > 180.0f) gimbalAtt.yaw_deg_absolute -= 360.0f;
        gimbalAtt.yaw_deg_absolute = gimbalAtt.yaw_deg_relative + att.yaw_rad * FRADTODEG;
        if (gimbalAtt.yaw_deg_absolute < -180.0f) gimbalAtt.yaw_deg_absolute += 360.0f;
        INCU8(gimbalAtt.updated);
        gimbalAtt.flags = -1;
		}break;

    case MAVLINK_MSG_ID_MOUNT_STATUS: {
    	mavlink_mount_status_t payload;
        mavlink_msg_mount_status_decode(&_msg, &payload);
        gimbalAtt.roll_deg = ((float)payload.pointing_b * 0.01f);
        gimbalAtt.pitch_deg = ((float)payload.pointing_a * 0.01f);
        gimbalAtt.yaw_deg_relative = ((float)payload.pointing_c * 0.01f);
        if (gimbalAtt.yaw_deg_absolute > 180.0f) gimbalAtt.yaw_deg_absolute -= 360.0f;
        gimbalAtt.yaw_deg_absolute = gimbalAtt.yaw_deg_relative + att.yaw_rad * FRADTODEG;
        if (gimbalAtt.yaw_deg_absolute < -180.0f) gimbalAtt.yaw_deg_absolute += 360.0f;
        INCU8(gimbalAtt.updated);
        gimbalAtt.flags = -1;
		}break;

    case MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS: {
    	mavlink_gimbal_device_attitude_status_t payload;
        mavlink_msg_gimbal_device_attitude_status_decode(&_msg, &payload);
        calc_G_angles_deg_from_q(&gimbalAtt.roll_deg, &gimbalAtt.pitch_deg, &gimbalAtt.yaw_deg_relative, payload.q);
        gimbalAtt.yaw_deg_absolute = gimbalAtt.yaw_deg_relative + att.yaw_rad * FRADTODEG;
        if (gimbalAtt.yaw_deg_absolute < -180.0f) gimbalAtt.yaw_deg_absolute += 360.0f;
        gimbalAtt.flags = payload.flags;
        INCU8(gimbalAtt.updated);
        // update gimbal manager flags, if gimbal manager has been discovered
        if (gimbalmanager.compid) {
        	gimbalmanagerStatus.flags &=~ 0x0000FFFF; //clear
        	gimbalmanagerStatus.flags |= (uint16_t)payload.flags; //set
        }
		}break;

    case MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION: {
    	mavlink_gimbal_device_information_t payload;
        mavlink_msg_gimbal_device_information_decode(&_msg, &payload);
		memset(gimbaldeviceInfo.vendor_name, 0, 32+1);
		memcpy(gimbaldeviceInfo.vendor_name, payload.vendor_name, 32);
		memset(gimbaldeviceInfo.model_name, 0, 32+1);
		memcpy(gimbaldeviceInfo.model_name, payload.model_name, 32);
		gimbaldeviceInfo.firmware_version = payload.firmware_version;
        gimbaldeviceInfo.cap_flags = payload.cap_flags;
        INCU8(gimbaldeviceInfo.updated);
        clear_request(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION);
        gimbal.requests_waiting_mask &=~ GIMBAL_REQUESTWAITING_GIMBAL_DEVICE_INFORMATION;
        gimbalmanager.requests_waiting_mask &=~ GIMBAL_REQUESTWAITING_GIMBAL_DEVICE_INFORMATION;
        // we do not copy to the gimbal manager capability flags, we rely on the gimbal manager to provide the correct ones
	    }break;

	}
}



// -- Handle incoming MAVLink messages, which are for the Gimbal Client --

void MavlinkTelem::handleMessageGimbalClient(void)
{
    gimbalmanager.is_receiving = MAVLINK_TELEM_RECEIVING_TIMEOUT; //we accept any msg from the manager to indicate it is alive

	switch (_msg.msgid) {

	case MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS: {
		mavlink_gimbal_manager_status_t payload;
		mavlink_msg_gimbal_manager_status_decode(&_msg, &payload);
		if (payload.gimbal_device_id != gimbal.compid) break; //not for us
		gimbalmanagerStatus.flags = payload.flags;
        INCU8(gimbalmanagerStatus.updated);
        //gimbalmanager.is_receiving = MAVLINK_TELEM_RECEIVING_TIMEOUT;
		}break;

    case MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION: {
		mavlink_gimbal_manager_information_t payload;
		mavlink_msg_gimbal_manager_information_decode(&_msg, &payload);
		if (payload.gimbal_device_id != gimbal.compid) break; //not for us
		gimbalmanagerInfo.cap_flags = payload.cap_flags;
        INCU8(gimbalmanagerInfo.updated);
        clear_request(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_MANAGER_INFORMATION);
        gimbalmanager.requests_waiting_mask &=~ GIMBAL_REQUESTWAITING_GIMBAL_MANAGER_INFORMATION;
		}break;

	}
}



// -- Startup Requests --

void MavlinkTelem::setGimbalStartupRequests(void)
{
	set_request(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION, 10, 200-12);
}


void MavlinkTelem::setGimbalClientStartupRequests(void)
{
	set_request(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_DEVICE_INFORMATION, 10, 200-12);
	set_request(TASK_GIMBAL, TASK_SENDREQUEST_GIMBAL_MANAGER_INFORMATION, 10, 200-18);
}


// -- Resets --

void MavlinkTelem::_resetGimbalAndGimbalClient(void)
{
    _task[TASK_GIMBAL] = 0;

	gimbal.compid = 0;
	gimbal.is_receiving = 0;

    gimbal.requests_triggered = 0;
    gimbal.requests_waiting_mask = GIMBAL_REQUESTWAITING_ALL;
    gimbal.is_initialized = false;

	gimbal.system_status = MAV_STATE_UNINIT;
	gimbal.custom_mode = 0;
	gimbal.is_armed = false;
	gimbal.is_standby = true;
	gimbal.is_critical = false;
	gimbal.prearm_ok = false;
	gimbal.updated = 0;

    gimbalAtt.roll_deg = 0.0f;
    gimbalAtt.pitch_deg = 0.0f;
    gimbalAtt.yaw_deg_relative = 0.0f;
    gimbalAtt.yaw_deg_absolute = 0.0f;
    gimbalAtt.flags = -1;
    gimbalAtt.updated = 0;

    _resetGimbalClient();
}


void MavlinkTelem::_resetGimbalClient(void)
{
    //_task[TASK_GIMBAL] = 0; // only reset gimbal client tasks, but not very important

	gimbalmanager.compid = 0;
	gimbalmanager.is_receiving = 0;

    gimbalmanager.requests_triggered = 0;
    gimbalmanager.requests_waiting_mask = GIMBALCLIENT_REQUESTWAITING_ALL; //this prevents it ever becomes initialized = true
    gimbalmanager.is_initialized = false;

	gimbalmanager.updated = 0;

	gimbaldeviceInfo.vendor_name[0] = 0;
	gimbaldeviceInfo.model_name[0] = 0;
	gimbaldeviceInfo.firmware_version = 0;
	gimbaldeviceInfo.cap_flags = 0;
	gimbaldeviceInfo.updated = 0;

    gimbalmanagerInfo.cap_flags = 0;
	gimbalmanagerInfo.updated = 0;

	gimbalmanagerStatus.flags = 0;
	gimbalmanagerStatus.updated = 0;
}


