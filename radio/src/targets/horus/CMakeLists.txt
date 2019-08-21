option(DISK_CACHE "Enable SD card disk cache" ON)
option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" ON)
option(PXX1 "PXX1 protocol support" ON)
option(PXX2 "PXX2 protocol support" OFF)

set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
set(CPU_TYPE STM32F4)
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM SDCARD)
set(HAPTIC YES)
set(GUI_DIR 480x272)
set(NAVIGATION_TYPE horus)
set(TARGET_DIR horus)
set(LINKER_SCRIPT targets/horus/stm32f4_flash.ld)
set(RAMBACKUP YES)
set(LUA YES)
set(PPM_LIMITS_SYMETRICAL YES)
set(USB_SERIAL ON CACHE BOOL "Enable USB serial (CDC)")

# for size report script
set(CPU_TYPE_FULL STM32F429xI)
set(SIZE_TARGET_MEM_DEFINE "MEM_SIZE_SDRAM2=8192")

if(SPLASH STREQUAL OFF)
  message(FATAL_ERROR "Option SPLASH=OFF not supported on Horus")
endif()

if (PCB STREQUAL X10)
  set(FLAVOUR x10)
  set(PCBREV "STD" CACHE STRING "PCB Revision")
  add_definitions(-DPCBX10)
  add_definitions(-DSOFTWARE_VOLUME)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/audio_dac_driver.cpp
    ../common/arm/stm32/adc_driver.cpp
    ../common/arm/stm32/sticks_pwm_driver.cpp
    )
  set(BITMAPS_TARGET x10_bitmaps)
  set(FONTS_TARGET x10_fonts)
  set(LCD_DRIVER lcd_driver.cpp)
  set(LUA_EXPORT lua_export_x10)
  if (PCBREV STREQUAL EXPRESS)
    option(INTERNAL_MODULE_PXX1 "Support for PXX1 internal module" OFF)
    option(INTERNAL_MODULE_PXX2 "Support for PXX2 internal module" ON)
  else()
    option(INTERNAL_MODULE_PXX1 "Support for PXX1 internal module" ON)
    option(INTERNAL_MODULE_PXX2 "Support for PXX2 internal module" OFF)
  endif()
elseif (PCB STREQUAL X12S)
  set(FLAVOUR x12s)
  set(PCBREV "13" CACHE STRING "PCB Revision")
  if(${PCBREV} GREATER 10)
    option(INTERNAL_GPS "Internal GPS installed" YES)
  else()
    option(INTERNAL_GPS "Internal GPS installed" NO)
    if(NOT INTERNAL_GPS)
      message("Horus: Internal GPS is optional, use INTERNAL_GPS=YES option to enable it")
    endif()
  endif()
  set(INTERNAL_GPS_BAUDRATE "9600" CACHE STRING "Baud rate for internal GPS")
  add_definitions(-DPCBX12S)
  set(TARGET_SRC
    ${TARGET_SRC}
    audio_spi_driver.cpp
    adc_driver.cpp
    gps_driver.cpp
    )
  set(AUX_SERIAL_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)
  set(BITMAPS_TARGET x12s_bitmaps)
  set(FONTS_TARGET x12s_fonts)
  set(LCD_DRIVER lcd_driver.cpp)
  set(LUA_EXPORT lua_export_x12s)
endif()

add_definitions(-DPCBREV=${PCBREV})
add_definitions(-DPCBREV_${PCBREV})

set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${BITMAPS_TARGET})

add_definitions(-DPCBHORUS -DSTM32F429_439xx -DSDRAM -DCOLORLCD)
add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DGPS_USART_BAUDRATE=${INTERNAL_GPS_BAUDRATE})
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

if(INTERNAL_MODULE_PXX1)
  add_definitions(-DINTERNAL_MODULE_PXX1)
endif()

if(INTERNAL_MODULE_PXX2)
  set(PXX2 ON)
  add_definitions(-DINTERNAL_MODULE_PXX2)
endif()

include_directories(${RADIO_SRC_DIRECTORY}/fonts/480x272 gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

file(GLOB THEMES_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/480x272 ${RADIO_SRC_DIRECTORY}/gui/480x272/themes/*.cpp)
file(GLOB LAYOUTS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/480x272 ${RADIO_SRC_DIRECTORY}/gui/480x272/layouts/*.cpp)
file(GLOB WIDGETS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/480x272 ${RADIO_SRC_DIRECTORY}/gui/480x272/widgets/*.cpp)
set(GUI_SRC
  ${GUI_SRC}
  bitmapbuffer.cpp
  curves.cpp
  bitmaps.cpp
  radio_sdmanager.cpp
  radio_hardware.cpp
  model_inputs.cpp
  model_mixes.cpp
  model_curves.cpp
  view_channels.cpp
  view_logical_switches.cpp
  view_about.cpp
  view_text.cpp
  screens_setup.cpp
  theme.cpp
  topbar.cpp
  layout.cpp
  widget.cpp
  menus.cpp
  ${THEMES_SRC}
  ${LAYOUTS_SRC}
  ${WIDGETS_SRC}
  rle.cpp
  )

if(PXX2 OR LUA)
  set(GUI_SRC ${GUI_SRC} radio_tools.cpp)
endif()

if(PXX2)
  set(GUI_SRC
    ${GUI_SRC}
    radio_spectrum_analyser.cpp
    radio_power_meter.cpp
    )
endif()

if(DISK_CACHE)
  set(SRC ${SRC} disk_cache.cpp)
  add_definitions(-DDISK_CACHE)
endif()

if(INTERNAL_GPS)
  set(SRC ${SRC} gps.cpp)
  add_definitions(-DINTERNAL_GPS)
  message("Horus: Internal GPS enabled")
endif()

set(GVAR_SCREEN model_gvars.cpp)

set(TARGET_SRC
  ${TARGET_SRC}
  led_driver.cpp
  extmodule_driver.cpp
  trainer_driver.cpp
  ../common/arm/stm32/heartbeat_driver.cpp
  ../common/arm/stm32/timers_driver.cpp
  ../common/arm/stm32/intmodule_serial_driver.cpp
  ../common/arm/stm32/bluetooth_driver.cpp
  ../common/arm/stm32/rotary_encoder_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
  board.cpp
  backlight_driver.cpp
  pwr_driver.cpp
  sdram_driver.c
  startup_stm32f42_43xxx.s
  ../common/arm/stm32/sdio_sd.c
  ../common/arm/loadboot.cpp
  )

add_definitions(-DBLUETOOTH)

set(SRC
  ${SRC}
  bluetooth.cpp
  io/frsky_firmware_update.cpp
  )

set(STM32LIB_SRC
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
  )

if(PYTHONINTERP_FOUND)
  add_custom_target(datacopy
    WORKING_DIRECTORY ${RADIO_DIRECTORY}/src
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/generate_datacopy.py datastructs.h -DPCBHORUS -DPCBX10 -DCPUARM -DCOLORLCD -DBACKUP -Itargets/horus > storage/datacopy.cpp
    DEPENDS ${RADIO_DIRECTORY}/src/datastructs.h ${RADIO_DIRECTORY}/util/generate_datacopy.py
  )
endif()
