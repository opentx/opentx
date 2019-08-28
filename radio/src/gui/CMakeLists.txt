include(gui/${GUI_DIR}/CMakeLists.txt)
include_directories(gui gui/${GUI_DIR})

if(NOT GUI_DIR STREQUAL colorlcd)
  set(SRC
    ${SRC}
    gui/navigation/common.cpp
    gui/navigation/navigation_${NAVIGATION_TYPE}.cpp
    )
  string(TOUPPER ${NAVIGATION_TYPE} NAVIGATION_TYPE)
  add_definitions(-DNAVIGATION_${NAVIGATION_TYPE})
endif()

set(SRC
  ${SRC}
  gui/screenshot.cpp
  )
