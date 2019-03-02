set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
  #  popups.cpp
  widgets.cpp
  #  menus.cpp
  menu_model.cpp
  #  model_select.cpp
  model_setup.cpp
  model_heli.cpp
  #  model_logical_switches.cpp
  special_functions.cpp
  #  model_telemetry.cpp
  menu_radio.cpp
  radio_setup.cpp
  radio_trainer.cpp
  radio_version.cpp
  radio_hardware.cpp
  radio_calibration.cpp
  view_main.cpp
  #  view_statistics.cpp
  )

if(LUA STREQUAL YES)
  # TODO set(SRC ${SRC} lua/widgets.cpp)
  # TODO set(GUI_SRC ${GUI_SRC} model_custom_scripts.cpp)
endif()

# gui/${GUI_DIR}/modelslist.cpp

set(SRC
  ${SRC}
  stubs.cpp
  )

# includes libopenui
include(gui/libopenui/CMakeLists.txt)
include_directories(gui/libopenui)
add_definitions(-DLIBOPENUI)
foreach(LIBOPENUI_FILE ${LIBOPENUI_SRC})
  set(SRC ${SRC} gui/libopenui/${LIBOPENUI_FILE})
endforeach()
