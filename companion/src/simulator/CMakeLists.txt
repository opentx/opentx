set(TH9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/th9x)
add_custom_command(
  OUTPUT ${TH9X_CHECKOUT_DIRECTORY}/th9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${TH9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${TH9X_CHECKOUT_DIRECTORY} && svn checkout http://th9x.googlecode.com/svn/trunk/src@285 .
  COMMAND ${CMAKE_COMMAND} -E copy ${COMPANION_SRC_DIRECTORY}/firmwares/th9x/th9xsimulator.cpp ${TH9X_CHECKOUT_DIRECTORY}
  DEPENDS ${COMPANION_SRC_DIRECTORY}/firmwares/th9x/th9xsimulator.cpp
)

set(ER9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/er9x)
add_custom_command(
  OUTPUT ${ER9X_CHECKOUT_DIRECTORY}/er9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ER9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${ER9X_CHECKOUT_DIRECTORY} && svn checkout http://er9x.googlecode.com/svn/trunk/src@790 .
  COMMAND ${CMAKE_COMMAND} -E copy ${COMPANION_SRC_DIRECTORY}/firmwares/er9x/er9xsimulator.cpp ${ER9X_CHECKOUT_DIRECTORY}
  DEPENDS ${COMPANION_SRC_DIRECTORY}/firmwares/er9x/er9xsimulator.cpp
)

set(ERSKY9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/ersky9x)
add_custom_command(
  OUTPUT ${ERSKY9X_CHECKOUT_DIRECTORY}/ersky9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ERSKY9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${ERSKY9X_CHECKOUT_DIRECTORY} && svn checkout http://ersky9x.googlecode.com/svn/trunk/src@170 .
  COMMAND ${CMAKE_COMMAND} -E copy ${COMPANION_SRC_DIRECTORY}/firmwares/ersky9x/ersky9xsimulator.cpp ${ERSKY9X_CHECKOUT_DIRECTORY}
  DEPENDS ${COMPANION_SRC_DIRECTORY}/firmwares/ersky9x/ersky9xsimulator.cpp
)

set(simulation_SRCS
  simulatordialog.cpp
)

set(simulation_UIS
  simulatordialog-9x.ui
  simulatordialog-taranis.ui
)

set(simulation_HDRS
  simulatordialog.h
  cursorwidget.h
  menuwidget.h
  xcursorwidget.h
  xmenuwidget.h
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
  # ${CMAKE_BINARY_DIR} 
  # ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/../..
  ${COMPANION_SRC_DIRECTORY}
  ${RADIO_SRC_DIRECTORY}/fonts/std
)

qt4_wrap_ui(simulation_SRCS ${simulation_UIS})
qt4_wrap_cpp(simulation_SRCS ${simulation_HDRS})

add_library(simulation ${simulation_SRCS})

set(simu_SRCS
  ${COMPANION_SRC_DIRECTORY}/eeprominterface.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/th9x/th9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/th9x/th9xinterface.cpp
  ${TH9X_CHECKOUT_DIRECTORY}/th9xsimulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/er9x/er9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/er9x/er9xinterface.cpp
  ${ER9X_CHECKOUT_DIRECTORY}/er9xsimulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/gruvin9x/gruvin9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/gruvin9x/gruvin9xinterface.cpp  
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/open9xStockeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/open9xGruvin9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/open9xSky9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxinterface.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxTaranisSimulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxSky9xsimulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxGruvin9xsimulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxM128simulator.cpp
  ${COMPANION_SRC_DIRECTORY}/firmwares/opentx/opentxsimulator.cpp  
  ${COMPANION_SRC_DIRECTORY}/firmwares/ersky9x/ersky9xeeprom.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/firmwares/ersky9x/ersky9xinterface.cpp
  ${ERSKY9X_CHECKOUT_DIRECTORY}/ersky9xsimulator.cpp
  ${RADIO_SRC_DIRECTORY}/bitmaps/sticks.lbm
  ${COMPANION_SRC_DIRECTORY}/file.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/helpers.cpp # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/modeledit/node.cpp
  ${COMPANION_SRC_DIRECTORY}/modeledit/edge.cpp # TODO not needed
  main.cpp
)

set(simu_HDRS
  ${COMPANION_SRC_DIRECTORY}/modeledit/node.h  
  ${COMPANION_SRC_DIRECTORY}/helpers.h # TODO not needed
  ${COMPANION_SRC_DIRECTORY}/myslider.h # TODO not needed
)

qt4_wrap_cpp(simu_SRCS ${simu_HDRS} )
qt4_add_resources(simu_SRCS ${COMPANION_SRC_DIRECTORY}/companion.qrc) # TODO not needed

add_executable(simu WIN32 ${simu_SRCS})

target_link_libraries(simu simulation ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${XERCESC_LIBRARY} ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${PHONON_LIBS})
