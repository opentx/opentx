include(gui/${GUI_DIR}/CMakeLists.txt)
include_directories(gui gui/${GUI_DIR})

set(SRC
  ${SRC}
  gui/navigation/common.cpp
  gui/navigation/navigation_${NAVIGATION_TYPE}.cpp
  gui/screenshot.cpp
  )

string(TOUPPER ${NAVIGATION_TYPE} NAVIGATION_TYPE)
add_definitions(-DNAVIGATION_${NAVIGATION_TYPE})
