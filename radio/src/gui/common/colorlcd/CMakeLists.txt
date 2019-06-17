set(GUI_SRC
  ${GUI_SRC}
  ../common/colorlcd/widgets.cpp
  )

if(HELI)
  set(GUI_SRC ${GUI_SRC} ../common/colorlcd/model_heli.cpp)
endif()

