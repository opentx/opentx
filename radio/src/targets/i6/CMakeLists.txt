option(STICKS "Sticks type" STANDARD)
set(CPU_TYPE MKL16Z64)
set(LINKER_SCRIPT targets/i6/startup/stm32f4_flash.ld)
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  #top_lcd_driver.cpp
  #bluetooth_driver.cpp
   startup_MKL16Z4.s
  )
#set(HSE_VALUE 12000000)
#set(SDCARD YES)
#set(EEPROM EEPROM_RLC)
set(GUI_DIR i6)
set(TARGET_DIR i6)
set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} i6_bitmaps)
set(VIRTUAL_INPUTS YES)
add_definitions(-DPCBI6 -DPPM_PIN_TIMER)
#add_definitions(-DAUDIO -DRTCLOCK)
#add_definitions(-DVIRTUALINPUTS -DLUAINPUTS -DXCURVES -DVARIO)
set(GUI_SRC ${GUI_SRC}
  #bmp.cpp
  #menu_model_inputs.cpp
  #menu_model_mixes.cpp
  #menu_model_display.cpp
  #menu_general_diagkeys.cpp
  #menu_general_diaganas.cpp
  #menu_general_hardware.cpp
  #view_channels.cpp
  #view_telemetry.cpp
  #view_text.cpp
  #view_about.cpp
  )
set(TARGET_SRC
  ${TARGET_SRC}
  board_i6.cpp
  #extmodule_driver.cpp
  #rtc_driver.cpp
  )
set(FIRMWARE_SRC
  ${FIRMWARE_SRC}
  )
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  #lcd_driver.cpp
  #delays.c
  #i2c_driver.cpp
  #usb_driver.c
  #pwr_driver.c
  #configure_pins.cpp
  #flash_driver.cpp
  #aspi.c
  )
