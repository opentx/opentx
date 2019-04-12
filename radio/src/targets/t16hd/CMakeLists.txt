option(DISK_CACHE "Enable SD card disk cache" YES)
option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" YES)
set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")

set(CPU_TYPE STM32F4)
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM SDCARD)
set(HAPTIC YES)
set(GUI_DIR 800x480)
set(TARGET_DIR t16hd)
set(LINKER_SCRIPT targets/t16hd/stm32f4_flash.ld)
set(RAMBACKUP YES)
set(LUA YES)
set(PPM_LIMITS_SYMETRICAL YES)
set(MULTIMODULE YES)

# for size report script
set(CPU_TYPE_FULL STM32F429xI)
set(SIZE_TARGET_MEM_DEFINE "MEM_SIZE_SDRAM2=8192")

if(SPLASH STREQUAL OFF)
  message(FATAL_ERROR "Option SPLASH=OFF not supported on Horus")
endif()


set(FLAVOUR t16hd)
add_definitions(-DPCBT16HD)
add_definitions(-DSOFTWARE_VOLUME)
set(TARGET_SRC
${TARGET_SRC}
../common/arm/stm32/audio_dac_driver.cpp
../common/arm/stm32/adc_driver.cpp
../common/arm/stm32/sticks_pwm_driver.cpp
)

set(BITMAPS_TARGET t16hd_bitmaps)
set(FONTS_TARGET t16hd_fonts)
set(LCD_DRIVER lcd_driver.cpp)
set(LUA_EXPORT lua_export_t16hd)


set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${BITMAPS_TARGET})

add_definitions(-DPCBHORUS -DSTM32F429_439xx -DSDRAM -DCOLORLCD)
add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DGPS_USART_BAUDRATE=${INTERNAL_GPS_BAUDRATE})
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

include_directories(${RADIO_SRC_DIRECTORY}/fonts/800x480 gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

file(GLOB THEMES_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/800x480 ${RADIO_SRC_DIRECTORY}/gui/800x480/themes/*.cpp)
file(GLOB LAYOUTS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/800x480 ${RADIO_SRC_DIRECTORY}/gui/800x480/layouts/*.cpp)
file(GLOB WIDGETS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/800x480 ${RADIO_SRC_DIRECTORY}/gui/800x480/widgets/*.cpp)
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
  message("Horus: Internal GPS enabled")
endif()

set(SERIAL2_DRIVER ../common/arm/stm32/serial2_driver.cpp)

set(GVAR_SCREEN model_gvars.cpp)

set(TARGET_SRC
  ${TARGET_SRC}
  board.cpp
  led_driver.cpp
  extmodule_driver.cpp
  trainer_driver.cpp
  ../common/arm/stm32/intmodule_serial_driver.cpp
  ../common/arm/stm32/bluetooth_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
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
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/generate_datacopy.py datastructs.h -DPCBHORUS -DPCBT16HD -DCPUARM -DCOLORLCD -DBACKUP -DVIRTUAL_INPUTS -Itargets/t16hd > storage/datacopy.cpp
    DEPENDS ${RADIO_DIRECTORY}/src/datastructs.h ${RADIO_DIRECTORY}/util/generate_datacopy.py
  )
endif()
