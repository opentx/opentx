foreach(FILE ${SRC})
  set(SIMU_SRC ${SIMU_SRC} ../../${FILE})
endforeach()

set(SIMU_SRC
  ${SIMU_SRC}
  simpgmspace.cpp
  simueeprom.cpp
  )

add_definitions(-DSIMU)
add_definitions(-DLUA_COMPILER)
remove_definitions(-DCLI)

if(SDL_FOUND)
  include_directories(${SDL_INCLUDE_DIR})
  add_definitions(-DSIMU_AUDIO)
  add_definitions(-DJOYSTICKS)
endif()

if(Qt5Widgets_FOUND)
  set(SIMULATOR_FLAVOUR opentx-${FLAVOUR})
  set(SIMULATOR_TARGET ${SIMULATOR_FLAVOUR}-simulator)
  add_definitions(-DSIMULATOR_FLAVOUR="${SIMULATOR_FLAVOUR}")

  if (${CMAKE_VERSION} VERSION_LESS 3.1.0})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=gnu++11")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=gnu++11")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=gnu++11")
  endif()

  include_directories(${COMPANION_SRC_DIRECTORY} ${COMPANION_SRC_DIRECTORY}/simulation)
  add_library(${SIMULATOR_TARGET} SHARED ${SIMU_SRC} opentxsimulator.cpp)
  add_dependencies(${SIMULATOR_TARGET} ${FIRMWARE_DEPENDENCIES})
  target_link_libraries(${SIMULATOR_TARGET} ${SDL_LIBRARY})
  qt5_use_modules(${SIMULATOR_TARGET} Core)
  add_custom_target(libsimulator DEPENDS ${SIMULATOR_TARGET})
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "/EHsc /LD /MP")
  include_directories(${PROJECT_SOURCE_DIR}/winbuild C:/Programs/pthreads/Pre-built.2/include C:/Programs/msinttypes C:/Programs/dirent/include)
  target_link_libraries(${SIMULATOR_TARGET} PRIVATE C:/Programs/pthreads/Pre-built.2/lib/pthreadVC2.lib)
else()
  if (${CMAKE_VERSION} VERSION_LESS 3.1.0})
    set(COMMON_CPP_FLAGS "${COMMON_FLAGS} -std=gnu++11")
    message("USING -std=gnu++11")
  else()
    set(COMMON_CPP_FLAGS ${COMMON_FLAGS})
  endif()
  set(CMAKE_C_FLAGS_DEBUG  "${COMMON_FLAGS} -g -O0")
  set(CMAKE_CXX_FLAGS_DEBUG  "${COMMON_CPP_FLAGS} -g -O0")
endif()

if((NOT WIN32) AND FOX_FOUND)
  if(SIMU_DISKIO)
    add_definitions(-DSIMU_DISKIO)
    set(SIMU_SRC ${SIMU_SRC} ${FATFS_DIR}/FatFs/ff.c ${FATFS_DIR}/option/ccsbcs.c)
  endif()

  add_executable(simu WIN32 ${SIMU_SRC} ${RADIO_SRC_DIRECTORY}/simu.cpp)
  add_dependencies(simu ${FIRMWARE_DEPENDENCIES})
  target_include_directories(simu PUBLIC ${FOX_INCLUDE_DIR} )
  target_link_libraries(simu ${FOX_LIBRARY} pthread ${SDL_LIBRARY})
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
