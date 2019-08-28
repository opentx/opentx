set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
  popups.cpp
  widgets.cpp
  menu_model.cpp
  model_select.cpp
  model_setup.cpp
  model_module_options.cpp
  model_receiver_options.cpp
  model_failsafe.cpp
  model_logical_switches.cpp
  model_special_functions.cpp
  model_outputs.cpp
  model_telemetry.cpp
  model_telemetry_sensor.cpp
  menu_radio.cpp
  radio_setup.cpp
  radio_trainer.cpp
  radio_version.cpp
  radio_hardware.cpp
  radio_calibration.cpp
  view_main.cpp
  view_statistics.cpp
  )

if(FLIGHT_MODES)
  set(GUI_SRC
    ${GUI_SRC}
    model_flightmodes.cpp
    )
endif()

if(LUA STREQUAL YES)
  set(GUI_SRC
    ${GUI_SRC}
    model_custom_scripts.cpp
    )
endif()

include(gui/common/colorlcd/CMakeLists.txt)
