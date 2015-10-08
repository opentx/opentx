set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(RADIO_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/../../radio/src)

add_custom_target(about_lbm_9x
  make lbm PCB=9X
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

add_custom_target(about_lbm_taranis
  make lbm PCB=TARANIS
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

add_custom_target(commondeps
  make stamp_header tra
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${RADIO_SRC_DIRECTORY}/stamp-opentx.h)

add_custom_target(lua_exports 
  make PCB=TARANIS lua_exports_taranis.inc
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

set(OPENTX_SRC_FILES
  opentxsimulator.cpp
)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/firmwares/opentx
  ${RADIO_SRC_DIRECTORY}
  ${RADIO_SRC_DIRECTORY}/fonts/std
  ${RADIO_SRC_DIRECTORY}/lua/src
  ${RADIO_SRC_DIRECTORY}/thirdparty
  ${RADIO_SRC_DIRECTORY}/thirdparty/FatFs
  ${RADIO_SRC_DIRECTORY}/thirdparty/FatFs/option
  ${RADIO_SRC_DIRECTORY}/thirdparty/CoOS
  ${RADIO_SRC_DIRECTORY}/thirdparty/Lua/src
)

if(APPLE)
  # OS X compiler no longer automatically includes /Library/Frameworks in search path
  set(CMAKE_SHARED_LINKER_FLAGS -F/Library/Frameworks)
endif(APPLE)

if(DEFINED TRANSLATIONS)
  set(COMMON_DEFINITIONS ${COMMON_DEFINITIONS} TRANSLATIONS_${TRANSLATIONS})
  string(TOLOWER ${TRANSLATIONS} TRANSLATION_SUFFIX)
  set(SUFFIX ${SUFFIX}-${TRANSLATION_SUFFIX})
endif()

set(COMMON_DEFINITIONS
  ${COMMON_DEFINITIONS}
  SUFFIX="${SUFFIX}" 
  HELI
  GVARS
  FLIGHT_MODES
  PPM_CENTER_ADJUSTABLE
  PPM_LIMITS_SYMETRICAL
  OVERRIDE_CHANNEL_FUNCTION 
  FAI_CHOICE
)

set(COMMON_LIBRARIES
  ${QT_LIBRARIES}
  ${PTHREAD_LIBRARY}
  ${SDL_LIBRARY}
)

add_library(opentx-9x${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-9xr${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-9x128${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-9xr128${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-gruvin9x${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-mega2560${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-sky9x${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-9xrpro${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-ar9x${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranis${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranisplus${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranisx9e${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})

add_dependencies(opentx-9x${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-9xr${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-9x128${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-9xr128${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-gruvin9x${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-mega2560${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-sky9x${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-9xrpro${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-ar9x${SUFFIX}-simulator about_lbm_9x commondeps)
add_dependencies(opentx-taranis${SUFFIX}-simulator about_lbm_taranis commondeps lua_exports)
add_dependencies(opentx-taranisplus${SUFFIX}-simulator about_lbm_taranis commondeps lua_exports)
add_dependencies(opentx-taranisx9e${SUFFIX}-simulator about_lbm_taranis commondeps lua_exports)

set(OPENTX_LIBRARIES
  opentx-9x${SUFFIX}-simulator
  opentx-9xr${SUFFIX}-simulator
  opentx-9x128${SUFFIX}-simulator
  opentx-9xr128${SUFFIX}-simulator
  opentx-gruvin9x${SUFFIX}-simulator
  opentx-mega2560${SUFFIX}-simulator
  opentx-sky9x${SUFFIX}-simulator
  opentx-9xrpro${SUFFIX}-simulator
  opentx-ar9x${SUFFIX}-simulator
  opentx-taranis${SUFFIX}-simulator
  opentx-taranisplus${SUFFIX}-simulator
  opentx-taranisx9e${SUFFIX}-simulator
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  install(TARGETS ${OPENTX_LIBRARIES} LIBRARY DESTINATION ${SIMULATOR_LIB_PATH})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# 9X with ATmega64
target_include_directories(opentx-9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9x${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM64 EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega64
target_include_directories(opentx-9xr${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9xr${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM64 EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9X with ATmega128
target_include_directories(opentx-9x128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9x128${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM128 EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega128
target_include_directories(opentx-9xr128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9xr128${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM128 EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Gruvin9x
target_include_directories(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/gruvin9x)
target_compile_definitions(opentx-gruvin9x${SUFFIX}-simulator PRIVATE PCBGRUVIN9X CPUM2560 EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Mega2560
target_include_directories(opentx-mega2560${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/mega2560)
target_compile_definitions(opentx-mega2560${SUFFIX}-simulator PRIVATE PCBMEGA2560 CPUM2560 EEPROM_RLC SDCARD ROTARY_ENCODERS=2 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-mega2560${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Sky9x
target_include_directories(opentx-sky9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-sky9x${SUFFIX}-simulator PRIVATE PCBSKY9X CPUARM VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-sky9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR-PRO
target_include_directories(opentx-9xrpro${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-9xrpro${SUFFIX}-simulator PRIVATE PCBSKY9X REVX CPUARM VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xrpro${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# AR9X
target_include_directories(opentx-ar9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-ar9x${SUFFIX}-simulator PRIVATE PCBSKY9X AR9X CPUARM VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-ar9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis standard
target_include_directories(opentx-taranis${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranis${SUFFIX}-simulator PRIVATE PCBTARANIS CPUARM CPUSTM32 EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranis${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis Plus 
target_include_directories(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisplus${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS CPUARM CPUSTM32 EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis X9E
target_include_directories(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisx9e${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS REV9E CPUARM CPUSTM32 STM32F40_41xxx EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

add_custom_target(opentx-simulators
  DEPENDS ${OPENTX_LIBRARIES}
)

if(APPLE)
  set(SIMULATOR_BUNDLES)
  foreach(_file ${OPENTX_LIBRARIES})
    set(SIMULATOR_BUNDLE "${_file}-bundle")
    add_custom_target(${SIMULATOR_BUNDLE}
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore lib${_file}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork lib${_file}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml lib${_file}.dylib
      COMMAND install_name_tool -change /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui lib${_file}.dylib
      COMMAND install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/../Frameworks/SDL.framework/Versions/A/SDL lib${_file}.dylib
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMAND pwd
      COMMAND cp lib${_file}.dylib companion.app/Contents/Resources/
      DEPENDS ${_file}
    )
    list(APPEND SIMULATOR_BUNDLES ${SIMULATOR_BUNDLE})
  endforeach()
  add_custom_target(opentx-simulators-bundle DEPENDS ${SIMULATOR_BUNDLES})
endif(APPLE)

