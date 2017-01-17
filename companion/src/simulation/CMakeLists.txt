set(simulation_SRCS
  simulatordialog.cpp
  telemetrysimu.cpp
  trainersimu.cpp
  debugoutput.cpp
  simulatorinterface.cpp
  widgets/virtualjoystickwidget.cpp
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

  telemetrysimu.h
  trainersimu.h
  debugoutput.h
  widgets/buttonswidget.h
  #widgets/dialwidget.h
  #widgets/lcdwidget.h
  widgets/sliderwidget.h
  widgets/virtualjoystickwidget.h
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
  ${COMPANION_SRC_DIRECTORY}/simulation/widgets
  ${RADIO_SRC_DIRECTORY}/fonts/std
)

qt5_wrap_ui(simulation_SRCS ${simulation_UIS})
qt5_wrap_cpp(simulation_SRCS ${simulation_HDRS})

add_library(simulation ${simulation_SRCS} ${simulation_HDRS})
qt5_use_modules(simulation Widgets Xml)
