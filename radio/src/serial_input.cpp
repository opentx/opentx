#include "serial_input.h"

int16_t serialInput[MAX_NUM_SERIAL];
int8_t serialBytesAvailable = 0;
Fifo<uint8_t,256> serialInputFifo;

int32_t serialData = 0;
void processSerialInput()
{
    uint8_t c;

    while (!serialInputFifo.pop(c))
    {
        CoTickDelay(10); // 20ms
    }
}