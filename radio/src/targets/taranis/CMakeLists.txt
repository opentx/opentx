option(PCBREV "PCB Revision" REVPLUS)
option(SHUTDOWN_CONFIRMATION "Shutdown confirmation" OFF)
option(LCD_DUAL_BUFFER "Dual LCD Buffer" OFF)
option(TARANIS_INTERNAL_PPM "Taranis internal module hack to output PPM" OFF)
add_definitions(-DUSB_NAME="FrSky Taranis")
if(PCBREV STREQUAL REV9E)
  option(STICKS "Sticks type" STANDARD)
  set(CPU_TYPE STM32F4)
  set(LINKER_SCRIPT targets/taranis/stm32f4_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_taranis_x9e)
  set(FLAVOUR taranisx9e)
  add_definitions(-DSTM32F40_41xxx -DREVPLUS -DREV9E)
  add_definitions(-DEEPROM_VARIANT=32768)
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    top_lcd_driver.cpp
    bluetooth_driver.cpp
    startup_stm32f40_41xxx.s
    )
  if(STICKS STREQUAL HORUS)
    add_definitions(-DHORUS_STICKS)
  endif()
elseif(PCBREV STREQUAL REVPLUS)
  set(CPU_TYPE STM32F2)
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC YES)
  set(LUA_EXPORT lua_export_taranis)
  set(FLAVOUR taranisplus)
  add_definitions(-DREVPLUS)
  add_definitions(-DEEPROM_VARIANT=0)
else()
  set(CPU_TYPE STM32F2)
  set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
  set(HAPTIC NO)
  set(LUA_EXPORT lua_export_taranis)
  set(FLAVOUR taranis)
  add_definitions(-DEEPROM_VARIANT=0)
endif()
set(HSE_VALUE 12000000)
set(SDCARD YES)
set(EEPROM EEPROM_RLC)
set(GUI_DIR taranis)
set(TARGET_DIR taranis)
set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} taranis_bitmaps)
set(VIRTUAL_INPUTS YES)
add_definitions(-DPCBTARANIS -DPPM_PIN_TIMER)
add_definitions(-DAUDIO -DVOICE -DRTCLOCK)
add_definitions(-DVIRTUALINPUTS -DLUAINPUTS -DXCURVES -DVARIO)
set(GUI_SRC ${GUI_SRC} bmp.cpp menu_model_inputs.cpp menu_model_mixes.cpp menu_model_display.cpp menu_general_diagkeys.cpp menu_general_diaganas.cpp menu_general_hardware.cpp view_channels.cpp view_telemetry.cpp view_text.cpp view_about.cpp)
set(TARGET_SRC
  ${TARGET_SRC}
  board.cpp
  extmodule_driver.cpp
  rtc_driver.cpp
  )
set(FIRMWARE_SRC
  ${FIRMWARE_SRC}
  loadboot.cpp
  )
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  lcd_driver.cpp
  delays.c
  i2c_driver.cpp
  pwr_driver.c
  configure_pins.cpp
  flash_driver.cpp
  aspi.c
  )
if(LCD_DUAL_BUFFER)
  add_definitions(-DLCD_DUAL_BUFFER)
endif()
if(TARANIS_INTERNAL_PPM)
  add_definitions(-DTARANIS_INTERNAL_PPM)
endif()
if(MIXERS_MONITOR)
  add_definitions(-DMIXERS_MONITOR)
endif()

