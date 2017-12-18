set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
  navigation.cpp
  popups.cpp
  widgets.cpp
  menus.cpp
  menu_model.cpp
  model_select.cpp
  model_setup.cpp
  model_logical_switches.cpp
  model_special_functions.cpp
  model_telemetry.cpp
  menu_radio.cpp
  radio_setup.cpp
  radio_trainer.cpp
  radio_version.cpp
  radio_calibration.cpp
  view_main.cpp
  view_statistics.cpp
  )

if(GUI_DIR STREQUAL 128x64)
  if (ARCH STREQUAL ARM)
    set(GUI_SRC
      ${GUI_SRC}
      model_outputs_arm.cpp
      )
  else()
    set(GUI_SRC
      ${GUI_SRC}
      model_outputs_avr.cpp
      )
  endif()
else()
  set(GUI_SRC
    ${GUI_SRC}
    model_outputs.cpp
    )
endif()

set(SRC
  ${SRC}
  gui/common/widgets.cpp
  gui/common/navigation.cpp
  )

include_directories(gui gui/${GUI_DIR})

if(GUI_DIR STREQUAL 128x64 OR GUI_DIR STREQUAL 212x64)
  include(gui/common/stdlcd/CMakeLists.txt)
endif()

if(ARCH STREQUAL ARM)
  set(SRC
    ${SRC}
    gui/common/arm/widgets.cpp
    )
endif()

if(PCB STREQUAL X12S OR PCB STREQUAL X10)
  if(HELI)
    set(HELI_SRC gui/common/colorlcd/model_heli.cpp)
  endif()

  set(SRC
    ${SRC}
    gui/common/colorlcd/widgets.cpp
    ${HELI_SRC}
    )
endif()
