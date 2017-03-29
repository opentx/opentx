set(ARCH AVR)
set(MCU atmega2560)
set(AVRDUDE_MCU m2560)
string(TOLOWER ${PCB} FLAVOUR)
set(EEPROM EEPROM_RLC)
add_definitions(-DEEPROM_VARIANT=0)
set(SDCARD NO)
set(GUI_DIR 128x64)
set(TARGET_DIR gruvin9x)
set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} 9x_bitmaps)
add_definitions(-DPCB${PCB} -DCPUM2560 -DROTARY_ENCODERS=2)
set(TARGET_SRC ${TARGET_SRC} board.cpp)
set(GUI_SRC
  ${GUI_SRC}
  model_inputs_mixes.cpp
  radio_diagkeys.cpp
  radio_diaganas.cpp
  )
set(FIRMWARE_SRC ${FIRMWARE_SRC} targets/9x/lcd_driver.cpp)
