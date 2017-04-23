set(GUI_ELSE_SRC
  ${GUI_ELSE_SRC}
  lcd.cpp
  fonts.cpp
  widgets.cpp
  menus.cpp
  navigation.cpp
  splash.cpp
  popups.cpp
  view_main.cpp
  radio_calibration.cpp
  )

if(NOT ${PCB} STREQUAL X7)
set(GUI_SRC
  ${GUI_SRC}
  model_select.cpp
  radio_trainer.cpp
  )
endif()

set(GUI_SRC
  ${GUI_SRC}
  menu_model.cpp
  # model_setup.cpp
  # model_logical_switches.cpp
  # model_special_functions.cpp
  # model_telemetry.cpp
  # menu_radio.cpp
  radio_setup.cpp
  # radio_version.cpp
  # view_statistics.cpp
  )

if(GUI_DIR STREQUAL 128x64-ACAIR)
  set(GUI_SRC
    ${GUI_SRC}
    model_outputs.cpp
    model_controls.cpp
    )
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

if(GUI_DIR STREQUAL 128x64-ACAIR OR GUI_DIR STREQUAL 212x64)
  include(gui/common/stdlcd/CMakeLists.txt)
endif()

if(ARCH STREQUAL ARM)
  set(SRC
    ${SRC}
    gui/common/arm/widgets.cpp
    )
endif()

if(PCB STREQUAL FLAMENCO OR PCB STREQUAL X12S OR PCB STREQUAL X10)
  if(HELI)
    set(HELI_SRC gui/common/colorlcd/model_heli.cpp)
  endif()

  set(SRC
    ${SRC}
    gui/common/colorlcd/widgets.cpp
    ${HELI_SRC}
    )
endif()
