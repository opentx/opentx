if(NOT PCB STREQUAL X3)
  add_definitions(-DEEPROM_CONVERSIONS)
  set(SRC
    ${SRC}
    storage/conversions/conversions.cpp
    storage/conversions/conversions_216_218.cpp
    storage/conversions/conversions_218_219.cpp
  )
endif()
