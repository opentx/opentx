#include "afhds3.h"
#include "macros.h"

QString Afhds3Data::protocolToString(unsigned int protocol) {
  const char * const afhds3Protocols[] = {
    "PWM/IBUS",
    "PWM/SBUS",
    "PPM/IBUS",
    "PPM/SBUS",
  };
   return CHECK_IN_ARRAY(afhds3Protocols, protocol);
}
