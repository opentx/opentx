set(PCBREV "13" CACHE STRING "PCB Revision")
set(INTERNAL_GPS_BAUDRATE "9600" CACHE STRING "Baud rate for internal GPS")
option(DISK_CACHE "Enable SD card disk cache" YES)
if(${PCBREV} GREATER 10)
  option(INTERNAL_GPS "Internal GPS installed" YES)
else()
  option(INTERNAL_GPS "Internal GPS installed" NO)
  if(NOT INTERNAL_GPS)
    message("Internal GPS is optional, use INTERNAL_GPS=YES option to enable it")
  endif()
endif()
option(BLUETOOTH_CLI_PASSTHROUGH "Enable direct communicaton with Bluetooth module from CLI" NO)

set(CPU_TYPE STM32F4)
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM SDCARD)
set(HAPTIC YES)
set(GUI_DIR 480x272)
set(TARGET_DIR horus)
set(LINKER_SCRIPT targets/horus/stm32f4_flash.ld)
set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} horus_bitmaps)
set(LUA_EXPORT lua_export_horus)
set(FLAVOUR horus)
set(RAMBACKUP YES)
set(LUA YES)

add_definitions(-DPCBHORUS -DPCBREV=${PCBREV} -DSTM32F429_439xx -DSDRAM -DCOLORLCD -DPPM_PIN_UART -DPPM_PIN_TIMER)
add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DGPS_USART_BAUDRATE=${INTERNAL_GPS_BAUDRATE})

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
  ${THEMES_SRC}
  ${LAYOUTS_SRC}
  ${WIDGETS_SRC}
  )
if(DISK_CACHE)
  set(SRC ${SRC} disk_cache.cpp)
  add_definitions(-DDISK_CACHE)
endif()
if(INTERNAL_GPS)
  set(SRC ${SRC} gps.cpp)
  add_definitions(-DINTERNAL_GPS)
  message("Internal GPS enabled")
endif()
set(SERIAL2_DRIVER ../common/arm/stm32/serial2_driver.cpp)
set(GVAR_SCREEN model_gvars.cpp)
set(TARGET_SRC
  ${TARGET_SRC}
  board.cpp
  led_driver.cpp
  extmodule_driver.cpp
  gps_driver.cpp
  )
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  sdio_sd.c
  lcd_driver.cpp
  backlight_driver.cpp
  bluetooth_driver.cpp
  pwr_driver.cpp
  sdram_driver.c
  startup_stm32f40_41xxx.s
  )
if(BLUETOOTH_CLI_PASSTHROUGH)
  add_definitions(-DBLUETOOTH_CLI_PASSTHROUGH)
  message("Bluetooth pass-trough enabled")
endif()
set(STM32LIB_SRC
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
  )
add_custom_target(datacopy
  WORKING_DIRECTORY ${RADIO_DIRECTORY}/src
  COMMAND python ${RADIO_DIRECTORY}/util/generate_datacopy.py datastructs.h -DPCBHORUS -DCPUARM -DCOLORLCD -DCPUARM -DBACKUP > storage/datacopy.cpp
  DEPENDS ${RADIO_DIRECTORY}/src/datastructs.h ${RADIO_DIRECTORY}/util/generate_datacopy.py
)
