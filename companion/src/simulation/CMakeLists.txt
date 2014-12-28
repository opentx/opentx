set(simulation_SRCS
  simulatordialog.cpp
  telemetrysimu.cpp
)

set(simulation_UIS
  simulatordialog-9x.ui
  simulatordialog-taranis.ui
  telemetrysimu.ui
)

set(simulation_HDRS
  simulatordialog.h
  cursorwidget.h
  menuwidget.h
  xcursorwidget.h
  xmenuwidget.h
  myslider.h
  telemetrysimu.h
)

if(SDL_FOUND)
  set(simulation_SRCS
    ${simulation_SRCS}
    joystick.cpp
    joystickdialog.cpp
  )
  
  set(simulation_HDRS
    ${simulation_HDRS}
    joystick.h
    joystickdialog.h
  )
     
  set(simulation_UIS
    ${simulation_UIS}
    joystickdialog.ui
  ) 
endif() 

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/../..
  ${COMPANION_SRC_DIRECTORY}
  ${RADIO_SRC_DIRECTORY}/fonts/std
)

qt4_wrap_ui(simulation_SRCS ${simulation_UIS})
qt4_wrap_cpp(simulation_SRCS ${simulation_HDRS})

add_library(simulation ${simulation_SRCS})
