foreach(FILE ${SRC})
  set(SIMU_SRC ${SIMU_SRC} ../../${FILE})
endforeach()

set(SIMU_SRC ${SIMU_SRC} simpgmspace.cpp)

add_definitions(-DSIMU -g)
remove_definitions(-DCLI)

string(TOLOWER ${TRANSLATIONS} TRANSLATION_LOWERCASE)
set(SIMULATOR_FLAVOUR opentx-${FLAVOUR}-${TRANSLATION_LOWERCASE})
set(SIMULATOR_TARGET ${SIMULATOR_FLAVOUR}-simulator)
add_definitions(-DSIMULATOR_FLAVOUR="${SIMULATOR_FLAVOUR}")
include_directories(${COMPANION_SRC_DIRECTORY} ${COMPANION_SRC_DIRECTORY}/simulation)
add_library(${SIMULATOR_TARGET} SHARED ${SIMU_SRC} opentxsimulator.cpp)
add_dependencies(${SIMULATOR_TARGET} ${FIRMWARE_DEPENDENCIES})
qt5_use_modules(${SIMULATOR_TARGET} Core)

add_custom_target(opentx-simulator DEPENDS ${SIMULATOR_TARGET})

if(NOT WIN32)
  add_executable(simu WIN32 ${SIMU_SRC} ../../simu.cpp)
  add_dependencies(simu ${FIRMWARE_DEPENDENCIES})
  target_include_directories(simu PUBLIC /usr/local/include/fox-1.6 PUBLIC /usr/include/fox-1.6 /opt/local/include/fox-1.6)
  target_link_libraries(simu FOX-1.6 pthread)
endif()

if(APPLE)
  # OS X compiler no longer automatically includes /Library/Frameworks in search path
  set(CMAKE_SHARED_LINKER_FLAGS -F/Library/Frameworks)

  set(SIMULATOR_BUNDLES)
  foreach(library ${OPENTX_LIBRARIES})
    set(SIMULATOR_BUNDLE "${library}-bundle")
    add_custom_target(${SIMULATOR_BUNDLE}
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore lib${library}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork lib${library}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml lib${library}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui lib${library}.dylib
      COMMAND install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/../Frameworks/SDL.framework/Versions/A/SDL lib${library}.dylib
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMAND pwd
      COMMAND cp lib${library}.dylib companion.app/Contents/Resources/
      DEPENDS ${library}
      )
    list(APPEND SIMULATOR_BUNDLES ${SIMULATOR_BUNDLE})
  endforeach()
  add_custom_target(opentx-simulators-bundle DEPENDS ${SIMULATOR_BUNDLES})
endif(APPLE)

