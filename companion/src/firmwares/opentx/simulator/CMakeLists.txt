set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(RADIO_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/../../radio/src)

macro(add_bitmaps_target targetname filter width format) 
  file(GLOB bitmaps ${filter})
  foreach(bitmap ${bitmaps})
    get_filename_component(target ${bitmap} NAME_WE)
    get_filename_component(path ${bitmap} DIRECTORY)
    set(target ${path}/${target}.lbm)
    add_custom_command(
      OUTPUT ${target}
      COMMAND python ../util/img2lbm.py ${bitmap} ${target} ${width} ${format}
      DEPENDS ${bitmap}
      WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
    )
    list(APPEND bitmaps_files ${target})
  endforeach()
  add_custom_target(${targetname} DEPENDS ${bitmaps_files})
endmacro(add_bitmaps_target)

add_bitmaps_target(lbm_xbm_1bit ${RADIO_SRC_DIRECTORY}/bitmaps/*.xbm 128 1bit)
add_bitmaps_target(lbm_fonts_1bit ${RADIO_SRC_DIRECTORY}/fonts/std/*.png 128 "")
add_bitmaps_target(lbm_9x_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/9x/*.png 128 1bit)
add_bitmaps_target(lbm_taranis_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/taranis/*.png 212 4bits)

add_dependencies(lbm_9x_bitmaps lbm_fonts_1bit lbm_xbm_1bit)
add_dependencies(lbm_taranis_bitmaps lbm_fonts_1bit lbm_xbm_1bit)

macro(add_truetype_font_target radio name font size)
  set(target ${RADIO_SRC_DIRECTORY}/fonts/${radio}/font_${name})
  add_custom_command(
    OUTPUT ${target}.png
    OUTPUT ${target}.specs
    COMMAND ../util/font2png.py ${font} ${size} False ${target}
    WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
  )
  add_custom_target(ttf_${radio}_${name} DEPENDS ${target}.png ${target}.specs)
endmacro(add_truetype_font_target)

add_truetype_font_target(horus tinsize "DejaVu Sans" 9)
add_truetype_font_target(horus smlsize "DejaVu Sans" 13)
add_truetype_font_target(horus stdsize "DejaVu Sans" 16)
add_truetype_font_target(horus midsize "DejaVu Sans" 24)
add_truetype_font_target(horus dblsize "DejaVu Sans" 32)
add_truetype_font_target(horus xxlsize "DejaVu Sans" 48)

add_bitmaps_target(lbm_horus_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/horus/????[^_]*.png" 480 5/6/5/8)
add_bitmaps_target(lbm_horus_masks ${RADIO_SRC_DIRECTORY}/bitmaps/horus/mask_*.png 480 8bits)
add_bitmaps_target(lbm_horus_fonts ${RADIO_SRC_DIRECTORY}/fonts/horus/*.png 480 8bits)
add_dependencies(lbm_horus_fonts ttf_Horus_tinsize ttf_Horus_smlsize ttf_Horus_stdsize ttf_Horus_midsize ttf_Horus_dblsize ttf_Horus_xxlsize)
add_dependencies(lbm_horus_bitmaps lbm_horus_masks lbm_horus_fonts)

file(GLOB translations ${RADIO_SRC_DIRECTORY}/translations/*.h.txt)
foreach(translation ${translations})
  get_filename_component(language ${translation} NAME_WE)
  get_filename_component(path ${translation} DIRECTORY)
  set(target ${path}/${language}.h)
  add_custom_command(
    OUTPUT ${target}
    COMMAND python ../util/translate.py ${translation} ${target} ${language}
    DEPENDS ${translation}
    WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
  )
  list(APPEND translations_files ${target})
endforeach()

add_custom_target(firmware_translations DEPENDS ${translations_files})

TODAY(DATE)
NOW(TIME)
GIT_ID(GIT_STR)

configure_file(${RADIO_SRC_DIRECTORY}/stamp.h.in ${RADIO_SRC_DIRECTORY}/stamp.h @ONLY)

macro(add_lua_export_target target)
  add_custom_command(
    OUTPUT ${RADIO_SRC_DIRECTORY}/lua/lua_exports_${target}.inc
    COMMAND ${CMAKE_C_COMPILER} -E ${ARGN} -DEXPORT ${INTTYPES_INCLUDE_OPTION} myeeprom.h | grep LEXP > lua/lua_exports_${target}.txt
    COMMAND python ../util/luaexport.py ${VERSION} lua/lua_exports_${target}.txt lua/lua_exports_${target}.inc lua/lua_fields_${target}.txt
    WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
  )
  add_custom_target(lua_export_${target} DEPENDS ${RADIO_SRC_DIRECTORY}/lua/lua_exports_${target}.inc)
endmacro(add_lua_export_target)

add_lua_export_target(taranis -DPCBTARANIS -DLUA)
add_lua_export_target(taranis_x9e -DPCBTARANIS -DREVPLUS -DREV9E -DLUA)
add_lua_export_target(horus -DPCBHORUS -DLUA)

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
add_library(opentx-horus${SUFFIX}-simulator SHARED ${OPENTX_SRC_FILES})

set(dependencies_9x lbm_9x_bitmaps firmware_translations stamp)
add_dependencies(opentx-9x${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-9xr${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-9x128${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-9xr128${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-gruvin9x${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-mega2560${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-sky9x${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-9xrpro${SUFFIX}-simulator ${dependencies_9x})
add_dependencies(opentx-ar9x${SUFFIX}-simulator ${dependencies_9x})
set(dependencies_taranis lbm_taranis_bitmaps firmware_translations stamp)
add_dependencies(opentx-taranis${SUFFIX}-simulator ${dependencies_taranis} lua_export_taranis)
add_dependencies(opentx-taranisplus${SUFFIX}-simulator ${dependencies_taranis} lua_export_taranis)
add_dependencies(opentx-taranisx9e${SUFFIX}-simulator ${dependencies_taranis} lua_export_taranis_x9e)

add_dependencies(opentx-horus${SUFFIX}-simulator lbm_horus_bitmaps lbm_horus_fonts firmware_translations lua_export_horus)

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
  opentx-horus${SUFFIX}-simulator
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  install(TARGETS ${OPENTX_LIBRARIES} LIBRARY DESTINATION ${SIMULATOR_LIB_PATH})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# 9X with ATmega64
target_include_directories(opentx-9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/9x)
target_compile_definitions(opentx-9x${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM64 EEPROM EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega64
target_include_directories(opentx-9xr${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/9x)
target_compile_definitions(opentx-9xr${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM64 EEPROM EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9X with ATmega128
target_include_directories(opentx-9x128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/9x)
target_compile_definitions(opentx-9x128${SUFFIX}-simulator PRIVATE PCBSTD PCB9X CPUM128 EEPROM EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9x128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR with ATmega128
target_include_directories(opentx-9xr128${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/9x)
target_compile_definitions(opentx-9xr128${SUFFIX}-simulator PRIVATE PCBSTD PCB9XR CPUM128 EEPROM EEPROM_RLC ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xr128${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Gruvin9x
target_include_directories(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/gruvin9x)
target_compile_definitions(opentx-gruvin9x${SUFFIX}-simulator PRIVATE PCBGRUVIN9X CPUM2560 EEPROM EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-gruvin9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Mega2560
target_include_directories(opentx-mega2560${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/mega2560)
target_compile_definitions(opentx-mega2560${SUFFIX}-simulator PRIVATE PCBMEGA2560 CPUM2560 EEPROM EEPROM_RLC SDCARD ROTARY_ENCODERS=2 ${COMMON_DEFINITIONS})
target_link_libraries(opentx-mega2560${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Sky9x
target_include_directories(opentx-sky9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-sky9x${SUFFIX}-simulator PRIVATE PCBSKY9X CPUARM VOICE EEPROM SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-sky9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# 9XR-PRO
target_include_directories(opentx-9xrpro${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-9xrpro${SUFFIX}-simulator PRIVATE PCBSKY9X REVX CPUARM VOICE EEPROM SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-9xrpro${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# AR9X
target_include_directories(opentx-ar9x${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/sky9x)
target_compile_definitions(opentx-ar9x${SUFFIX}-simulator PRIVATE PCBSKY9X AR9X CPUARM VOICE EEPROM SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-ar9x${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis standard
target_include_directories(opentx-taranis${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranis${SUFFIX}-simulator PRIVATE PCBTARANIS CPUARM CPUSTM32 EEPROM EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranis${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis Plus 
target_include_directories(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisplus${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS CPUARM CPUSTM32 EEPROM EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisplus${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Taranis X9E
target_include_directories(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/taranis)
target_compile_definitions(opentx-taranisx9e${SUFFIX}-simulator PRIVATE PCBTARANIS REVPLUS REV9E CPUARM CPUSTM32 STM32F40_41xxx EEPROM EEPROM_RLC VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-taranisx9e${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

# Horus
target_include_directories(opentx-horus${SUFFIX}-simulator PRIVATE ${RADIO_SRC_DIRECTORY}/targets/horus)
target_compile_definitions(opentx-horus${SUFFIX}-simulator PRIVATE PCBHORUS CPUARM CPUSTM32 STM32F40_41xxx COLORLCD VOICE SDCARD ${COMMON_DEFINITIONS})
target_link_libraries(opentx-horus${SUFFIX}-simulator PRIVATE ${COMMON_LIBRARIES})

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

