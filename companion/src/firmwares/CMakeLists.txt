
set(firmwares_SRCS
  boards.cpp
  curvereference.cpp
  customfunctiondata.cpp
  eeprominterface.cpp
  generalsettings.cpp
  gvardata.cpp
  io_data.cpp
  logicalswitchdata.cpp
  modeldata.cpp
  moduledata.cpp
  multiprotocols.cpp
  radiodata.cpp
  radiodataconversionstate.cpp
  rawsource.cpp
  rawswitch.cpp
  sensordata.cpp
  telem_data.cpp
  er9x/er9xeeprom.cpp
  er9x/er9xinterface.cpp
  ersky9x/ersky9xeeprom.cpp
  ersky9x/ersky9xinterface.cpp
  opentx/opentxeeprom.cpp
  opentx/opentxinterface.cpp
  )

string(REPLACE ".cpp" ".h" firmwares_HDRS "${firmwares_SRCS}")

list(APPEND firmwares_HDRS
  eepromimportexport.h
  moduledata.h
  )

add_library(firmwares ${firmwares_SRCS} ${firmwares_HDRS})
target_link_libraries(firmwares ${CPN_COMMON_LIB} Qt5::Core Qt5::Xml Qt5::Widgets)
target_include_directories(firmwares
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/er9x"
    "${CMAKE_CURRENT_LIST_DIR}/ersky9x"
    "${CMAKE_CURRENT_LIST_DIR}/opentx"
  )
