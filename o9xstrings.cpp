#include "gruvin9x.h"

#ifdef TRANSLATIONS_FR
#include "translations/fr.h"
#else
#endif

#include "translations/en.h"

// The non-0-terminated-strings
const prog_char APM STR_OPEN9X[] =
    TR_OFFON
    TR_MMMINV
    TR_NCHANNELS
    TR_BEEPER
    TR_ADCFILTER
    TR_WARNSW
    TR_TRNMODE
    TR_TRNCHN
#if defined(PCBV3)
    TR_DATETIME
#endif
    ;

// The 0-terminated-strings
const prog_char APM STR_POPUPS[] = TR_POPUPS;
