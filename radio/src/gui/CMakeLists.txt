set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
  popups.cpp
  widgets.cpp
  menus.cpp
  menu_model.cpp
  model_select.cpp
  model_setup.cpp
  model_outputs.cpp
  model_failsafe.cpp
  model_module_options.cpp
  model_receiver_options.cpp
  model_logical_switches.cpp
  model_special_functions.cpp
  model_telemetry.cpp
  model_telemetry_sensor.cpp
  menu_radio.cpp
  radio_setup.cpp
  radio_trainer.cpp
  radio_version.cpp
  radio_calibration.cpp
  view_main.cpp
  view_statistics.cpp
  )

set(SRC
  ${SRC}
  gui/common/widgets.cpp
  gui/navigation/navigation_${NAVIGATION_TYPE}.cpp
  gui/navigation/common.cpp
  )

if(NAVIGATION_TYPE STREQUAL 9x)
  add_definitions(-DNAVIGATION_9X)
elseif(NAVIGATION_TYPE STREQUAL x9d)
  add_definitions(-DNAVIGATION_X9D)
elseif(NAVIGATION_TYPE STREQUAL x7)
  add_definitions(-DNAVIGATION_X7)
elseif(NAVIGATION_TYPE STREQUAL xlite)
  add_definitions(-DNAVIGATION_XLITE)
endif()

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
