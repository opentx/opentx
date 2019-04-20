set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/widgets.cpp
  ../common/stdlcd/popups.cpp
  )

add_definitions(-DGRAPHICS)
set(GUI_SRC
 ${GUI_SRC}
  ../common/stdlcd/model_inputs.cpp
  ../common/stdlcd/model_mixes.cpp
  ../common/stdlcd/view_text.cpp
)
set(CURVE_EDIT_SRC )

set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/model_curves.cpp
  model_curve_edit.cpp
  )

if(HELI)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/model_heli.cpp)
endif()

if(SDCARD)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_sdmanager.cpp
    ../common/stdlcd/radio_tools.cpp
    )
endif()

if(PXX2)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_spectrum_analyser.cpp
    ../common/stdlcd/radio_power_meter.cpp
    )
endif()

