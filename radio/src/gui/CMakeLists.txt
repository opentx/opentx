set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
#  popups.cpp
  widgets.cpp
#  menus.cpp
  menu_model.cpp
  model_heli.cpp
#  model_select.cpp
  model_setup.cpp
#  model_logical_switches.cpp
  special_functions.cpp
#  model_telemetry.cpp
  menu_radio.cpp
  radio_setup.cpp
#  radio_trainer.cpp
  radio_version.cpp
#  radio_calibration.cpp
  view_main.cpp
#  view_statistics.cpp
  )

if(GUI_DIR STREQUAL 128x64)
   set(GUI_SRC
      ${GUI_SRC}
      model_outputs_arm.cpp
      )
else()
  set(GUI_SRC
    ${GUI_SRC}
    # model_outputs.cpp
    )
endif()

if(NOT GUI_DIR STREQUAL 320x480 AND NOT GUI_DIR STREQUAL 480x272)
  set(SRC
    ${SRC}
    gui/navigation/navigation_${NAVIGATION_TYPE}.cpp
    gui/navigation/common.cpp
    )
endif()

if(GUI_DIR STREQUAL 320x480 OR GUI_DIR STREQUAL 480x272)
  include(gui/libopenui/CMakeLists.txt)
  include_directories(gui/libopenui)
  add_definitions(-DLIBOPENUI)
  foreach(LIBOPENUI_FILE ${LIBOPENUI_SRC})
    set(SRC ${SRC} gui/libopenui/${LIBOPENUI_FILE})
  endforeach()
endif()

include_directories(gui gui/${GUI_DIR})

if(GUI_DIR STREQUAL 128x64 OR GUI_DIR STREQUAL 212x64)
  include(gui/common/stdlcd/CMakeLists.txt)
endif()

if(ARCH STREQUAL ARM)
  set(SRC
    ${SRC}
#    gui/common/widgets.cpp
#    gui/common/arm/widgets.cpp
    )
endif()

if(PCB STREQUAL X12S OR PCB STREQUAL X10 OR PCB STREQUAL NV14)
  if(HELI)
#    set(HELI_SRC gui/common/colorlcd/model_heli.cpp)
  endif()

  set(SRC
    ${SRC}
    # gui/common/colorlcd/widgets.cpp
#    ${HELI_SRC}
    )
endif()
