set(simulation_SRCS
  simulatordialog.cpp
  telemetrysimu.cpp
  trainersimu.cpp
  debugoutput.cpp
  simulatorinterface.cpp
  virtualjoystickwidget.cpp
)

set(simulation_UIS
  simulatordialog-9x.ui
  simulatordialog-taranis.ui
  simulatordialog-horus.ui
  simulatordialog-flamenco.ui
  telemetrysimu.ui
  trainersimu.ui
  debugoutput.ui
)

set(simulation_HDRS
  simulatordialog.h
  # lcdwidget.h
  buttonswidget.h
  sliderwidget.h
  telemetrysimu.h
  trainersimu.h
  debugoutput.h
  virtualjoystickwidget.h
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
  ${COMPANION_SRC_DIRECTORY}
  ${COMPANION_SRC_DIRECTORY}/simulation
  ${RADIO_SRC_DIRECTORY}/fonts/std
)

qt5_wrap_ui(simulation_SRCS ${simulation_UIS})
qt5_wrap_cpp(simulation_SRCS ${simulation_HDRS})

add_library(simulation ${simulation_SRCS})
qt5_use_modules(simulation Widgets Xml)
