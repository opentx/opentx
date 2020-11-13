option(DISK_CACHE "Enable SD card disk cache" ON)
option(UNEXPECTED_SHUTDOWN "Enable the Unexpected Shutdown screen" ON)
option(FLYSKY_HALL_STICKS "sticks for FlySky hall sticks" ON)
option(FLYSKY_HALL_STICKS_REVERSE "Reverse sticks for FlySky hall sticks" OFF)
option(MULTIMODULE "DIY Multiprotocol TX Module (https://github.com/pascallanger/DIY-Multiprotocol-TX-Module)" ON)
option(AFHDS2 "Support for AFHDS2" ON)

set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
set(CPU_TYPE STM32F4)
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(STORAGE SDCARD)
set(STORAGE_FORMAT BIN)
set(HAPTIC YES)
set(GUI_DIR colorlcd)
set(BITMAPS_DIR 480x272)
# set(NAVIGATION_TYPE touch)
set(TARGET_DIR nv14)
add_definitions(-DHARDWARE_TRAINER_JACK)

if(BOOTLOADER)
  set(LINKER_SCRIPT targets/nv14/stm32f4_flash_bootloader.ld)
else()
  set(LINKER_SCRIPT targets/nv14/stm32f4_flash.ld)
endif()

set(RAMBACKUP YES)
set(PPM_LIMITS_SYMETRICAL YES)
# for size report script
set(CPU_TYPE_FULL STM32F429xI)
set(SIZE_TARGET_MEM_DEFINE "MEM_SIZE_SDRAM2=8192")
option(USB_SERIAL "Enable USB serial (CDC)" ON)

set(RF_BAUD_RATE 921600 230400 115200 57600 38400 19200 9600 4800 2400 1200)
set(PCB_RF_BAUD 921600 CACHE STRING "INTERNAL_MODULE_BAUDRATE: ${RF_BAUD_RATE}")
set_property(CACHE PCB_RF_BAUD PROPERTY STRINGS ${RF_BAUD_RATE})

set(FLAVOUR nv14)
add_definitions(-DPCBNV14 -DPCBFLYSKY -DINTERNAL_MODULE_BAUDRATE=${PCB_RF_BAUD})
add_definitions(-DBATTERY_CHARGE)  # -DSOFTWARE_VOLUME

set(TARGET_SRC
  ${TARGET_SRC}
  audio_spi_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  x12s_adc_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  battery_driver.cpp
  hallStick_driver.cpp
  )

set(BITMAPS_TARGET nv14_bitmaps)
set(FONTS_TARGET x12_fonts)
set(LCD_DRIVER lcd_driver.cpp)
set(LUA_EXPORT lua_export_x10)
set(TOUCH_DRIVER touch_driver.cpp)
set(HARDWARE_TOUCH YES)

set(RADIO_DEPENDENCIES ${RADIO_DEPENDENCIES} ${BITMAPS_TARGET} truetype_fonts)

set(HARDWARE_TOUCH ON)
set(SOFTWARE_KEYBOARD ON)

add_definitions(-DSTM32F429_439xx -DSDRAM -DCOLORLCD -DLIBOPENUI -DHARDWARE_TOUCH -DSOFTWARE_KEYBOARD)
add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DGPS_USART_BAUDRATE=${INTERNAL_GPS_BAUDRATE})
add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
add_definitions(-DCROSSFIRE_NATIVE)
add_definitions(-DFLYSKY_HALL_STICKS_REVERSE)
add_definitions(-DHALL_STICKS)
#add_definitions(-DFLYSKY_AUTO_POWER_DOWN)

if(STICKS_DEAD_ZONE)
  add_definitions(-DSTICK_DEAD_ZONE)
endif()

if(NOT UNEXPECTED_SHUTDOWN)
  add_definitions(-DNO_UNEXPECTED_SHUTDOWN)
endif()

set(AFHDS2 ON)
set(AFHDS3 ON)

include_directories(${RADIO_SRC_DIR}/fonts/colorlcd gui/${GUI_DIR} gui/${GUI_DIR}/layouts)

file(GLOB THEMES_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/themes/*.cpp)
file(GLOB LAYOUTS_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/layouts/*.cpp)
file(GLOB WIDGETS_SRC RELATIVE ${RADIO_SRC_DIR}/gui/colorlcd ${RADIO_SRC_DIR}/gui/colorlcd/widgets/*.cpp)

if(DISK_CACHE)
  set(SRC ${SRC} disk_cache.cpp)
  add_definitions(-DDISK_CACHE)
endif()

set(AUX_SERIAL_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)

set(SRC
  ${SRC}
  io/frsky_firmware_update.cpp
  )

set(GVAR_SCREEN model_gvars.cpp)

set(TARGET_SRC
  ${TARGET_SRC}
  extmodule_driver.cpp
  trainer_driver.cpp
  ../common/arm/stm32/intmodule_serial_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
  ${TOUCH_DRIVER}
  board.cpp
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

set(SRC
  ${SRC}
  io/frsky_firmware_update.cpp
  io/multi_firmware_update.cpp
  )

set(STM32LIB_SRC
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
  STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
  )

if(PYTHONINTERP_FOUND)
  add_custom_target(datacopy
    WORKING_DIRECTORY ${RADIO_DIRECTORY}/src
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/generate_datacopy.py datastructs.h -DPCBFLYSKY -DPCBNV14 -DCPUARM -DCOLORLCD -DBACKUP > storage/datacopy.cpp
    DEPENDS ${RADIO_DIRECTORY}/src/datastructs.h ${RADIO_DIRECTORY}/util/generate_datacopy.py
  )
endif()

set(LUA_EXPORT lua_export_nv14)
