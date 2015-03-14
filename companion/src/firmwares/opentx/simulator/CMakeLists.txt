set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(RADIO_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/../../radio/src)

add_custom_target( about_lbm_9x 
  make lbm PCB=9X
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

add_custom_target( about_lbm_taranis 
  make lbm PCB=TARANIS
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

add_custom_target( trans
  make tra
  WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
)

add_custom_target( lua_exports 
  make PCB=TARANIS lua_exports.inc
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
)

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
add_library(opentx-sky9x${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranis${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranisplus${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})
add_library(opentx-taranisx9e${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})

add_dependencies(opentx-9x${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-9xr${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-9x128${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-9xr128${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-gruvin9x${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-sky9x${SUFFIX}-simulator about_lbm_9x trans)
add_dependencies(opentx-taranis${SUFFIX}-simulator about_lbm_taranis trans lua_exports)
add_dependencies(opentx-taranisplus${SUFFIX}-simulator about_lbm_taranis trans lua_exports)
add_dependencies(opentx-taranisx9e${SUFFIX}-simulator about_lbm_taranis trans lua_exports)

set(OPENTX_LIBRARIES
  opentx-9x${SUFFIX}-simulator
  opentx-9xr${SUFFIX}-simulator
  opentx-9x128${SUFFIX}-simulator
  opentx-9xr128${SUFFIX}-simulator
  opentx-gruvin9x${SUFFIX}-simulator
  opentx-sky9x${SUFFIX}-simulator
  opentx-taranis${SUFFIX}-simulator
  opentx-taranisplus${SUFFIX}-simulator
  opentx-taranisx9e${SUFFIX}-simulator
)

# 9X with ATmega64
target_include_directories(opentx-9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9x${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM64 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega64
target_include_directories(opentx-9xr${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9xr${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM64 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9X with ATmega128
target_include_directories(opentx-9x128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9x128${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM128 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega128
target_include_directories(opentx-9xr128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/stock)
target_compile_definitions(opentx-9xr128${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM128 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Gruvin9x
target_include_directories(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/gruvin9x)
target_compile_definitions(opentx-gruvin9x${SUFFIX}-simulator PRIVATE PCBGRUVIN9X CPUM2560 VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Sky9x
target_include_directories(opentx-sky9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-sky9x${SUFFIX}-simulator PRIVATE PCBSKY9X CPUARM VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-sky9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis standard
target_include_directories(opentx-taranis${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranis${SUFFIX}-simulator PRIVATE PCBTARANIS CPUARM CPUSTM32 VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranis${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis Plus 
target_include_directories(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisplus${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS CPUARM CPUSTM32 VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis X9E
target_include_directories(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisx9e${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS REV9E CPUARM CPUSTM32 STM32F40_41xxx VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

ADD_CUSTOM_TARGET(opentx-simulators
  DEPENDS ${OPENTX_LIBRARIES}
)
