set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/widgets.cpp
  ../common/stdlcd/popups.cpp
  )

if(HELI)
  set(GUI_SRC ${GUI_SRC} ../common/stdlcd/model_heli.cpp)
endif()

if(ARCH STREQUAL ARM)
  add_definitions(-DGRAPHICS)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/model_inputs.cpp
    ../common/stdlcd/model_mixes.cpp
    ../common/stdlcd/view_text.cpp
    )
  set(CURVE_EDIT_SRC model_curve_edit.cpp)
else()
  set(CURVE_EDIT_SRC model_curve_simple_edit.cpp)
endif()

if(CURVES)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/model_curves.cpp
    ${CURVE_EDIT_SRC}
    )
endif()

if(SDCARD)
  set(GUI_SRC
    ${GUI_SRC}
    ../common/stdlcd/radio_sdmanager.cpp
    )
endif()
