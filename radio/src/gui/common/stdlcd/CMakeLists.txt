set(GUI_SRC
  ${GUI_SRC}
  ../common/stdlcd/widgets.cpp
  )

if(HELI)
  set(GUI_SRC ${GUI_SRC} ../common/stdlcd/model_heli.cpp)
endif()

if(ARCH STREQUAL ARM)
  add_definitions(-DGRAPHICS)
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
