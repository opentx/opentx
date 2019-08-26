add_definitions(-DGRAPHICS)

set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/menus.cpp
  ../common/stdlcd/widgets.cpp
  ../common/stdlcd/popups.cpp
  ../common/stdlcd/model_inputs.cpp
  ../common/stdlcd/model_mixes.cpp
  ../common/stdlcd/view_text.cpp
  ../common/stdlcd/model_module_options.cpp
  ../common/stdlcd/model_receiver_options.cpp
  ../common/stdlcd/model_notes.cpp
  ../common/stdlcd/model_curves.cpp
  ../common/stdlcd/radio_hardware.cpp
  ../common/stdlcd/radio_version.cpp
  ../common/stdlcd/radio_trainer.cpp
  model_curve_edit.cpp
  )

if(PXX2 OR LUA)
  set(GUI_SRC ${GUI_SRC} ../common/stdlcd/radio_tools.cpp)
endif()

if(HELI)
  set(GUI_SRC ${GUI_SRC} ../common/stdlcd/model_heli.cpp)
endif()

if(SDCARD)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_sdmanager.cpp
    )
endif()

if(PXX2)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_spectrum_analyser.cpp
    ../common/stdlcd/radio_power_meter.cpp
    )
endif()

