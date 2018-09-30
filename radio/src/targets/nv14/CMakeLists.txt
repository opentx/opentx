option(DISK_CACHE "Enable SD card disk cache" YES)
option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" YES)
set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")

set(CPU_TYPE STM32F4)
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM SDCARD)
set(HAPTIC YES)
set(GUI_DIR 320x480)
# set(NAVIGATION_TYPE touch)
set(TARGET_DIR nv14)

if(BOOTLOADER)
  set(LINKER_SCRIPT targets/nv14/stm32f4_flash_bootloader.ld)
else()
  set(LINKER_SCRIPT targets/nv14/stm32f4_flash.ld)
endif()

set(RAMBACKUP YES)
set(PPM_LIMITS_SYMETRICAL YES)
# for size report script
set(CPU_TYPE_FULL STM32F429BIT6)
set(SIZE_TARGET_MEM_DEFINE "MEM_SIZE_SDRAM2=8192")
option(USB_SERIAL "Enable USB serial (CDC)" ON)

set(RF_BAUD_RATE 921600 230400 115200 57600 38400 19200 9600 4800 2400 1200)
set(PCB_RF_BAUD 921600 CACHE STRING "INTERNAL_MODULE_BAUDRATE: ${RF_BAUD_RATE}")
set_property(CACHE PCB_RF_BAUD PROPERTY STRINGS ${RF_BAUD_RATE})

set(FLAVOUR NV14)
add_definitions(-DPCBNV14 -DPCBFLYSKY -DINTERNAL_MODULE_BAUDRATE=${PCB_RF_BAUD})
#add_definitions(-DSOFTWARE_VOLUME)
set(TARGET_SRC
  ${TARGET_SRC}
  #../common/arm/stm32/audio_dac_driver.cpp
  audio_spi_driver.cpp
  battery_driver.cpp
  adc_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  hallStick_driver.cpp
  )

set(BITMAPS_TARGET nv14_bitmaps)
set(FONTS_TARGET x12_fonts)
set(LCD_DRIVER lcd_driver.cpp)
set(LUA_EXPORT lua_export_x10)
set(TOUCH_DRIVER touch_driver.cpp)

set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${BITMAPS_TARGET})

add_definitions(-DSTM32F429_439xx -DSDRAM -DCOLORLCD -DTOUCH_UI)
add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DGPS_USART_BAUDRATE=${INTERNAL_GPS_BAUDRATE})
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

include_directories(${RADIO_SRC_DIRECTORY}/fonts/colorlcd gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

file(GLOB THEMES_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/320x480 ${RADIO_SRC_DIRECTORY}/gui/320x480/themes/*.cpp)
file(GLOB LAYOUTS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/320x480 ${RADIO_SRC_DIRECTORY}/gui/320x480/layouts/*.cpp)
file(GLOB WIDGETS_SRC RELATIVE ${RADIO_SRC_DIRECTORY}/gui/320x480 ${RADIO_SRC_DIRECTORY}/gui/320x480/widgets/*.cpp)
set(GUI_SRC
  ${GUI_SRC}
  bitmapbuffer.cpp
  draw_functions.cpp
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
  shutdown_animation.cpp
  theme.cpp
  topbar.cpp
  layout.cpp
  widget.cpp
  ${THEMES_SRC}
  ${LAYOUTS_SRC}
  ${WIDGETS_SRC}
  rle.cpp
  )
if(DISK_CACHE)
  set(SRC ${SRC} disk_cache.cpp)
  add_definitions(-DDISK_CACHE)
endif()

set(SERIAL2_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)

set(GVAR_SCREEN model_gvars.cpp)

set(TARGET_SRC
  ${TARGET_SRC}
  board.cpp
  extmodule_driver.cpp
  trainer_driver.cpp
  intmodule_serial_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
  ${TOUCH_DRIVER}
  backlight_driver.cpp
  pwr_driver.cpp
  sdram_driver.c
  startup_stm32f42_43xxx.s
  ../common/arm/stm32/sdio_sd.c
  )

if(BOOTLOADER)
  set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ../common/arm/loadboot.cpp
  )
endif()

set(PULSES_SRC
  ${PULSES_SRC}
  flysky_arm.cpp
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
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/generate_datacopy.py datastructs.h -DPCBFLYSKY -DPCBNV14 -DCPUARM -DCOLORLCD -DBACKUP > storage/datacopy.cpp
    DEPENDS ${RADIO_DIRECTORY}/src/datastructs.h ${RADIO_DIRECTORY}/util/generate_datacopy.py
  )
endif()

set(LUA_EXPORT lua_export_nv14)
