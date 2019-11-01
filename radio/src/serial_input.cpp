
#include "serial_input.h"
#include "rtos.h"
int16_t serialInput[MAX_NUM_SERIAL];
int8_t serialBytesAvailable = 0;
Fifo<uint8_t,256> serialInputFifo;

int32_t serialData = 0;
void processSerialInput()
{
    uint8_t c;

    while (!serialInputFifo.pop(c))
    {
        RTOS_WAIT_MS(10); // 20ms
    }
}