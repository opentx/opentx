if(PCB STREQUAL X9D OR PCB STREQUAL X9DP OR PCB STREQUAL SKY9X)
  set(EEPROM_CONVERSIONS 216)
endif()

if(PCB STREQUAL X12S OR PCB STREQUAL X10 OR PCB STREQUAL X7 OR PCB STREQUAL XLITE)
  set(EEPROM_CONVERSIONS 218)
endif()

if(EEPROM_CONVERSIONS)
  add_definitions(-DEEPROM_CONVERSIONS=${EEPROM_CONVERSIONS})
  set(SRC
    ${SRC}
    storage/conversions/conversions.cpp
    )
endif()

if(EEPROM_CONVERSIONS GREATER 215)
  set(SRC
    ${SRC}
    storage/conversions/conversions_216_218.cpp
    )
endif()

if(EEPROM_CONVERSIONS GREATER 217)
  set(SRC
    ${SRC}
    storage/conversions/conversions_218_219.cpp
  )
endif()
