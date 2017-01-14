include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(storage_NAMES
  hexinterface
  mountlist
  rlefile
  appdata
  firmwareinterface
  storage
  bineeprom
  eepe
  hexeeprom
  categorized
  sdcard
  otx
)

set(storage_SRCS
)

set(storage_HDRS
)

foreach(name ${storage_NAMES})
  set(storage_SRCS ${storage_SRCS} ${name}.cpp)
  set(storage_HDRS ${storage_HDRS} ${name}.h)
endforeach()

add_library(storage ${storage_SRCS})
target_link_libraries(storage Qt5::Core Qt5::Gui Qt5::Xml Qt5::Widgets)