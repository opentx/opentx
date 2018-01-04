#pragma once

#define MAVLINK_BAUDRATE      57600

extern bool processMavlinkTelemetryData(uint8_t data);
extern void wakeupMavlinkTelemetry(bool recvInProgress);