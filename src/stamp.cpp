#include "open9x.h"
#include "stamp-open9x.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

const char APM  stamp1[] = "VERS: V" VERS_STR;
const char APM  stamp2[] = " SVN: " SVN_STR;
const char APM  stamp3[] = "DATE: " DATE_STR;
const char APM  stamp4[] = "TIME: " TIME_STR;
const char APM  stamp5[] = " MOD: " MOD_STR;


