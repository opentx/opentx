option(SHUTDOWN_CONFIRMATION "Shutdown confirmation" OFF)
option(LCD_DUAL_BUFFER "Dual LCD Buffer" OFF)
option(INTERNAL_MODULE_PPM "Support for PPM internal module hack" OFF)
option(INTERNAL_MODULE_PXX1 "Support for PXX1 internal module replacement" OFF)
option(PXX1 "PXX1 protocol support" ON)
option(PXX2 "PXX2 protocol support" OFF)
option(AUTOUPDATE "Auto update internal chips from SD" OFF)

if(PCB STREQUAL X9E)
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(STICKS "STANDARD" CACHE STRING "Sticks type (STANDARD/HORUS)")
  set(CPU_TYPE STM32F4)
  set(CPU_TYPE_FULL STM32F407xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f4_flash.ld)
  add_definitions(-DSTM32F40_41xxx)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x9e)
  set(FLAVOUR x9e)
  add_definitions(-DPCBX9E -DPCBX9)
  add_definitions(-DEEPROM_VARIANT=0x8000)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    top_lcd_driver.cpp
    startup_stm32f40_41xxx.s
    )
  set(AUX_SERIAL_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)
  if(STICKS STREQUAL HORUS)
    add_definitions(-DHORUS_STICKS)
  endif()
  set(GUI_DIR 212x64)
  set(NAVIGATION_TYPE x9d)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS NO)
elseif(PCB STREQUAL X9D+)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x9d)
  add_definitions(-DPCBX9DP -DPCBX9)
  add_definitions(-DEEPROM_VARIANT=0)
  set(GUI_DIR 212x64)
  set(NAVIGATION_TYPE x9d)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(PCBREV 2014 CACHE STRING "PCB Revision")
  add_definitions(-DPCBREV=${PCBREV})
  if (${PCBREV} STREQUAL 2019)
    set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
    set(CPU_TYPE STM32F4)
    set(CPU_TYPE_FULL STM32F407xE)  # for size report
    set(FIRMWARE_TARGET_SRC
      ${FIRMWARE_TARGET_SRC}
      startup_stm32f40_41xxx.s
      )
    set(LINKER_SCRIPT targets/taranis/stm32f4_flash.ld)
    add_definitions(-DSTM32F40_41xxx)
    add_definitions(-DSOFTWARE_VOLUME -DPWR_BUTTON_${PWR_BUTTON} -DHARDWARE_POWER_MANAGEMENT_UNIT)
    set(STATUS_LEDS YES)
    set(PXX2 ON)
    set(PXX_FREQUENCY "HIGH")
    set(FLAVOUR x9d+2019)
  else()
    set(CPU_TYPE STM32F2)
    set(CPU_TYPE_FULL STM32F205xE)  # for size report
    set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
    set(AUX_SERIAL_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)
    set(STATUS_LEDS NO)
    set(FLAVOUR x9d+)
  endif()
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
  set(NAVIGATION_TYPE x9d)
  set(BITMAPS_TARGET taranis_bitmaps)
  set(FONTS_TARGET taranis_fonts)
  set(LCD_DRIVER lcd_driver_aspi.cpp)
  set(AUX_SERIAL_DRIVER ../common/arm/stm32/aux_serial_driver.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS NO)
elseif(PCB STREQUAL X7)
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  if(PCBREV STREQUAL T12)
    set(LUA_EXPORT lua_export_t12)
    set(FLAVOUR t12)
    set(NAVIGATION_TYPE 9x)
    add_definitions(-DRADIO_T12)
    add_definitions(-DEEPROM_VARIANT=0x4001)
  else()
    set(LUA_EXPORT lua_export_x7)
    set(FLAVOUR x7)
    set(NAVIGATION_TYPE x7)
    add_definitions(-DRADIO_X7)
    add_definitions(-DEEPROM_VARIANT=0x4000)
  endif()
  add_definitions(-DPCBX7 -DSOFTWARE_VOLUME)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(GUI_DIR 128x64)
  set(BITMAPS_TARGET 9x_bitmaps)
  set(FONTS_TARGET 9x_fonts_1bit)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS YES)
elseif(PCB STREQUAL X9LITE)
  set(PXX_FREQUENCY "HIGH")
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_x9lite)
  set(FLAVOUR x9lite)
  add_definitions(-DBOARD_NAME="x9lite")
  add_definitions(-DPCBX9LITE -DPCBX9 -DSOFTWARE_VOLUME -DHARDWARE_POWER_MANAGEMENT_UNIT)
  add_definitions(-DEEPROM_VARIANT=0x0800)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(PXX2 ON)
  set(GUI_DIR 128x64)
  set(NAVIGATION_TYPE x7)
  set(BITMAPS_TARGET 9x_bitmaps)
  set(FONTS_TARGET 9x_fonts_1bit)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS YES)
elseif(PCB STREQUAL XLITE)
  set(PXX_FREQUENCY "HIGH" CACHE STRING "PXX frequency (LOW / HIGH)") # always use HIGH except on some prototype boards
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_xlite)
  set(FLAVOUR xlite)
  add_definitions(-DPCBXLITE -DSOFTWARE_VOLUME -DEXTERNAL_ANTENNA -DSTICKS_PWM)
  add_definitions(-DEEPROM_VARIANT=0x2000)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(PXX2 ON)
  set(GUI_DIR 128x64)
  set(NAVIGATION_TYPE xlite)
  set(BITMAPS_TARGET 9x_bitmaps)
  set(FONTS_TARGET 9x_fonts_1bit)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS YES)
elseif(PCB STREQUAL XLITES)
  set(PXX_FREQUENCY "HIGH")
  set(PWR_BUTTON "PRESS" CACHE STRING "Pwr button type (PRESS/SWITCH)")
  set(CPU_TYPE STM32F2)
  set(CPU_TYPE_FULL STM32F205xE)  # for size report
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_xlites)
  set(FLAVOUR xlites)
  add_definitions(-DPCBXLITES -DPCBXLITE -DSOFTWARE_VOLUME -DEXTERNAL_ANTENNA -DSTICKS_PWM -DHARDWARE_POWER_MANAGEMENT_UNIT)
  add_definitions(-DEEPROM_VARIANT=0x1000)
  add_definitions(-DPWR_BUTTON_${PWR_BUTTON})
  set(GUI_DIR 128x64)
  set(NAVIGATION_TYPE xlite)
  set(BITMAPS_TARGET 9x_bitmaps)
  set(FONTS_TARGET 9x_fonts_1bit)
  set(LCD_DRIVER lcd_driver_spi.cpp)
  set(GVAR_SCREEN model_gvars.cpp)
  set(STATUS_LEDS YES)
  set(PXX2 ON)
endif()

if(PCB STREQUAL XLITES)
  add_definitions(-DGYRO)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/lsm6ds_driver.cpp
    )
  set(SRC
    ${SRC}
    gyro.cpp
    )
endif()

if(PCB STREQUAL XLITE OR PCB STREQUAL XLITES OR PCB STREQUAL X9LITE OR (PCB STREQUAL X9D+ AND PCBREV STREQUAL 2019))
  add_definitions(-DINTERNAL_MODULE_SERIAL)
  if(PXX_FREQUENCY STREQUAL HIGH)
    add_definitions(-DPXX_FREQUENCY_HIGH)
  endif()
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/intmodule_serial_driver.cpp
    )
elseif(NOT (PCB STREQUAL X7 AND PCBREV STREQUAL T12))
  set(TARGET_SRC
    ${TARGET_SRC}
    intmodule_pulses_driver.cpp
    )
endif()

if(PCB STREQUAL X9E OR (PCB STREQUAL X9D+ AND PCBREV STREQUAL 2019) OR (PCB STREQUAL X7 AND NOT (PCBREV STREQUAL T12)) OR PCB STREQUAL X9LITE)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/rotary_encoder_driver.cpp
    )
endif()

if(PCB STREQUAL X9E OR PCB STREQUAL X7 OR PCB STREQUAL XLITE OR PCB STREQUAL XLITES OR (PCB STREQUAL X9D+ AND PCBREV STREQUAL 2019))
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

if(AUTOUPDATE)
  add_definitions(-DAUTOUPDATE)
endif()

set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM EEPROM_RLC)
set(TARGET_DIR taranis)
set(PPM_LIMITS_SYMETRICAL YES)

if(SPLASH STREQUAL OFF)
  message(FATAL_ERROR "Option SPLASH=OFF not supported on Taranis")
endif()

add_definitions(-DPCBTARANIS)
add_definitions(-DAUDIO -DVOICE -DRTCLOCK)

set(SRC
  ${SRC}
  io/frsky_firmware_update.cpp
  )

set(GUI_SRC
  ${GUI_SRC}
  model_input_edit.cpp
  model_mix_edit.cpp
  model_display.cpp
  radio_diagkeys.cpp
  radio_diaganas.cpp
  view_channels.cpp
  view_telemetry.cpp
  view_about.cpp
  bmp.cpp
  ../screenshot.cpp
  )

if(STATUS_LEDS)
  set(LED_DRIVER led_driver.cpp)
endif()

set(SRC
  ${SRC}
  sbus.cpp
  )

set(TARGET_SRC
  ${TARGET_SRC}
  ${LED_DRIVER}
  backlight_driver.cpp
  extmodule_driver.cpp
  trainer_driver.cpp
  ../common/arm/stm32/timers_driver.cpp
  ../common/arm/stm32/audio_dac_driver.cpp
  ../common/arm/stm32/adc_driver.cpp
  ../common/arm/stm32/heartbeat_driver.cpp
  )

if(PCB STREQUAL XLITE OR PCB STREQUAL XLITES)
  set(TARGET_SRC
    ${TARGET_SRC}
    ../common/arm/stm32/sticks_pwm_driver.cpp
    )
endif()

set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ${LCD_DRIVER}
  board.cpp
  i2c_driver.cpp
  pwr_driver.cpp
  ../common/arm/loadboot.cpp
  )

if(LCD_DUAL_BUFFER)
  add_definitions(-DLCD_DUAL_BUFFER)
endif()

if(INTERNAL_MODULE_PPM)
  add_definitions(-DINTERNAL_MODULE_PPM)
endif()

if(INTERNAL_MODULE_PXX1)
  add_definitions(-DINTERNAL_MODULE_PXX1)
endif()

set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${BITMAPS_TARGET})
