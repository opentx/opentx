if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(C9X_NAME_SUFFIX ${VERSION_MAJOR}${VERSION_MINOR})
  set(COMPANION_NAME "companion${C9X_NAME_SUFFIX}")
  set(SIMULATOR_NAME "simulator${C9X_NAME_SUFFIX}")
  set(SIMULATOR_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib/companion${C9X_NAME_SUFFIX})
else(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(COMPANION_NAME "companion")
  set(SIMULATOR_NAME "simulator")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# This the name that the user will see in the generated DMG and what the application
# will be called under /Applications. We include the version string to make installing
# different versions for different eeproms easier, i.e. without manually renaming
set(COMPANION_OSX_APP_BUNDLE_NAME "OpenTX Companion ${VERSION_MAJOR}.${VERSION_MINOR}")

# On OS X we copy dfu-util to the application bundle. This the path from which we copy
# the binary (default homebrew path)
set(DFU_UTIL_PATH "/usr/local/bin/dfu-util")
set(AVRDUDE_PATH "/usr/local/bin/avrdude")

option(ALLOW_NIGHTLY_BUILDS "Allow nightly builds download / update") # Disabled by default
if(ALLOW_NIGHTLY_BUILDS)
  add_definitions(-DALLOW_NIGHTLY_BUILDS)
endif(ALLOW_NIGHTLY_BUILDS)

option(DEBUG_STORAGE_IMPORT "Turn on debug output for storage import") # Disabled by default
if(DEBUG_STORAGE_IMPORT)
  add_definitions(-DDEBUG_STORAGE_IMPORT)
  message(STATUS "Storage import debugging enabled")
endif()
if(DEBUG)
  add_definitions(-DDEBUG)
endif()

option(TABLE_LAYOUT "Use QTableWidget for grid layouts") # Disabled by default
if(TABLE_LAYOUT)
  add_definitions(-DTABLE_LAYOUT)
  message(STATUS "Using QTableWidget")
endif()

if(WIN32)
  option(WIN_DO_FULL_INSTALL "'install' target will perform a full installation into CMAKE_INSTALL_PREFIX path, otherwise only supporting DLLs are copied to binaries folder (default)" OFF)
  option(WIN_USE_CONSOLE "Open a Windows command console for Simulator debug output." OFF)
  if(WIN_USE_CONSOLE)
    add_definitions(-DWIN_USE_CONSOLE_STDIO)
  endif()
endif()

if(SDL_FOUND)
  include_directories(${SDL_INCLUDE_DIR})
  add_definitions(-DJOYSTICKS)
else()
  remove_definitions(-DSIMU_AUDIO)
endif()

find_package(Lupdate)
if(LUPDATE_FOUND)
  message("Qt lupdate: " ${LUPDATE_EXECUTABLE})
else()
  message("Qt lupdate not found, 'translations' target will not be availabe.")
endif()

add_definitions(-DSIMU)
add_definitions(-DQXT_STATIC)

if (NOT MSVC AND ${CMAKE_VERSION} VERSION_LESS 3.1.0})
  message("companion: adding -std=gnu++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

if(WIN32)
  include_directories(SYSTEM ${WIN_INCLUDE_DIRS})
  if(MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc /LD /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,5.01")
    message("companion: using MSVC with ${CMAKE_CXX_FLAGS}")
  elseif(MINGW)
    # struct packing breaks on MinGW w/out -mno-ms-bitfields: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991 & http://stackoverflow.com/questions/24015852/struct-packing-and-alignment-with-mingw
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-ms-bitfields -mwindows")
    message("companion: using MinGW with ${CMAKE_CXX_FLAGS}")
  endif()
else()
  set(PTHREAD_LIBRARY pthread)
  add_definitions(-Wall)
  link_directories(/usr/local/lib)
endif()

set(RADIO_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/radio/src)
set(SIMU_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/companion/src/simulation)
set(COMPANION_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/companion/src)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${COMPANION_SRC_DIRECTORY}
  ${COMPANION_SRC_DIRECTORY}/shared
  ${SIMU_SRC_DIRECTORY}
  ${COMPANION_SRC_DIRECTORY}/thirdparty/qxtcommandoptions
  ${COMPANION_SRC_DIRECTORY}/thirdparty/qcustomplot
  ${COMPANION_SRC_DIRECTORY}/thirdparty/miniz
  ${COMPANION_SRC_DIRECTORY}/storage
)

add_subdirectory(shared)
add_subdirectory(modeledit)
add_subdirectory(generaledit)
add_subdirectory(simulation)
add_subdirectory(storage)
add_subdirectory(thirdparty/qcustomplot)
add_subdirectory(thirdparty/qxtcommandoptions)

set(common_SRCS
  eeprominterface.cpp
  firmwares/er9x/er9xeeprom.cpp
  firmwares/er9x/er9xinterface.cpp
  firmwares/ersky9x/ersky9xeeprom.cpp
  firmwares/ersky9x/ersky9xinterface.cpp
  firmwares/opentx/opentxeeprom.cpp
  firmwares/opentx/opentxinterface.cpp
  )

qt5_wrap_cpp(common_SRCS)

add_library(common ${common_SRCS})
qt5_use_modules(common Core Xml Widgets)

set(companion_SRCS
  helpers.cpp
  helpers_html.cpp
  mdichild.cpp
  modelslist.cpp
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

set(companion_RESOURCES
  companion.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
  )

configure_file(${COMPANION_SRC_DIRECTORY}/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)
configure_file(${COMPANION_SRC_DIRECTORY}/translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc @ONLY)
configure_file(${COMPANION_SRC_DIRECTORY}/companion.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/companion.desktop @ONLY)
configure_file(${COMPANION_SRC_DIRECTORY}/simulator.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/simulator.desktop @ONLY)

if(WIN32)
  set(companion_SRCS ${companion_SRCS} icon.rc)
  if(NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS -mwindows)
  endif()
endif()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

set(LANGUAGES he pl pt ru de fr es it sv cs fi nl)
foreach(language ${LANGUAGES})
  set(companion_TS ${companion_TS} translations/companion_${language}.ts)
endforeach(language)

qt5_wrap_ui(companion_SRCS ${companion_UIS})
qt5_wrap_cpp(companion_SRCS ${companion_MOC_HDRS})
qt5_add_translation(companion_QM ${companion_TS})
qt5_add_resources(companion_RCC ${companion_RESOURCES})
add_custom_target(gen_qrc DEPENDS ${companion_RCC})

add_definitions(-DQT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}")

add_executable(${COMPANION_NAME} MACOSX_BUNDLE WIN32 ${companion_SRCS} ${companion_RCC} ${companion_QM})

add_dependencies(${COMPANION_NAME} gen_qrc)
qt5_use_modules(${COMPANION_NAME} Core Widgets Network)

target_link_libraries(${COMPANION_NAME} PRIVATE generaledit modeledit simulation common qcustomplot shared storage ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${WIN_LINK_LIBRARIES})

############# Standalone simu ###############

set(simu_SRCS
  modeledit/node.cpp
  modeledit/edge.cpp # TODO not needed
  simulator.cpp
  )

set(simu_MOC_HDRS
  modeledit/node.h
  )

qt5_wrap_cpp(simu_SRCS ${simu_MOC_HDRS} )

if(WIN32)
  set(simu_SRCS ${simu_SRCS} icon.rc)
endif()

add_executable(${SIMULATOR_NAME} MACOSX_BUNDLE WIN32 ${simu_SRCS} ${companion_RCC})

add_dependencies(${SIMULATOR_NAME} gen_qrc)

# TODO not the same link command than companion?
target_link_libraries(${SIMULATOR_NAME} PRIVATE simulation common storage qxtcommandoptions ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${OPENTX_SIMULATOR_LIBS} ${WIN_LINK_LIBRARIES})

############# Install ####################

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  message(STATUS "install " ${CMAKE_BINARY_DIR} " to " ${CMAKE_INSTALL_PREFIX}/bin)
  install(TARGETS ${COMPANION_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  install(TARGETS ${SIMULATOR_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  file(GLOB simulator_plugins "${CMAKE_BINARY_DIR}/libopentx-*${CMAKE_SHARED_LIBRARY_SUFFIX}")
  install(FILES ${simulator_plugins} DESTINATION "${SIMULATOR_LIB_PATH}")
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/companion.desktop DESTINATION share/applications RENAME companion${C9X_NAME_SUFFIX}.desktop)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/simulator.desktop DESTINATION share/applications RENAME simulator${C9X_NAME_SUFFIX}.desktop)
  if(${CMAKE_INSTALL_PREFIX} MATCHES "/usr/local")
    set(INSTALL_TEMP_SHR_PFX "/usr/")
    set(INSTALL_TEMP_LIB_PFX "/")
  endif()
  install(FILES images/linuxicons/16x16/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/16x16/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/22x22/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/22x22/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/24x24/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/24x24/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/32x32/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/32x32/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/48x48/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/48x48/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/128x128/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/128x128/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/256x256/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/256x256/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/512x512/companion.png DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/512x512/apps RENAME companion${C9X_NAME_SUFFIX}.png)
  install(FILES images/linuxicons/scalable/companion.svg DESTINATION ${INSTALL_TEMP_SHR_PFX}share/icons/hicolor/scalable/apps RENAME companion${C9X_NAME_SUFFIX}.svg)
  install(FILES ../targets/linux/45-companion-taranis.rules DESTINATION ${INSTALL_TEMP_LIB_PFX}lib/udev/rules.d RENAME 45-companion${C9X_NAME_SUFFIX}-taranis.rules)
  # Linux specific code
  set(OperatingSystem "Linux")

elseif(WIN32)

  get_target_property(QtCore_LOCATION Qt5::Core LOCATION)
  get_filename_component(QT_DLL_DIR ${QtCore_LOCATION} PATH)
  set(INSTALL_TEMP_QTDLL_FILES Qt5Core Qt5Gui Qt5Widgets Qt5Xml Qt5Network Qt5PrintSupport Qt5Multimedia)
  set(INSTALL_TEMP_ICUDLL_FILES icudt54.dll icuin54.dll icuuc54.dll)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(INSTALL_TEMP_QTDLL_SFX "d")
  endif()

  if(NOT WIN_DO_FULL_INSTALL)
    # Just copy supporting DLLs to build folder
    set(INSTALL_DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message(STATUS "Partial install to " ${INSTALL_DESTINATION})
  else()
    # Full Windows installation with all supporting files
    set(INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX})
    message(STATUS "Full install to " ${INSTALL_DESTINATION})
    # companion & simulator binaries
    install(TARGETS ${COMPANION_NAME} DESTINATION ${INSTALL_DESTINATION})
    install(TARGETS ${SIMULATOR_NAME} DESTINATION ${INSTALL_DESTINATION})
    file(GLOB simulator_plugins "${CMAKE_BINARY_DIR}/*opentx-*-simulator${CMAKE_SHARED_LIBRARY_SUFFIX}")
    install(FILES ${simulator_plugins} DESTINATION "${INSTALL_DESTINATION}")
    # language files
    file(GLOB language_files "${CMAKE_BINARY_DIR}/companion/src/companion_*.qm")
    install(FILES ${language_files} DESTINATION "${INSTALL_DESTINATION}/lang")
    # supporting utilities
    set(INSTALL_TEMP_FILES avrdude.exe avrdude.conf dfu-util.exe libusb0.dll libusb-1.0.dll)
    foreach(tmpfile ${INSTALL_TEMP_FILES})
      install(FILES "${COMPANION_SRC_DIRECTORY}/../targets/windows/${tmpfile}" DESTINATION ${INSTALL_DESTINATION})
    endforeach()
  endif()  # WIN_DO_FULL_INSTALL

  # Qt dlls
  foreach(tmpfile ${INSTALL_TEMP_QTDLL_FILES})
    install(FILES ${QT_DLL_DIR}/${tmpfile}${INSTALL_TEMP_QTDLL_SFX}.dll DESTINATION ${INSTALL_DESTINATION})
  endforeach()
  install(FILES ${QT_DLL_DIR}/../plugins/platforms/qwindows${INSTALL_TEMP_QTDLL_SFX}.dll DESTINATION ${INSTALL_DESTINATION}/platforms)
  # ICU dlls
  foreach(tmpfile ${INSTALL_TEMP_ICUDLL_FILES})
    # Qt5.7+ doesn't use icu dlls anymore (at least with MinGW), hence optional
    install(FILES ${QT_DLL_DIR}/${tmpfile}.dll DESTINATION ${INSTALL_DESTINATION} OPTIONAL)
  endforeach()
  # SDL dll
  if(SDL_FOUND AND SDL_LIBRARY_PATH AND EXISTS "${SDL_LIBRARY_PATH}/SDL.dll")
    set(SDL_DIR ${SDL_LIBRARY_PATH})  # this is also used by NSIS installer script
    install(FILES ${SDL_DIR}/SDL.dll DESTINATION ${INSTALL_DESTINATION})
  else()
    message(WARNING "Installer: SDL.dll not found!")
  endif()
  # C++/system dlls, depends on compiler
  if(MSVC)
    set(WIN_SYSDIR "$ENV{windir}/SysWOW64")  # This would be "system32" for a 64-bit build or on x86... but how to tell?
    if(IS_DIRECTORY ${WIN_SYSDIR})
      install(FILES ${WIN_SYSDIR}/msvcp140.dll ${WIN_SYSDIR}/vcruntime140.dll DESTINATION ${INSTALL_DESTINATION} OPTIONAL)
    endif()
    install(FILES "${WIN_PTHREAD_BASE_PATH}/lib/pthreadVC2.dll" DESTINATION ${INSTALL_DESTINATION})
  elseif(MINGW)
    get_filename_component(MINGW_DIR ${CMAKE_CXX_COMPILER} PATH)
    if(IS_DIRECTORY ${MINGW_DIR})
      install(FILES ${MINGW_DIR}/libgcc_s_dw2-1.dll ${MINGW_DIR}/libstdc++-6.dll ${MINGW_DIR}/libwinpthread-1.dll DESTINATION ${INSTALL_DESTINATION})
      set(MINGW_DIR "${MINGW_DIR}/../opt/bin")
      # SSL support
      install(FILES ${MINGW_DIR}/libeay32.dll ${MINGW_DIR}/ssleay32.dll DESTINATION ${INSTALL_DESTINATION} OPTIONAL)
    endif()
  endif()

endif() # WIN32 install

############# Packaging ####################

# Create Windows installer with NSIS (only works for MSVC for now)
if(WIN32)
  if(MSVC)
    set(COMPANION_NSI_IN_FILE companion-vs.nsi.in)
    set(SIMULATOR_NSI_IN_FILE simulator-vs.nsi.in)
  else()
    set(COMPANION_NSI_IN_FILE companion-msys.nsi.in)
    set(SIMULATOR_NSI_IN_FILE simulator-msys.nsi.in)
  endif()

  set(SYSDIR "$ENV{windir}/system32")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../targets/windows/${COMPANION_NSI_IN_FILE} ${PROJECT_BINARY_DIR}/companion/companion.nsi @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../targets/windows/${SIMULATOR_NSI_IN_FILE} ${PROJECT_BINARY_DIR}/companion/simulator.nsi @ONLY)

  find_program(NSIS_EXE makensis.exe PATHS
    "C:/Program Files/NSIS"
    "C:/Program Files (x86)/NSIS"
    "C:/Programs/NSIS"
    )

  add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/companion/companion-windows.exe
    COMMAND "${NSIS_EXE}" ARGS ${PROJECT_BINARY_DIR}/companion/companion.nsi
    DEPENDS companion simulator opentx-simulators ${PROJECT_BINARY_DIR}/companion/companion.nsi
    COMMENT "Companion Windows NSIS Installer")

  add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/companion/simulator-install.exe
    COMMAND "${NSIS_EXE}" ARGS ${PROJECT_BINARY_DIR}/companion/simulator.nsi
    DEPENDS simulator ${PROJECT_BINARY_DIR}/companion/simulator.nsi
    COMMENT "Simulator Windows NSIS Installer")

  add_custom_target(installer
    DEPENDS ${PROJECT_BINARY_DIR}/companion/companion-windows.exe
    DEPENDS ${PROJECT_BINARY_DIR}/companion/simulator-windows.exe
    SOURCES ${PROJECT_BINARY_DIR}/companion/companion.nsi
    SOURCES ${PROJECT_BINARY_DIR}/companion/simulator.nsi
    )
endif()

if(LUPDATE_FOUND)
  add_custom_target(translations
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${LUPDATE_EXECUTABLE} ${CMAKE_SOURCE_DIR} -no-obsolete -ts ${companion_TS}
    )
else()
  add_custom_target(translations
    COMMAND echo "Sorry, QT lupdate was not found."
    )
endif()

set(CPACK_PACKAGE_NAME "companion${C9X_NAME_SUFFIX}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Models and settings editor for the OpenTX open source firmware")
string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)

# The file stripping is deliberately disabled, with the stripped file we get
# very poor trace-backs from the users when they report Companion crash
set(CPACK_STRIP_FILES FALSE)

# Qt5 + Cmake + Mac is poorly documented. A lot of this is guesswork
# and trial and error. Do not hesitate to fix it for the better
IF(APPLE)
  set(plugin_dest_dir ${COMPANION_NAME}.app/Contents/Plugins)
  set(qtconf_dest_dir ${COMPANION_NAME}.app/Contents/Resources)
  set(translations_dest_dir ${COMPANION_NAME}.app/Contents/translations)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${COMPANION_NAME}.app")
  set_target_properties(${COMPANION_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Companion ${VERSION_MAJOR}.${VERSION_MINOR}")
  set_target_properties(${SIMULATOR_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Simulator ${VERSION_MAJOR}.${VERSION_MINOR}")

  # Use a non standard Info.plist that adds Retina support flags
  set_target_properties(${COMPANION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/companion/targets/mac/MacOSXBundleInfo.plist.in)
  set_target_properties(${SIMULATOR_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/companion/targets/mac/MacOSXBundleInfo.plist.in)

  INSTALL(TARGETS ${COMPANION_NAME}
      BUNDLE DESTINATION . COMPONENT Runtime
      RUNTIME DESTINATION bin COMPONENT Runtime
      )

  file(GLOB simulator_plugins "${CMAKE_BINARY_DIR}/libopentx-*${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set(companion_res_dir ${COMPANION_NAME}.app/Contents/Resources)
  install(FILES ${simulator_plugins} DESTINATION "${companion_res_dir}" COMPONENT Runtime)

  # Write qt.conf to tell qt where to find it plugins
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
    "[Paths]\nPlugins = Plugins\n")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
    DESTINATION "${companion_res_dir}"
    COMPONENT Runtime)

  # menu file
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/companion/targets/mac/qt_menu.nib DESTINATION "${companion_res_dir}" COMPONENT Runtime)

  # translations file
  install(FILES ${companion_QM} DESTINATION "${translations_dest_dir}" COMPONENT Runtime)

  # manually add the cocoa plugin
  get_target_property(QPA_PLUGIN Qt5::QCocoaIntegrationPlugin LOCATION)
  install(FILES ${QPA_PLUGIN} DESTINATION "${plugin_dest_dir}/platforms/" COMPONENT Runtime)
  set(bundle_qt_libs "\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/platforms/libqcocoa.dylib")

  # Add icon
  set(MACOSX_BUNDLE_ICON_FILE iconmac.icns)
  set(MAC_ICON_FILE ${COMPANION_SRC_DIRECTORY}/images/${MACOSX_BUNDLE_ICON_FILE})
  SET_SOURCE_FILES_PROPERTIES(${MAC_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  set(${COMPANION_SOURCES} ${COMPANION_SOURCES} ${PROJECT_SOURCE_DIR}/images/${MACOSX_BUNDLE_ICON_FILE})
  install(FILES ${MAC_ICON_FILE} DESTINATION ${companion_res_dir} COMPONENT Runtime)

  # Copy dfu-util and avrdude, resolve symlink first
  get_filename_component(DFU_UTIL_ABSOLUTE_PATH ${DFU_UTIL_PATH} REALPATH)
  get_filename_component(AVRDUDE_ABSOLUTE_PATH ${AVRDUDE_PATH} REALPATH)
  install(PROGRAMS ${DFU_UTIL_ABSOLUTE_PATH} ${AVRDUDE_ABSOLUTE_PATH} DESTINATION ${companion_res_dir} COMPONENT Runtime)
  set(bundle_tools_path "\${CMAKE_INSTALL_PREFIX}/${companion_res_dir}/dfu-util;\${CMAKE_INSTALL_PREFIX}/${companion_res_dir}/avrdude")

  # These needs to be relative to CMAKE_INSTALL_PREFIX
  get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
  get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
  get_filename_component(QT_LIBRARY_DIR "${QT_LIBRARY_DIR}/.." ABSOLUTE)

  # Include depencies (adding frameworks, fixing the embbeded libraries)
  # I get write errors without setting BU_CHMOD_BUNDLE_ITEMS even though it is
  # technically a hack (that is already is in the Bundle library ....)
  install(CODE "
   include(BundleUtilities)
   file(GLOB bundle_simulator_libs \"\${CMAKE_INSTALL_PREFIX}/${companion_res_dir}/libopentx-*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
   set(BU_CHMOD_BUNDLE_ITEMS on)
   fixup_bundle(\"${APPS}\"   \"\${bundle_simulator_libs};${bundle_qt_libs};${bundle_tools_path}\"   \"${QT_LIBRARY_DIR}\")
   file(RENAME \"\${CMAKE_INSTALL_PREFIX}/${COMPANION_NAME}.app\" \"\${CMAKE_INSTALL_PREFIX}/${COMPANION_OSX_APP_BUNDLE_NAME}.app\")
   " COMPONENT Runtime)
endif()

find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")

if(DPKG_PROGRAM)
  set(CPACK_GENERATOR "DEB")
  execute_process(
    COMMAND ${DPKG_PROGRAM} --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Bertrand Songis <bsongis-nosp@m-gmail.com>")
  set(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
  set(CPACK_DEBIAN_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
  set(CPACK_DEBIAN_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
  set(CPACK_DEBIAN_ARCHITECTURE $(CMAKE_SYSTEM_PROCESSOR))
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
# set(CPACK_GENERATOR "TGZ")  # for quick testing
  set(CPACK_BINARY_DRAGNDROP ON)
  set(CPACK_DMG_BACKGROUND_IMAGE ${COMPANION_SRC_DIRECTORY}/images/splash22_3_dmg.png)
  set(CPACK_DMG_VOLUME_NAME "OpenTX Companion")
  set(CPACK_DMG_DS_STORE ${PROJECT_SOURCE_DIR}/companion/targets/mac/DS_Store)
  set(CPACK_PACKAGE_FILE_NAME "opentx-${CPACK_PACKAGE_NAME_LOWERCASE}-${VERSION}")
else(DPKG_PROGRAM)
  set(CPACK_GENERATOR "RPM")
  set(CPACK_RPM_PACKAGE_VERSION ${VERSION})
  set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}-${VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/lib /lib/udev /lib/udev/rules.d")
endif(DPKG_PROGRAM)

include(CPack)
