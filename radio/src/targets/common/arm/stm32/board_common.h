// CPU Unique ID
#define LEN_CPU_UID                    (3*8+2)
void getCPUUniqueID(char * s);

uint16_t getBatteryVoltage();   // returns current battery voltage in 100mV steps
uint16_t getRTCBatteryVoltage();   // returns current battery voltage in 100mV steps
uint16_t getTemperature();        // return CPU temperature in 0.1Cx steps

// STM32F2 uses a 25K+25K voltage divider bridge to measure the battery voltage
// Measuring VBAT puts considerable drain (22 µA) on the battery instead of normal drain (~10 nA)
// Todo these crash simulator, unsure why
inline void disableVBatBridge()
{
#if !defined(SIMU)
  ADC->CCR &= ~(ADC_CCR_VBATE);
#endif
}

inline void enableVBatBridge()
{
#if !defined(SIMU)
  ADC->CCR |= ADC_CCR_VBATE;
#endif
}
