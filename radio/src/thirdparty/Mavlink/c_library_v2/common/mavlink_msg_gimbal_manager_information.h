#pragma once
// MESSAGE GIMBAL_MANAGER_INFORMATION PACKING

#define MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION 280

MAVPACKED(
typedef struct __mavlink_gimbal_manager_information_t {
 uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
 uint32_t cap_flags; /*<  Bitmap of gimbal capability flags.*/
 float tilt_max; /*< [rad] Maximum tilt/pitch angle (positive: up, negative: down)*/
 float tilt_min; /*< [rad] Minimum tilt/pitch angle (positive: up, negative: down)*/
 float tilt_rate_max; /*< [rad/s] Maximum tilt/pitch angular rate (positive: up, negative: down)*/
 float pan_max; /*< [rad] Maximum pan/yaw angle (positive: to the right, negative: to the left)*/
 float pan_min; /*< [rad] Minimum pan/yaw angle (positive: to the right, negative: to the left)*/
 float pan_rate_max; /*< [rad/s] Minimum pan/yaw angular rate (positive: to the right, negative: to the left)*/
 uint8_t gimbal_device_id; /*<  Gimbal device ID that this gimbal manager is responsible for.*/
}) mavlink_gimbal_manager_information_t;

#define MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN 33
#define MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN 33
#define MAVLINK_MSG_ID_280_LEN 33
#define MAVLINK_MSG_ID_280_MIN_LEN 33

#define MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC 166
#define MAVLINK_MSG_ID_280_CRC 166



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_GIMBAL_MANAGER_INFORMATION { \
    280, \
    "GIMBAL_MANAGER_INFORMATION", \
    9, \
    {  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_gimbal_manager_information_t, time_boot_ms) }, \
         { "cap_flags", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_gimbal_manager_information_t, cap_flags) }, \
         { "gimbal_device_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_gimbal_manager_information_t, gimbal_device_id) }, \
         { "tilt_max", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_gimbal_manager_information_t, tilt_max) }, \
         { "tilt_min", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_gimbal_manager_information_t, tilt_min) }, \
         { "tilt_rate_max", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_gimbal_manager_information_t, tilt_rate_max) }, \
         { "pan_max", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_gimbal_manager_information_t, pan_max) }, \
         { "pan_min", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_gimbal_manager_information_t, pan_min) }, \
         { "pan_rate_max", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_gimbal_manager_information_t, pan_rate_max) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_GIMBAL_MANAGER_INFORMATION { \
    "GIMBAL_MANAGER_INFORMATION", \
    9, \
    {  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_gimbal_manager_information_t, time_boot_ms) }, \
         { "cap_flags", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_gimbal_manager_information_t, cap_flags) }, \
         { "gimbal_device_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_gimbal_manager_information_t, gimbal_device_id) }, \
         { "tilt_max", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_gimbal_manager_information_t, tilt_max) }, \
         { "tilt_min", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_gimbal_manager_information_t, tilt_min) }, \
         { "tilt_rate_max", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_gimbal_manager_information_t, tilt_rate_max) }, \
         { "pan_max", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_gimbal_manager_information_t, pan_max) }, \
         { "pan_min", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_gimbal_manager_information_t, pan_min) }, \
         { "pan_rate_max", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_gimbal_manager_information_t, pan_rate_max) }, \
         } \
}
#endif

/**
 * @brief Pack a gimbal_manager_information message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param cap_flags  Bitmap of gimbal capability flags.
 * @param gimbal_device_id  Gimbal device ID that this gimbal manager is responsible for.
 * @param tilt_max [rad] Maximum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_min [rad] Minimum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_rate_max [rad/s] Maximum tilt/pitch angular rate (positive: up, negative: down)
 * @param pan_max [rad] Maximum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_min [rad] Minimum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_rate_max [rad/s] Minimum pan/yaw angular rate (positive: to the right, negative: to the left)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gimbal_manager_information_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint32_t time_boot_ms, uint32_t cap_flags, uint8_t gimbal_device_id, float tilt_max, float tilt_min, float tilt_rate_max, float pan_max, float pan_min, float pan_rate_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_uint32_t(buf, 4, cap_flags);
    _mav_put_float(buf, 8, tilt_max);
    _mav_put_float(buf, 12, tilt_min);
    _mav_put_float(buf, 16, tilt_rate_max);
    _mav_put_float(buf, 20, pan_max);
    _mav_put_float(buf, 24, pan_min);
    _mav_put_float(buf, 28, pan_rate_max);
    _mav_put_uint8_t(buf, 32, gimbal_device_id);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN);
#else
    mavlink_gimbal_manager_information_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.cap_flags = cap_flags;
    packet.tilt_max = tilt_max;
    packet.tilt_min = tilt_min;
    packet.tilt_rate_max = tilt_rate_max;
    packet.pan_max = pan_max;
    packet.pan_min = pan_min;
    packet.pan_rate_max = pan_rate_max;
    packet.gimbal_device_id = gimbal_device_id;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
}

/**
 * @brief Pack a gimbal_manager_information message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param cap_flags  Bitmap of gimbal capability flags.
 * @param gimbal_device_id  Gimbal device ID that this gimbal manager is responsible for.
 * @param tilt_max [rad] Maximum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_min [rad] Minimum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_rate_max [rad/s] Maximum tilt/pitch angular rate (positive: up, negative: down)
 * @param pan_max [rad] Maximum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_min [rad] Minimum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_rate_max [rad/s] Minimum pan/yaw angular rate (positive: to the right, negative: to the left)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gimbal_manager_information_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint32_t time_boot_ms,uint32_t cap_flags,uint8_t gimbal_device_id,float tilt_max,float tilt_min,float tilt_rate_max,float pan_max,float pan_min,float pan_rate_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_uint32_t(buf, 4, cap_flags);
    _mav_put_float(buf, 8, tilt_max);
    _mav_put_float(buf, 12, tilt_min);
    _mav_put_float(buf, 16, tilt_rate_max);
    _mav_put_float(buf, 20, pan_max);
    _mav_put_float(buf, 24, pan_min);
    _mav_put_float(buf, 28, pan_rate_max);
    _mav_put_uint8_t(buf, 32, gimbal_device_id);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN);
#else
    mavlink_gimbal_manager_information_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.cap_flags = cap_flags;
    packet.tilt_max = tilt_max;
    packet.tilt_min = tilt_min;
    packet.tilt_rate_max = tilt_rate_max;
    packet.pan_max = pan_max;
    packet.pan_min = pan_min;
    packet.pan_rate_max = pan_rate_max;
    packet.gimbal_device_id = gimbal_device_id;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
}

/**
 * @brief Encode a gimbal_manager_information struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param gimbal_manager_information C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gimbal_manager_information_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_gimbal_manager_information_t* gimbal_manager_information)
{
    return mavlink_msg_gimbal_manager_information_pack(system_id, component_id, msg, gimbal_manager_information->time_boot_ms, gimbal_manager_information->cap_flags, gimbal_manager_information->gimbal_device_id, gimbal_manager_information->tilt_max, gimbal_manager_information->tilt_min, gimbal_manager_information->tilt_rate_max, gimbal_manager_information->pan_max, gimbal_manager_information->pan_min, gimbal_manager_information->pan_rate_max);
}

/**
 * @brief Encode a gimbal_manager_information struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param gimbal_manager_information C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gimbal_manager_information_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_gimbal_manager_information_t* gimbal_manager_information)
{
    return mavlink_msg_gimbal_manager_information_pack_chan(system_id, component_id, chan, msg, gimbal_manager_information->time_boot_ms, gimbal_manager_information->cap_flags, gimbal_manager_information->gimbal_device_id, gimbal_manager_information->tilt_max, gimbal_manager_information->tilt_min, gimbal_manager_information->tilt_rate_max, gimbal_manager_information->pan_max, gimbal_manager_information->pan_min, gimbal_manager_information->pan_rate_max);
}

/**
 * @brief Send a gimbal_manager_information message
 * @param chan MAVLink channel to send the message
 *
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param cap_flags  Bitmap of gimbal capability flags.
 * @param gimbal_device_id  Gimbal device ID that this gimbal manager is responsible for.
 * @param tilt_max [rad] Maximum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_min [rad] Minimum tilt/pitch angle (positive: up, negative: down)
 * @param tilt_rate_max [rad/s] Maximum tilt/pitch angular rate (positive: up, negative: down)
 * @param pan_max [rad] Maximum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_min [rad] Minimum pan/yaw angle (positive: to the right, negative: to the left)
 * @param pan_rate_max [rad/s] Minimum pan/yaw angular rate (positive: to the right, negative: to the left)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_gimbal_manager_information_send(mavlink_channel_t chan, uint32_t time_boot_ms, uint32_t cap_flags, uint8_t gimbal_device_id, float tilt_max, float tilt_min, float tilt_rate_max, float pan_max, float pan_min, float pan_rate_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_uint32_t(buf, 4, cap_flags);
    _mav_put_float(buf, 8, tilt_max);
    _mav_put_float(buf, 12, tilt_min);
    _mav_put_float(buf, 16, tilt_rate_max);
    _mav_put_float(buf, 20, pan_max);
    _mav_put_float(buf, 24, pan_min);
    _mav_put_float(buf, 28, pan_rate_max);
    _mav_put_uint8_t(buf, 32, gimbal_device_id);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, buf, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
#else
    mavlink_gimbal_manager_information_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.cap_flags = cap_flags;
    packet.tilt_max = tilt_max;
    packet.tilt_min = tilt_min;
    packet.tilt_rate_max = tilt_rate_max;
    packet.pan_max = pan_max;
    packet.pan_min = pan_min;
    packet.pan_rate_max = pan_rate_max;
    packet.gimbal_device_id = gimbal_device_id;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, (const char *)&packet, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
#endif
}

/**
 * @brief Send a gimbal_manager_information message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_gimbal_manager_information_send_struct(mavlink_channel_t chan, const mavlink_gimbal_manager_information_t* gimbal_manager_information)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_gimbal_manager_information_send(chan, gimbal_manager_information->time_boot_ms, gimbal_manager_information->cap_flags, gimbal_manager_information->gimbal_device_id, gimbal_manager_information->tilt_max, gimbal_manager_information->tilt_min, gimbal_manager_information->tilt_rate_max, gimbal_manager_information->pan_max, gimbal_manager_information->pan_min, gimbal_manager_information->pan_rate_max);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, (const char *)gimbal_manager_information, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
#endif
}

#if MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_gimbal_manager_information_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t time_boot_ms, uint32_t cap_flags, uint8_t gimbal_device_id, float tilt_max, float tilt_min, float tilt_rate_max, float pan_max, float pan_min, float pan_rate_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_uint32_t(buf, 4, cap_flags);
    _mav_put_float(buf, 8, tilt_max);
    _mav_put_float(buf, 12, tilt_min);
    _mav_put_float(buf, 16, tilt_rate_max);
    _mav_put_float(buf, 20, pan_max);
    _mav_put_float(buf, 24, pan_min);
    _mav_put_float(buf, 28, pan_rate_max);
    _mav_put_uint8_t(buf, 32, gimbal_device_id);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, buf, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
#else
    mavlink_gimbal_manager_information_t *packet = (mavlink_gimbal_manager_information_t *)msgbuf;
    packet->time_boot_ms = time_boot_ms;
    packet->cap_flags = cap_flags;
    packet->tilt_max = tilt_max;
    packet->tilt_min = tilt_min;
    packet->tilt_rate_max = tilt_rate_max;
    packet->pan_max = pan_max;
    packet->pan_min = pan_min;
    packet->pan_rate_max = pan_rate_max;
    packet->gimbal_device_id = gimbal_device_id;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, (const char *)packet, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_MIN_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_CRC);
#endif
}
#endif

#endif

// MESSAGE GIMBAL_MANAGER_INFORMATION UNPACKING


/**
 * @brief Get field time_boot_ms from gimbal_manager_information message
 *
 * @return [ms] Timestamp (time since system boot).
 */
static inline uint32_t mavlink_msg_gimbal_manager_information_get_time_boot_ms(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field cap_flags from gimbal_manager_information message
 *
 * @return  Bitmap of gimbal capability flags.
 */
static inline uint32_t mavlink_msg_gimbal_manager_information_get_cap_flags(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 * @brief Get field gimbal_device_id from gimbal_manager_information message
 *
 * @return  Gimbal device ID that this gimbal manager is responsible for.
 */
static inline uint8_t mavlink_msg_gimbal_manager_information_get_gimbal_device_id(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  32);
}

/**
 * @brief Get field tilt_max from gimbal_manager_information message
 *
 * @return [rad] Maximum tilt/pitch angle (positive: up, negative: down)
 */
static inline float mavlink_msg_gimbal_manager_information_get_tilt_max(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field tilt_min from gimbal_manager_information message
 *
 * @return [rad] Minimum tilt/pitch angle (positive: up, negative: down)
 */
static inline float mavlink_msg_gimbal_manager_information_get_tilt_min(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field tilt_rate_max from gimbal_manager_information message
 *
 * @return [rad/s] Maximum tilt/pitch angular rate (positive: up, negative: down)
 */
static inline float mavlink_msg_gimbal_manager_information_get_tilt_rate_max(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field pan_max from gimbal_manager_information message
 *
 * @return [rad] Maximum pan/yaw angle (positive: to the right, negative: to the left)
 */
static inline float mavlink_msg_gimbal_manager_information_get_pan_max(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field pan_min from gimbal_manager_information message
 *
 * @return [rad] Minimum pan/yaw angle (positive: to the right, negative: to the left)
 */
static inline float mavlink_msg_gimbal_manager_information_get_pan_min(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field pan_rate_max from gimbal_manager_information message
 *
 * @return [rad/s] Minimum pan/yaw angular rate (positive: to the right, negative: to the left)
 */
static inline float mavlink_msg_gimbal_manager_information_get_pan_rate_max(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Decode a gimbal_manager_information message into a struct
 *
 * @param msg The message to decode
 * @param gimbal_manager_information C-struct to decode the message contents into
 */
static inline void mavlink_msg_gimbal_manager_information_decode(const mavlink_message_t* msg, mavlink_gimbal_manager_information_t* gimbal_manager_information)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    gimbal_manager_information->time_boot_ms = mavlink_msg_gimbal_manager_information_get_time_boot_ms(msg);
    gimbal_manager_information->cap_flags = mavlink_msg_gimbal_manager_information_get_cap_flags(msg);
    gimbal_manager_information->tilt_max = mavlink_msg_gimbal_manager_information_get_tilt_max(msg);
    gimbal_manager_information->tilt_min = mavlink_msg_gimbal_manager_information_get_tilt_min(msg);
    gimbal_manager_information->tilt_rate_max = mavlink_msg_gimbal_manager_information_get_tilt_rate_max(msg);
    gimbal_manager_information->pan_max = mavlink_msg_gimbal_manager_information_get_pan_max(msg);
    gimbal_manager_information->pan_min = mavlink_msg_gimbal_manager_information_get_pan_min(msg);
    gimbal_manager_information->pan_rate_max = mavlink_msg_gimbal_manager_information_get_pan_rate_max(msg);
    gimbal_manager_information->gimbal_device_id = mavlink_msg_gimbal_manager_information_get_gimbal_device_id(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN? msg->len : MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN;
        memset(gimbal_manager_information, 0, MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN);
    memcpy(gimbal_manager_information, _MAV_PAYLOAD(msg), len);
#endif
}
