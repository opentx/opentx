option(SHUTDOWN_CONFIRMATION "Shutdown confirmation" OFF)
option(LCD_DUAL_BUFFER "Dual LCD Buffer" OFF)
option(TARANIS_INTERNAL_PPM "Taranis internal module hack to output PPM" OFF)

if(PCB STREQUAL X9E)
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(STICKS "STANDARD" CACHE STRING "Sticks type (STANDARD/HORUS)")
  set(CPU_TYPE STM32F4)
  set(CPU_TYPE_FULL STM32F407xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f4_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x9e)
  set(FLAVOUR x9e)
  add_definitions(-DSTM32F40_41xxx -DPCBX9E -DPCBX9)
  add_definitions(-DEEPROM_VARIANT=0x8000)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    top_lcd_driver.cpp
    startup_stm32f40_41xxx.s
    )
  set(SERIAL2_DRIVER ../common/arm/stm32/serial2_driver.cpp)
  if(STICKS STREQUAL HORUS)
    add_definitions(-DHORUS_STICKS)
  endif()
  set(GUI_DIR 212x64)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
elseif(PCB STREQUAL X9D+)
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x9d)
  set(FLAVOUR x9d+)
  add_definitions(-DPCBX9DP -DPCBX9)
  add_definitions(-DEEPROM_VARIANT=0)
  set(GUI_DIR 212x64)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(SERIAL2_DRIVER ../common/arm/stm32/serial2_driver.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
elseif(PCB STREQUAL X9D)
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  option(HAPTIC "Haptic support" OFF)
  set(LUA_EXPORT lua_export_x9d)
  set(FLAVOUR x9d)
  add_definitions(-DPCBX9D -DPCBX9)
  add_definitions(-DEEPROM_VARIANT=0)
  set(GUI_DIR 212x64)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_aspi.cpp)
  set(SERIAL2_DRIVER ../common/arm/stm32/serial2_driver.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
elseif(PCB STREQUAL X7)
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x7)
  set(FLAVOUR x7)
  add_definitions(-DPCBX7 -DSOFTWARE_VOLUME)
  add_definitions(-DEEPROM_VARIANT=0x4000)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(GUI_DIR 128x64)
  set(BITMAPS_TARGET 9x_bitmaps)
  set(FONTS_TARGET 9x_fonts_1bit)
  set(LCD_DRIVER lcd_driver_spi.cpp)
endif()

if(PCB STREQUAL X9E OR PCB STREQUAL X7)
  add_definitions(-DBLUETOOTH)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/bluetooth_driver.cpp
    )
  set(SRC
    ${SRC}
    bluetooth.cpp
    )
endif()

set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM EEPROM_RLC)
set(TARGET_DIR taranis)
set(PPM_LIMITS_SYMETRICAL YES)

if(SPLASH STREQUAL OFF)
  message(FATAL_ERROR "Option SPLASH=OFF not supported on Taranis")
endif()

add_definitions(-DPCBTARANIS -DPPM_PIN_TIMER)
add_definitions(-DAUDIO -DVOICE -DRTCLOCK)

set(GUI_SRC ${GUI_SRC}
  model_input_edit.cpp
  model_mix_edit.cpp
  model_display.cpp
  radio_diagkeys.cpp
  radio_diaganas.cpp
  radio_hardware.cpp
  view_channels.cpp
  view_telemetry.cpp
  view_about.cpp
  bmp.cpp
  ../screenshot.cpp
  )

if(PCB STREQUAL X7)
  set(LED_DRIVER led_driver.cpp)
endif()

set(TARGET_SRC
  ${TARGET_SRC}
  board.cpp
  ${LED_DRIVER}
  backlight_driver.cpp
  extmodule_driver.cpp
  ../common/arm/stm32/audio_dac_driver.cpp
  ../common/arm/stm32/adc_driver.cpp
  )

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
  i2c_driver.cpp
  pwr_driver.cpp
  ../common/arm/loadboot.cpp
  )

if(LCD_DUAL_BUFFER)
  add_definitions(-DLCD_DUAL_BUFFER)
endif()

if(TARANIS_INTERNAL_PPM)
  add_definitions(-DTARANIS_INTERNAL_PPM)
endif()

set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${BITMAPS_TARGET})
