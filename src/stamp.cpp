#include "open9x.h"
#include "stamp-open9x.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

const pm_char stamp1[] PROGMEM = "VERS: V" VERS_STR;
const pm_char stamp2[] PROGMEM = " SVN: " SVN_STR;
const pm_char stamp3[] PROGMEM = "DATE: " DATE_STR;
const pm_char stamp4[] PROGMEM = "TIME: " TIME_STR;
const pm_char stamp5[] PROGMEM = " MOD: " MOD_STR;


