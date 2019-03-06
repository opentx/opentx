include(gui/${GUI_DIR}/CMakeLists.txt)
include_directories(gui gui/${GUI_DIR})

if(NOT GUI_DIR STREQUAL 320x480 AND NOT GUI_DIR STREQUAL colorlcd)
  set(SRC
    ${SRC}
    gui/navigation/navigation_${NAVIGATION_TYPE}.cpp
    gui/navigation/common.cpp
    )
endif()
