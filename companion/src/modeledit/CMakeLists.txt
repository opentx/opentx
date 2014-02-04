include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(modeledit_NAMES
  modeledit
  setup
  heli
  curves
  telemetry
  expodialog
  mixerdialog
)

set(modeledit_SRCS
  modelpanel.cpp
  flightmodes.cpp
  inputs.cpp
  mixes.cpp
  channels.cpp
  customswitches.cpp
  customfunctions.cpp
  # templates.cpp
  mixerslist.cpp
  node.cpp
  edge.cpp
)

set(modeledit_HDRS
  modelpanel.h
  flightmodes.h
  inputs.h
  mixes.h
  channels.h
  customswitches.h
  customfunctions.h
  # templates.h
  mixerslist.h
  node.h
)

set(modeledit_UIS
  setup_timer.ui
  setup_module.ui
  flightmode.ui
  telemetry_analog.ui
  telemetry_customscreen.ui
)

foreach(name ${modeledit_NAMES})
  set(modeledit_SRCS ${modeledit_SRCS} ${name}.cpp)
  set(modeledit_HDRS ${modeledit_HDRS} ${name}.h)
  set(modeledit_UIS  ${modeledit_UIS}  ${name}.ui)
endforeach()
 
qt4_wrap_ui(modeledit_SRCS ${modeledit_UIS})
qt4_wrap_cpp(modeledit_SRCS ${modeledit_HDRS})

add_library(modeledit ${modeledit_SRCS})