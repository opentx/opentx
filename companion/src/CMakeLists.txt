PROJECT( companion )

SET(C9X_VERSION_MAJOR "2")
SET(C9X_VERSION_MINOR "2")
SET(C9X_VERSION_REVISION "0")
SET(C9X_VERSION_SUFFIX $ENV{OPENTX_VERSION_SUFFIX})

SET(C9X_VERSION ${C9X_VERSION_MAJOR}.${C9X_VERSION_MINOR}.${C9X_VERSION_REVISION}${C9X_VERSION_SUFFIX})
MESSAGE(STATUS "OpenTX Companion ${C9X_VERSION}")

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(C9X_NAME_SUFFIX ${C9X_VERSION_MAJOR}${C9X_VERSION_MINOR})
  SET(COMPANION_NAME "companion${C9X_NAME_SUFFIX}")
  SET(SIMULATOR_NAME "simulator${C9X_NAME_SUFFIX}")
  SET( SIMULATOR_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/companion${C9X_NAME_SUFFIX} )
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(COMPANION_NAME "companion")
  SET(SIMULATOR_NAME "simulator")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

OPTION(ALLOW_NIGHTLY_BUILDS "Allow nightly builds download / update") # Disabled by default 
IF(ALLOW_NIGHTLY_BUILDS)
	ADD_DEFINITIONS(-DALLOW_NIGHTLY_BUILDS)
ENDIF(ALLOW_NIGHTLY_BUILDS)

OPTION(DEBUG_IMPORT "Turn on debug output for EEPROM import") # Disabled by default
IF(DEBUG_IMPORT)
    ADD_DEFINITIONS( -DDEBUG_MODEL_IMPORT)
    MESSAGE( STATUS "EEPROM import debugging enabled" )
ENDIF(DEBUG_IMPORT)

MESSAGE( STATUS "Looking for XercesC " )
FIND_PACKAGE(XercesC)
IF ( XERCESC_FOUND )
    INCLUDE_DIRECTORIES( ${XERCESC_INCLUDE_DIR} )
    SET(LidarFormat_LIBRAIRIES ${LidarFormat_LIBRAIRIES} ${XERCESC_LIBRARIES})
ELSE()
    MESSAGE( FATAL_ERROR "XercesC not found! Please set XercesC path ..." )
ENDIF()

MESSAGE( STATUS "Looking for Xsd " )
FIND_PACKAGE(Xsd)
IF ( XSD_FOUND )
    INCLUDE_DIRECTORIES( ${XSD_INCLUDE_DIR} )
    MESSAGE("XSD Include Path: " ${XSD_INCLUDE_DIR} )
ELSE()
    MESSAGE( FATAL_ERROR "XSD not found! Please set XSD path ..." )
ENDIF()

MESSAGE( STATUS "Looking for SDL " )
FIND_PACKAGE(Sdl)
IF ( SDL_FOUND )
    INCLUDE_DIRECTORIES( ${SDL_INCLUDE_DIR} )
    MESSAGE("SDL Include Path: " ${SDL_INCLUDE_DIR} )
    ADD_DEFINITIONS( -DJOYSTICKS )
    ADD_DEFINITIONS( -DSIMU_AUDIO )
ELSE()
    MESSAGE( "SDL not found! Joysticks support will be disabled. Simulator audio will not work." )
ENDIF()

FIND_PACKAGE(Qt4 4.8.1 REQUIRED)
SET( QT_USE_QTNETWORK True )
SET( QT_USE_QTXML True )
INCLUDE( ${QT_USE_FILE} )

MESSAGE( STATUS "Looking for PHONON " )
FIND_PACKAGE(Phonon)
IF ( PHONON_FOUND )
    INCLUDE_DIRECTORIES( ${PHONO_INCLUDE_DIR} )
    MESSAGE("PHONON Include Path: " ${PHONON_INCLUDE_DIR} )
    ADD_DEFINITIONS( -DPHONON )
ELSE()
    MESSAGE( "PHONON not found!" )
ENDIF()

MESSAGE( STATUS "Looking for QT lupdate " )
FIND_PACKAGE(Lupdate)
if( LUPDATE_FOUND )
  MESSAGE( "QT lupdate: " ${LUPDATE_EXECUTABLE} )
ELSE()
  MESSAGE( "QT lupdate not found!" )
ENDIF()

ADD_CUSTOM_COMMAND(
  COMMAND ${XSDCXX_EXECUTABLE} cxx-tree --root-element-all --generate-serialization --output-dir ${PROJECT_BINARY_DIR}/ ${PROJECT_SOURCE_DIR}/radio.xsd
  OUTPUT ${PROJECT_BINARY_DIR}/radio.hxx ${PROJECT_BINARY_DIR}/radio.cxx
  DEPENDS ${PROJECT_SOURCE_DIR}/radio.xsd
)

IF( MSVC )
  SET(CMAKE_CXX_FLAGS "/EHsc /LD")
  INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/winbuild C:/Programs/pthreads/Pre-built.2/include C:/Programs/msinttypes)
  LINK_DIRECTORIES( C:/Programs/pthreads/Pre-built.2/lib/ )
  SET( PTHREAD_LIBRARY pthreadVC2.lib )
ELSE( )
  SET( PTHREAD_LIBRARY pthread )
  ADD_DEFINITIONS( -Wall )
  LINK_DIRECTORIES( /usr/local/lib )
ENDIF( )

set(RADIO_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/../../radio/src)
set(SIMU_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/simulation)
set(COMPANION_SRC_DIRECTORY ${PROJECT_SOURCE_DIR})

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/../..
  ${PROJECT_SOURCE_DIR}/shared
  ${SIMU_SRC_DIRECTORY}
  ${XSD_INCLUDE_DIR}
)

add_subdirectory(shared)
add_subdirectory(modeledit)
add_subdirectory(generaledit)
add_subdirectory(simulation)
add_subdirectory(firmwares/opentx/simulator)
add_subdirectory(qcustomplot)

set(common_SRCS
  eeprominterface.cpp
  firmwares/th9x/th9xeeprom.cpp # TODO not needed
  firmwares/th9x/th9xinterface.cpp
  firmwares/er9x/er9xeeprom.cpp
  firmwares/er9x/er9xinterface.cpp
  firmwares/gruvin9x/gruvin9xeeprom.cpp
  firmwares/gruvin9x/gruvin9xinterface.cpp
  firmwares/ersky9x/ersky9xeeprom.cpp
  firmwares/ersky9x/ersky9xinterface.cpp
  firmwares/opentx/opentxeeprom.cpp
  firmwares/opentx/open9xStockeeprom.cpp
  firmwares/opentx/open9xGruvin9xeeprom.cpp
  firmwares/opentx/open9xSky9xeeprom.cpp
  firmwares/opentx/opentxinterface.cpp
  file.cpp
  appdata.cpp
)

qt4_wrap_cpp(common_SRCS)

add_library(common ${common_SRCS})

set(companion_SRCS
  hexinterface.cpp
  firmwareinterface.cpp
  converteeprom.cpp
  # xmlinterface.cpp
  # ${PROJECT_BINARY_DIR}/radio.cxx
  helpers.cpp
  helpers_html.cpp
  mdichild.cpp
  modelslist.cpp
  mountlist.cpp
  apppreferencesdialog.cpp
  fwpreferencesdialog.cpp
  burnconfigdialog.cpp
  multimodelprinter.cpp
  comparedialog.cpp
  contributorsdialog.cpp
  releasenotesdialog.cpp
  releasenotesfirmwaredialog.cpp
  customizesplashdialog.cpp
  radiointerface.cpp
  progresswidget.cpp
  progressdialog.cpp
  process_copy.cpp
  process_flash.cpp
  process_sync.cpp
  flashfirmwaredialog.cpp
  flasheepromdialog.cpp
  printdialog.cpp
  modelprinter.cpp
  fusesdialog.cpp
  logsdialog.cpp
  downloaddialog.cpp
  splashlibrarydialog.cpp
  mainwindow.cpp
  companion.cpp
  radionotfound.cpp
  wizarddata.cpp
  wizarddialog.cpp
)

set(companion_MOC_HDRS
  apppreferencesdialog.h
  fwpreferencesdialog.h
  burnconfigdialog.h
  comparedialog.h
  printdialog.h
  fusesdialog.h
  logsdialog.h
  contributorsdialog.h
  releasenotesdialog.h
  releasenotesfirmwaredialog.h
  customizesplashdialog.h
  splashlibrarydialog.h
  splashlabel.h
  progresswidget.h
  progressdialog.h
  process_copy.h
  process_flash.h
  process_sync.h
  flashfirmwaredialog.h
  flasheepromdialog.h
  downloaddialog.h
  modelslist.h
  mdichild.h
  mainwindow.h
  radionotfound.h
  helpers.h
  wizarddialog.h
  modelprinter.h
  multimodelprinter.h
)

set(companion_UIS
  mdichild.ui
  comparedialog.ui
  fusesdialog.ui
  logsdialog.ui
  apppreferencesdialog.ui
  fwpreferencesdialog.ui
  burnconfigdialog.ui
  downloaddialog.ui
  printdialog.ui
  htmldialog.ui
  customizesplashdialog.ui
  splashlibrarydialog.ui
  progresswidget.ui
  progressdialog.ui
  flashfirmwaredialog.ui
  flasheepromdialog.ui
  radionotfound.ui
)

set(companion_RCS
  companion.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

MACRO (TODAY RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} ${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} ${${RESULT}})
    ELSE (WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${RESULT} 00.00.0000)
    ENDIF (WIN32)
ENDMACRO (TODAY)

MACRO (NOW RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C time /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..):(..).*" "\\1:\\2:00" ${RESULT} ${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..):(..):(..).*" "\\1:\\2:\\3" ${RESULT} ${${RESULT}})
    ELSE (WIN32)
        MESSAGE(SEND_ERROR "time not implemented")
        SET(${RESULT} 00:00:00)
    ENDIF (WIN32)
ENDMACRO (NOW)

TODAY(DATE)
NOW(TIME)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/firmwares/opentx/stamp-opentx.h.in ${CMAKE_CURRENT_BINARY_DIR}/stamp-opentx.h @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/companion.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/companion.desktop @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/simulator.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/simulator.desktop @ONLY)

IF( WIN32 )
  SET( companion_SRCS ${companion_SRCS} icon.rc )
  IF ( NOT MSVC )
    SET(CMAKE_EXE_LINKER_FLAGS -mwindows)
  ENDIF( )
ENDIF( )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )

SET( LANGUAGES he pl pt ru de fr es it sv cs fi nl)
FOREACH( language ${LANGUAGES} )
  SET( companion_TS ${companion_TS} translations/companion_${language}.ts )
ENDFOREACH( language )

set( companion_SRCS ${companion_SRCS}  )
QT4_WRAP_UI( companion_SRCS ${companion_UIS} )
QT4_WRAP_CPP( companion_SRCS ${companion_MOC_HDRS} )
QT4_ADD_TRANSLATION( companion_QM ${companion_TS}  )
QT4_ADD_RESOURCES( companion_SRCS ${companion_RCS} )

ADD_DEFINITIONS( -DQT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}" )

IF( APPLE )
    ADD_EXECUTABLE( ${COMPANION_NAME} MACOSX_BUNDLE ${companion_SRCS} ${companion_QM} )
    # OS X compiler no longer automatically includes /Library/Frameworks in search path
    SET( CMAKE_EXE_LINKER_FLAGS -F/Library/Frameworks )
ELSE( )
    ADD_EXECUTABLE( ${COMPANION_NAME} WIN32 ${companion_SRCS} ${companion_QM} )
ENDIF( )

target_link_libraries(${COMPANION_NAME} generaledit modeledit simulation common qcustomplot shared ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${XERCESC_LIBRARY} ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${PHONON_LIBS})

############# Standalone simu ###############

set(simu_SRCS
  modeledit/node.cpp
  modeledit/edge.cpp # TODO not needed
  simulator.cpp
)

set(simu_MOC_HDRS
  modeledit/node.h
)

set( simu_SRCS ${simu_SRCS} )
qt4_wrap_cpp(simu_SRCS ${simu_MOC_HDRS} )
qt4_add_resources(simu_SRCS companion.qrc) # TODO not needed

if(WIN32)
  set(simu_SRCS ${simu_SRCS} icon.rc)
endif()

if(APPLE)
  add_executable(${SIMULATOR_NAME} MACOSX_BUNDLE ${simu_SRCS})
else()
  add_executable(${SIMULATOR_NAME} WIN32 ${simu_SRCS})
endif()

target_link_libraries(${SIMULATOR_NAME} simulation common ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${PHONON_LIBS} ${OPENTX_SIMULATOR_LIBS})

############# Packaging ####################

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    INSTALL( TARGETS ${COMPANION_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    INSTALL( TARGETS ${SIMULATOR_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/companion.desktop DESTINATION share/applications RENAME companion${C9X_NAME_SUFFIX}.desktop)
    INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/simulator.desktop DESTINATION share/applications RENAME simulator${C9X_NAME_SUFFIX}.desktop)
    INSTALL( FILES images/linuxicons/16x16/companion.png    DESTINATION /usr/share/icons/hicolor/16x16/apps    RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/22x22/companion.png    DESTINATION /usr/share/icons/hicolor/22x22/apps    RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/24x24/companion.png    DESTINATION /usr/share/icons/hicolor/24x24/apps    RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/32x32/companion.png    DESTINATION /usr/share/icons/hicolor/32x32/apps    RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/48x48/companion.png    DESTINATION /usr/share/icons/hicolor/48x48/apps    RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/128x128/companion.png  DESTINATION /usr/share/icons/hicolor/128x128/apps  RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/256x256/companion.png  DESTINATION /usr/share/icons/hicolor/256x256/apps  RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/512x512/companion.png  DESTINATION /usr/share/icons/hicolor/512x512/apps  RENAME companion${C9X_NAME_SUFFIX}.png )
    INSTALL( FILES images/linuxicons/scalable/companion.svg DESTINATION /usr/share/icons/hicolor/scalable/apps RENAME companion${C9X_NAME_SUFFIX}.svg )
    INSTALL( FILES ../targets/linux/45-companion-taranis.rules DESTINATION /lib/udev/rules.d RENAME 45-companion${C9X_NAME_SUFFIX}-taranis.rules)
    # Linux specific code
    SET(OperatingSystem "Linux")
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    INSTALL( TARGETS ${COMPANION_NAME} DESTINATION ${QT_BINARY_DIR} )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF( WIN32 )
  IF( MSVC )
    SET(COMPANION_NSI_IN_FILE companion-vs.nsi.in)
    SET(SIMULATOR_NSI_IN_FILE simulator-vs.nsi.in)
  ELSE( )
    SET(COMPANION_NSI_IN_FILE companion-msys.nsi.in)
    SET(SIMULATOR_NSI_IN_FILE simulator-msys.nsi.in)
  ENDIF( )

  SET( SYSDIR "$ENV{windir}/system32" )
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../targets/windows/${COMPANION_NSI_IN_FILE} ${CMAKE_CURRENT_BINARY_DIR}/companion.nsi @ONLY)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../targets/windows/${SIMULATOR_NSI_IN_FILE} ${CMAKE_CURRENT_BINARY_DIR}/simulator.nsi @ONLY)

  FIND_PROGRAM(NSIS_EXE makensis.exe PATHS
    "C:/Program Files/NSIS"
    "C:/Program Files (x86)/NSIS"
    "C:/Programs/NSIS"
  )

  ADD_CUSTOM_COMMAND(OUTPUT ${PROJECT_BINARY_DIR}/companion-windows.exe
                     COMMAND "${NSIS_EXE}" ARGS companion.nsi
                     DEPENDS companion simulator opentx-simulators ${PROJECT_BINARY_DIR}/companion.nsi
                     COMMENT "Companion Windows NSIS Installer")

  ADD_CUSTOM_COMMAND(OUTPUT ${PROJECT_BINARY_DIR}/simulator-install.exe
                     COMMAND "${NSIS_EXE}" ARGS simulator.nsi
                     DEPENDS simulator opentx-simulators ${PROJECT_BINARY_DIR}/simulator.nsi
                     COMMENT "Simulator Windows NSIS Installer")

  ADD_CUSTOM_TARGET( installer
    DEPENDS ${PROJECT_BINARY_DIR}/companion-windows.exe
    DEPENDS ${PROJECT_BINARY_DIR}/simulator-windows.exe
    SOURCES ${PROJECT_BINARY_DIR}/companion.nsi
    SOURCES ${PROJECT_BINARY_DIR}/simulator.nsi
  )
ENDIF( )

IF( APPLE )
  SET(MACOSX_BUNDLE_ICON_FILE iconmac.icns)
  IF( CMAKE_GENERATOR STREQUAL Xcode )
    SET( WORK_DIR ${CMAKE_BINARY_DIR}/Debug )
  ELSE( )
    SET( WORK_DIR ${PROJECT_BINARY_DIR} )
  ENDIF()

  ADD_CUSTOM_TARGET( addframeworks
                     COMMAND mkdir -p companion.app/Contents/Frameworks
                     COMMAND mkdir -p companion.app/Contents/Resources
                     COMMAND mkdir -p companion.app/Contents/translations
                     COMMAND rsync ${PROJECT_SOURCE_DIR}/images/${MACOSX_BUNDLE_ICON_FILE} companion.app/Contents/Resources/${MACOSX_BUNDLE_ICON_FILE} # Copy the icon files
                     COMMAND rsync -r --exclude=.svn  ${PROJECT_SOURCE_DIR}/../targets/mac/qt_menu.nib companion.app/Contents/Resources/
                     COMMAND rsync ${PROJECT_BINARY_DIR}/*.qm companion.app/Contents/translations
                     COMMAND rsync ${PROJECT_BINARY_DIR}/*.qm companion.app/Contents/translations
                     COMMAND rsync -r ${XSD_INCLUDE_DIR}/.. companion.app/Contents/Frameworks/
                     COMMAND rsync -r /Library/Frameworks/SDL.framework companion.app/Contents/Frameworks/
                     COMMAND ${QT_BINARY_DIR}/macdeployqt companion.app
                     COMMAND install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/../Frameworks/SDL.framework/Versions/A/SDL companion.app/Contents/MacOS/companion
                     WORKING_DIRECTORY ${WORK_DIR}
                     DEPENDS ${COMPANION_NAME} )

  ADD_CUSTOM_TARGET( makedmg
                     COMMAND ${QT_BINARY_DIR}/macdeployqt companion.app -dmg -no-plugins
                     WORKING_DIRECTORY ${WORK_DIR}
                     DEPENDS addframeworks opentx-simulators-bundle)
ENDIF( )

ADD_CUSTOM_TARGET( stamp
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/version.h ${CMAKE_SOURCE_DIR}/../companion.stamp
                   DEPENDS ${CMAKE_BINARY_DIR}/version.h )

IF( LUPDATE_FOUND )
  ADD_CUSTOM_TARGET( translations
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${LUPDATE_EXECUTABLE} ${CMAKE_SOURCE_DIR} -no-obsolete -ts ${companion_TS}
  )
ELSE()
  ADD_CUSTOM_TARGET( translations
    COMMAND echo "Sorry, QT lupdate was not found."
  )
ENDIF()

SET(CPACK_PACKAGE_NAME "companion${C9X_NAME_SUFFIX}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Models and settings editor for the OpenTx open source firmware")
string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
SET( CPACK_STRIP_FILES TRUE )
find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
if(DPKG_PROGRAM)
  SET(CPACK_GENERATOR "DEB")
  execute_process(
    COMMAND ${DPKG_PROGRAM} --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${C9X_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Bertrand Songis <bsongis-nosp@m-gmail.com>")
  SET(CPACK_DEBIAN_PACKAGE_VERSION ${C9X_VERSION})
  SET(CPACK_DEBIAN_PACKAGE_VERSION_MAJOR ${C9X_VERSION_MAJOR})
  SET(CPACK_DEBIAN_PACKAGE_VERSION_MINOR ${C9X_VERSION_MINOR})
  SET(CPACK_DEBIAN_ARCHITECTURE $(CMAKE_SYSTEM_PROCESSOR))
  SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
else(DPKG_PROGRAM)
  SET(CPACK_GENERATOR "RPM")
  SET(CPACK_RPM_PACKAGE_VERSION ${C9X_VERSION})
  SET(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}-${C9X_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
endif(DPKG_PROGRAM)

INCLUDE(CPack)
