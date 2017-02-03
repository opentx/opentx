set(simulation_SRCS
  debugoutput.cpp
  simulatordialog.cpp
  simulatorinterface.cpp
  simulateduiwidget.cpp
  simulateduiwidget9X.cpp
  simulateduiwidgetX7.cpp
  simulateduiwidgetX9.cpp
  simulateduiwidgetX12.cpp
  simulatorstartupdialog.cpp
  telemetrysimu.cpp
  trainersimu.cpp
  widgets/radiowidget.cpp
  widgets/virtualjoystickwidget.cpp
)

set(simulation_UIS
  debugoutput.ui
  simulatordialog.ui
  simulateduiwidget9X.ui
  simulateduiwidgetX7.ui
  simulateduiwidgetX9.ui
  simulateduiwidgetX12.ui
  simulatorstartupdialog.ui
  telemetrysimu.ui
  trainersimu.ui
)

set(simulation_HDRS
  debugoutput.h
  radiouiaction.h
  simulateduiwidget.h
  # simulator.h
  simulatordialog.h
  simulatorstartupdialog.h
  telemetrysimu.h
  trainersimu.h
  widgets/buttonswidget.h
  widgets/lcdwidget.h
  widgets/radiowidget.h
  widgets/radiofaderwidget.h
  widgets/radioknobwidget.h
  widgets/radioswitchwidget.h
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
