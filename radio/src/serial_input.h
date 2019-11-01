#ifndef serial_input_h
#define serial_input_h
#include "opentx.h"
#include "myeeprom.h"

extern Fifo<uint8_t,256> serialInputFifo;

// Serial input channels
extern int16_t serialInput[MAX_NUM_SERIAL];
extern int8_t serialBytesAvailable;
extern int32_t serialData;
void processSerialInput();

#endif