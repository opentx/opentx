#include "gruvin9x.h"
#include "stamp-gruvin9x.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

const char APM  stamp1[] = "VERS: V" DEFNUMSTR(VERS) "." DEFNUMSTR(TAG_VERS);
const char APM  stamp2[] = "DATE: " DATE_STR;
const char APM  stamp3[] = "TIME: " TIME_STR;
const char APM  stamp4[] = " SVN: " SVN_VERS;
const char APM  stamp5[] = " BLD: " DEFNUMSTR(BUILD_NUM);


