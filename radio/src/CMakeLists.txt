include(CMakeForceCompiler)
include(Bitmaps)

set(PCB "X9D+" CACHE STRING "Radio type")
set(TRANSLATIONS "EN" CACHE STRING "Radio language")
option(HELI "Heli menu" ON)
option(FLIGHT_MODES "Flight Modes" ON)
option(CURVES "Curves" ON)
option(GVARS "Global variables" OFF)
option(GUI "GUI enabled" ON)
set(SPLASH "DEFAULT" CACHE STRING "Splash (DEFAULT/OFF/FRSKY)")
set(PPM_UNIT "PERCENT_PREC1" CACHE STRING "PPM display unit (US/PERCENT_PREC1/PERCENT_PREC0)")
option(PPM_CENTER_ADJUSTABLE "PPM center adjustable" ON)
option(PPM_LIMITS_SYMETRICAL "PPM limits symetrical" OFF)
option(OVERRIDE_CHANNEL_FUNCTION "OverrideChannel function available" ON)
option(DANGEROUS_MODULE_FUNCTIONS "Dangerous module functions (RangeCheck / Bind / Module OFF, etc.) available" OFF)
option(FAI "Competition mode (no telemetry)" OFF)
option(AUTOSOURCE "Automatic source detection in menus" ON)
option(AUTOSWITCH "Automatic switch detection in menus" ON)
option(JITTER_MEASURE "Enable ADC jitter measurement" OFF)
option(WATCHDOG_DISABLED "Disable hardware Watchdog" OFF)
option(SIMU_AUDIO "Enable simulator audio" OFF)
option(SIMU_DISKIO "Enable disk IO simulation in simulator. Simulator will use FatFs module and simulated IO layer that  uses \"./sdcard.image\" file as image of SD card. This file must contain whole SD card from first to last sector" OFF)
option(FAS_PROTOTYPE "Support of old FAS prototypes (different resistors)" OFF)
option(TEMPLATES "Model templates menu" OFF)
option(WARNINGS_AS_ERRORS "Treat all compiler warnings as error" OFF)
option(TRACE_SIMPGMSPACE "Turn on traces in simpgmspace.cpp" ON)
set(DEFAULT_MODE "" CACHE STRING "Default sticks mode")

enable_language(ASM)
set(OPT s)

set(THIRDPARTY_DIR thirdparty)
set(LUA_DIR ${THIRDPARTY_DIR}/Lua/src)
set(COOS_DIR ${THIRDPARTY_DIR}/CoOS)
set(FATFS_DIR ${THIRDPARTY_DIR}/FatFs)
set(RADIO_BIN_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(GUI_LANGUAGES cz de en es fr it pt sk se pl hu nl)
set(TTS_LANGUAGES cz de en es fr it pt sk se pl hu)

configure_file(stamp.h.in stamp.h @ONLY)

add_subdirectory(translations)
add_subdirectory(bitmaps)
add_subdirectory(fonts)

set(EEPROM_VARIANT  0)
set(GVARS_VARIANT   1)
set(FRSKY_VARIANT   2)
set(3POS_VARIANT    4)
set(MAVLINK_VARIANT 8)
set(M2561_VARIANT   16384)
set(M128_VARIANT    32768)

set(FIRMWARE_DEPENDENCIES firmware_translations)

set(FATFS_SRC
  ${FATFS_DIR}/ff.c
  ${FATFS_DIR}/fattime.c
  ${FATFS_DIR}/option/ccsbcs.c
)

if(PCB STREQUAL HORUS)
  include(targets/horus/CMakeLists.txt)
elseif(PCB STREQUAL X9E OR PCB STREQUAL X9D+ OR PCB STREQUAL X9D OR PCB STREQUAL X7)
  include(targets/taranis/CMakeLists.txt)
elseif(PCB STREQUAL SKY9X OR PCB STREQUAL 9XRPRO OR PCB STREQUAL AR9X)
  include(targets/sky9x/CMakeLists.txt)
elseif(PCB STREQUAL 9X OR PCB STREQUAL 9XR OR PCB STREQUAL 9X128 OR PCB STREQUAL 9XR128)
  include(targets/9x/CMakeLists.txt)
elseif(PCB STREQUAL 9X2561)
  set(ARCH AVR)
  set(MCU atmega2561)
  string(TOLOWER ${PCB} FLAVOUR)
  set(EEPROM EEPROM_RLC)
  add_definitions(-DEEPROM_VARIANT=${M2561_VARIANT})
  set(GUI_DIR 128x64)
  set(TARGET_DIR 9x)
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} 9x_bitmaps)
  add_definitions(-DPCBSTD -DPCB${PCB} -DCPUM2561)
  set(TARGET_SRC ${TARGET_SRC} board.cpp)
  set(GUI_SRC
    ${GUI_SRC}
    model_inputs_mixes.cpp
    radio_diagkeys.cpp
    radio_diaganas.cpp)
  if(SP22)
    add_definitions(-DSP22)
  endif()
  if(PWM_BACKLIGHT)
    add_definitions(-DPWM_BACKLIGHT)
  endif()
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} lcd_driver.cpp)
elseif(PCB STREQUAL GRUVIN9X)
  include(targets/gruvin9x/CMakeLists.txt)
elseif(PCB STREQUAL MEGA2560)
  include(targets/mega2560/CMakeLists.txt)
else()
  message(FATAL_ERROR "Unknown PCB '${PCB}'")
endif()

if(NOT PCB STREQUAL 9XR AND NOT PCB STREQUAL 9XRPRO)
  option(DBLKEYS "Double Keys" ON)
  if(DBLKEYS)
    add_definitions(-DDBLKEYS)
  endif()
endif()

if(CPU_TYPE STREQUAL STM32F4)
  include(targets/common/arm/stm32/f4/CMakeLists.txt)
endif()

if(CPU_TYPE STREQUAL STM32F2)
  include(targets/common/arm/stm32/f2/CMakeLists.txt)
endif()

if(CPU_FAMILY STREQUAL STM32)
  include(targets/common/arm/stm32/CMakeLists.txt)
endif()

if(ARCH STREQUAL ARM)
  include(targets/common/arm/CMakeLists.txt)
else()
  include(targets/common/avr/CMakeLists.txt)
endif()

include_directories(targets/${TARGET_DIR} ${THIRDPARTY_DIR})

foreach(LANGUAGE ${GUI_LANGUAGES})
  string(TOUPPER ${LANGUAGE} LANGUAGE_CAPITALIZED)
  if(TRANSLATIONS STREQUAL ${LANGUAGE_CAPITALIZED})
    add_definitions(-DTRANSLATIONS_${LANGUAGE_CAPITALIZED} -DTRANSLATIONS="${LANGUAGE_CAPITALIZED}")
  endif()
endforeach()

set(SRC ${SRC} debug.cpp)

if(${EEPROM} STREQUAL SDCARD)
  set(SRC ${SRC} storage/storage_common.cpp storage/sdcard_raw.cpp)
elseif(${EEPROM} STREQUAL EEPROM_RLC)
  set(SRC ${SRC} storage/storage_common.cpp storage/eeprom_common.cpp storage/eeprom_rlc.cpp)
  add_definitions(-DEEPROM -DEEPROM_RLC)
else()
  set(SRC ${SRC} storage/storage_common.cpp storage/eeprom_common.cpp storage/eeprom_raw.cpp)
  add_definitions(-DEEPROM -DEEPROM_RAW)
endif()

if(ARCH STREQUAL ARM AND NOT PCB STREQUAL HORUS AND NOT PCB STREQUAL FLAMENCO)
  set(SRC ${SRC} storage/eeprom_conversions.cpp)
endif()

add_definitions(-DFLAVOUR="${FLAVOUR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/bitmaps/${GUI_DIR})

add_subdirectory(lua)
include(gui/CMakeLists.txt)

if(RAMBACKUP)
  add_definitions(-DRAMBACKUP)
  set(SRC ${SRC} storage/rambackup.cpp storage/rlc.cpp)
endif()

if(TEMPLATES)
  add_definitions(-DTEMPLATES)
  set(SRC ${SRC} templates.cpp)
  set(GUI_SRC ${GUI_SRC} model_templates.cpp)
endif()

if(NOT LUA STREQUAL NO)
  add_definitions(-DLUA)
  include_directories(${LUA_DIR})
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${LUA_EXPORT})
  if(LUA STREQUAL YES)
    add_definitions(-DLUA_MODEL_SCRIPTS)
    set(GUI_SRC ${GUI_SRC} model_custom_scripts.cpp)
  endif()
  set(SRC ${SRC} lua/interface.cpp lua/api_general.cpp lua/api_lcd.cpp lua/api_model.cpp)
  if(PCB STREQUAL HORUS)
    set(SRC ${SRC} lua/widgets.cpp)
  endif()
  set(LUA_SRC lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c
    lstate.c lstring.c ltable.c lrotable.c ltm.c lundump.c lvm.c lzio.c linit.c
    lbaselib.c lmathlib.c lbitlib.c loadlib.c lauxlib.c ltablib.c lcorolib.c liolib.c lstrlib.c
    )
  foreach(FILE ${LUA_SRC})
    set(SRC ${SRC} ${LUA_DIR}/${FILE})
  endforeach()
endif()

if(HELI)
  add_definitions(-DHELI)
endif()

if(FLIGHT_MODES)
  add_definitions(-DFLIGHT_MODES)
  set(GUI_SRC ${GUI_SRC} model_flightmodes.cpp)
endif()

if(CURVES)
  add_definitions(-DCURVES)
  set(SRC ${SRC} curves.cpp)
endif()

if(GVARS)
  add_definitions(-DGVARS)
  set(SRC ${SRC} gvars.cpp)
  math(EXPR EEPROM_VARIANT ${EEPROM_VARIANT}+${GVARS_VARIANT})
endif()

if(FAI STREQUAL YES)
  add_definitions(-DFAI)
elseif(FAI STREQUAL CHOICE)
  add_definitions(-DFAI_CHOICE)
endif()

if(FAS_PROTOTYPE)
  add_definitions(-DFAS_PROTOTYPE)
endif()

if(AUTOSOURCE)
  add_definitions(-DAUTOSOURCE)
endif()

if(AUTOSWITCH)
  add_definitions(-DAUTOSWITCH)
endif()

if(JITTER_MEASURE)
  add_definitions(-DJITTER_MEASURE)
endif()

if(WATCHDOG_DISABLED)
  add_definitions(-DWATCHDOG_DISABLED)
endif()

if(SIMU_AUDIO)
  add_definitions(-DSIMU_AUDIO)
endif()

if(SDCARD)
  add_definitions(-DSDCARD)
  include_directories(${FATFS_DIR} ${FATFS_DIR}/option)
  set(SRC ${SRC} sdcard.cpp rtc.cpp logs.cpp)
  set(FIRMWARE_SRC ${FIRMWARE_SRC} ${FATFS_SRC})
endif()

if(SHUTDOWN_CONFIRMATION)
  add_definitions(-DSHUTDOWN_CONFIRMATION)
endif()

if(PPM_UNIT STREQUAL US)
  add_definitions(-DPPM_UNIT_US)
elseif(PPM_UNIT STREQUAL PERCENT_PREC1)
  add_definitions(-DPPM_UNIT_PERCENT_PREC1)
else()
  add_definitions(-DPPM_UNIT_PERCENT_PREC0)
endif()

if(PPM_CENTER_ADJUSTABLE)
  add_definitions(-DPPM_CENTER_ADJUSTABLE)
endif()

if(PPM_LIMITS_SYMETRICAL)
  add_definitions(-DPPM_LIMITS_SYMETRICAL)
endif()

if(OVERRIDE_CHANNEL_FUNCTION)
  add_definitions(-DOVERRIDE_CHANNEL_FUNCTION)
endif()

if(DANGEROUS_MODULE_FUNCTIONS)
  add_definitions(-DDANGEROUS_MODULE_FUNCTIONS)
endif()

if(SPLASH STREQUAL FRSKY)
  add_definitions(-DSPLASH -DSPLASH_FRSKY)
elseif(NOT SPLASH STREQUAL OFF)
  add_definitions(-DSPLASH)
endif()

if(NOT DEFAULT_MODE STREQUAL "")
  add_definitions(-DDEFAULT_MODE=${DEFAULT_MODE})
endif()

if(TRACE_SIMPGMSPACE)
  add_definitions(-DTRACE_SIMPGMSPACE)
endif()

set(SRC
  ${SRC}
  opentx.cpp
  functions.cpp
  strhelpers.cpp
  switches.cpp
  mixer.cpp
  stamp.cpp
  timers.cpp
  trainer_input.cpp
  )

if(GUI)
  add_definitions(-DGUI)
  set(SRC
    ${SRC}
    keys.cpp
    maths.cpp
    translations.cpp
    gui/gui_common_arm.cpp
    )
  foreach(FILE ${GUI_SRC})
    set(SRC ${SRC} gui/${GUI_DIR}/${FILE})
  endforeach()
endif()

foreach(FILE ${TARGET_SRC})
  set(SRC targets/${TARGET_DIR}/${FILE} ${SRC})
endforeach()

foreach(FILE ${FIRMWARE_TARGET_SRC})
  set(FIRMWARE_SRC ${FIRMWARE_SRC} targets/${TARGET_DIR}/${FILE})
endforeach()

foreach(FILE ${PULSES_SRC})
  set(SRC ${SRC} pulses/${FILE})
endforeach()

add_definitions(-DCORRECT_NEGATIVE_SHIFTS)

if(NOT WIN32)
  add_definitions(-Wall -Wno-strict-aliasing -Wformat -Wreturn-type -Wunused -Wuninitialized -Wunknown-pragmas -Wno-switch -Wtype-limits)
  if(WARNINGS_AS_ERRORS)
    set(WARNING_FLAGS -Werror)
  endif(WARNINGS_AS_ERRORS)
else()
  add_definitions(-DHAVE_STRUCT_TIMESPEC)
endif()

add_subdirectory(targets/simu)
if(NOT WIN32)
  add_subdirectory(tests)
endif()

set(SRC ${SRC} ${FIRMWARE_SRC})

# trick to remove the -rdynamic and --out-implib issues
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_COMMON_FLAGS "")
# similar for -mmacosx-version-min="
set(CMAKE_C_OSX_DEPLOYMENT_TARGET_FLAG "")
set(CMAKE_CXX_OSX_DEPLOYMENT_TARGET_FLAG "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET>")

if(NOT MSVC)
  if(ARCH STREQUAL ARM)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-none-eabi-as)

    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_VERSION 1)
    set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> -o <OBJECT> <SOURCE>")

    set(COMMON_FLAGS "-mcpu=${MCU} -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=firmware.lst -O${OPT} -gdwarf-2 -DHSE_VALUE=${HSE_VALUE} -fno-exceptions -fdata-sections -ffunction-sections ${WARNING_FLAGS}")
    # Remove once the minimum cmake version is set to 3.1 in the main CmakeLists.tx
    if (${CMAKE_VERSION} VERSION_LESS 3.1.0})
      set(COMMON_CPP_FLAGS "${COMMON_FLAGS} -std=gnu++11")
    else()
      set(COMMON_CPP_FLAGS ${COMMON_FLAGS})
    endif()
    set(CMAKE_C_FLAGS "${COMMON_FLAGS} -Wimplicit")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${COMMON_FLAGS} -Wimplicit -g")
    set(CMAKE_C_FLAGS_RELEASE "${COMMON_FLAGS} -Wimplicit")
    set(CMAKE_C_FLAGS_DEBUG "${COMMON_FLAGS} -Wimplicit -g")
    set(CMAKE_CXX_FLAGS "${COMMON_CPP_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${COMMON_CPP_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_CPP_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_CPP_FLAGS} -g")
    set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU} -mthumb -lm -T${RADIO_SRC_DIRECTORY}/${LINKER_SCRIPT} -Wl,-Map=firmware.map,--cref,--no-warn-mismatch,--gc-sections")

    if(PCB STREQUAL X9D OR PCB STREQUAL X9D+ OR PCB STREQUAL X9E OR PCB STREQUAL X7)
      add_subdirectory(targets/${TARGET_DIR}/bootloader)
      include_directories(${CMAKE_CURRENT_BINARY_DIR}/targets/${TARGET_DIR}/bootloader)
      set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} bootloader)
    elseif(OPENRC_BOOTLOADER)
      # We fetch Mike's bootloader as it is not included inside OpenTX
      message(STATUS "Downloading Mike's bootloader: https://github.com/MikeBland/OpenRcBootloader/releases/download/V1.9/${OPENRC_BOOTLOADER}")
      file(DOWNLOAD https://github.com/MikeBland/OpenRcBootloader/releases/download/V1.9/${OPENRC_BOOTLOADER} ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lbm STATUS status)
      list(GET status 0 error_code)
      if(error_code)
        list(GET status 1 error_message)
        message(FATAL_ERROR "Download failed: ${error_message}")
      endif()
    endif()

    add_executable(firmware ${SRC} ${FIRMWARE_HEADERS})
    link_libraries(firmware -lstdc++)
    add_dependencies(firmware ${FIRMWARE_DEPENDENCIES})

    add_custom_command(
      TARGET firmware POST_BUILD
      COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(firmware-size
      COMMAND arm-none-eabi-size firmware.elf
      DEPENDS firmware
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      )
  else()
    set(CMAKE_C_COMPILER avr-gcc)
    set(CMAKE_CXX_COMPILER avr-gcc)
    set(CMAKE_CXX_FLAGS "-mmcu=${MCU} -DF_CPU=16000000UL -O${OPT} -gdwarf-2 -fno-exceptions -flto -fwhole-program -fno-inline-small-functions -mstrict-X ${WARNING_FLAGS}")

    # Remove once the minimum cmake version is set to 3.1 in the main CmakeLists.tx
    if (${CMAKE_VERSION} VERSION_LESS 3.1.0})
      set(CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif()

    include_directories(storage gui/${GUI_DIR})

    set(ALLSRC ${PROJECT_BINARY_DIR}/allsrc.cpp)
    foreach(FILE ${SRC})
      file(READ ${FILE} FILE_CONTENT)
      set(ALLSRC_CONTENT "${ALLSRC_CONTENT}# 1 \"${FILE}\"\n${FILE_CONTENT}")
    endforeach()

    file(WRITE ${ALLSRC} "${ALLSRC_CONTENT}")

    add_executable(firmware ${ALLSRC})
    add_dependencies(firmware ${FIRMWARE_DEPENDENCIES})

    add_custom_command(
      TARGET firmware POST_BUILD
      COMMAND avr-objcopy -O ihex -R .eeprom firmware.elf firmware.hex
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      )

    add_custom_target(firmware.lss
      COMMAND avr-objdump -h -S firmware.elf > firmware.lss
      DEPENDS firmware
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      )

    add_custom_target(firmware-size
      COMMAND avr-size --format=avr --mcu=${MCU} firmware.elf
      DEPENDS firmware
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      )
  endif()

  if(CPU_FAMILY STREQUAL STM32)
    add_custom_target(flash
      COMMAND dfu-util --alt 0 --dfuse-address 0x08000000:leave -d 0483:df11 -D firmware.bin
      DEPENDS firmware
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(jtag-flash
      COMMAND st-flash --reset write firmware.bin 0x8000000
      DEPENDS firmware
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      )
  endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(NOT MSVC)
