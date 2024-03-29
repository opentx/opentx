add_definitions(-DGRAPHICS)

if(LCD_DEPTH EQUAL 4)
  set(LCD_SRC lcd_4bits.cpp)
else()
  set(LCD_SRC lcd_1bit.cpp)
endif()

set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/${LCD_SRC}
  ../common/stdlcd/menus.cpp
  ../common/stdlcd/draw_functions.cpp
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
  ../common/stdlcd/view_telemetry.cpp
  model_curve_edit.cpp
  )

if(PXX2 OR LUA OR MULTIMODULE)
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
    ../common/stdlcd/radio_power_meter.cpp
    )
endif()

if(GHOST)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_ghost_menu.cpp
    )
endif()

if(PXX2 OR MULTIMODULE)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_spectrum_analyser.cpp
    )
endif()

