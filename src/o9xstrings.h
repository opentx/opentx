#ifndef o9xstrings_h
#define o9xstrings_h

#ifdef TRANSLATIONS_FR
#include "translations/fr.h"
#else
#endif

#include "translations/en.h"

#define PSIZE(x) ( sizeof(x) - 1 )

// The non-0-terminated-strings

extern const PROGMEM char STR_OPEN9X[];

#define OFS_OFFON     0
#define OFS_MMMINV    (OFS_OFFON + PSIZE(TR_OFFON))
#define OFS_NCHANNELS (OFS_MMMINV + PSIZE(TR_MMMINV))
#define OFS_BEEPER    (OFS_NCHANNELS + PSIZE(TR_NCHANNELS))
#define OFS_ADCFILTER (OFS_BEEPER + PSIZE(TR_BEEPER))
#define OFS_WARNSW    (OFS_ADCFILTER + PSIZE(TR_ADCFILTER))
#define OFS_TRNMODE   (OFS_WARNSW + PSIZE(TR_WARNSW))
#define OFS_TRNCHN    (OFS_TRNMODE + PSIZE(TR_TRNMODE))
#if defined(PCBV3)
#define OFS_DATETIME  (OFS_TRNCHN + PSIZE(TR_TRNCHN))
#endif

#define STR_OFFON     (STR_OPEN9X + OFS_OFFON)
#define STR_MMMINV    (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS (STR_OPEN9X + OFS_NCHANNELS)
#define STR_BEEPER    (STR_OPEN9X + OFS_BEEPER)
#define STR_ADCFILTER (STR_OPEN9X + OFS_ADCFILTER)
#define STR_WARNSW    (STR_OPEN9X + OFS_WARNSW)
#define STR_TRNMODE   (STR_OPEN9X + OFS_TRNMODE)
#define STR_TRNCHN    (STR_OPEN9X + OFS_TRNCHN)

#if defined(PCBV3)
#define STR_DATETIME  (STR_OPEN9X + OFS_DATETIME)
#endif

// The 0-terminated-strings

extern const PROGMEM char STR_POPUPS[];

#endif
