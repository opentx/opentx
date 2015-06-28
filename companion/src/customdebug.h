#ifndef customdebug_h
#define customdebug_h

#include <QDebug>


// Controls the generation of debug output for EEPROM import
#if defined(DEBUG_MODEL_IMPORT)
  inline QDebug eepromImportDebug() { return QDebug(QtDebugMsg); }
#else
  #undef eepromImportDebug
  inline QNoDebug eepromImportDebug() { return QNoDebug(); }
#endif


#endif