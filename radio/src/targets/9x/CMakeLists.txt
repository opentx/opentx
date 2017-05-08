option(SP22 "SmartieParts 2.2 Backlight support" OFF)
option(PWM_BACKLIGHT "PWM Backlight" OFF)
set(ARCH AVR)
set(MCU atmega64)
set(AVRDUDE_MCU m64)
string(TOLOWER ${PCB} FLAVOUR)
set(EEPROM_VARIANT_NEEDED ON)
set(EEPROM EEPROM_RLC)
set(SDCARD NO)
set(GUI_DIR 128x64)
set(TARGET_DIR 9x)
set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} 9x_bitmaps)
add_definitions(-DPCBSTD -DPCB${PCB} -DCPUM64)
set(TARGET_SRC ${TARGET_SRC} board.cpp)
set(GUI_SRC
  ${GUI_SRC}
  model_inputs_mixes.cpp
  radio_diagkeys.cpp
  radio_diaganas.cpp
  )
if(SP22)
  add_definitions(-DSP22)
endif()
if(PWM_BACKLIGHT)
  add_definitions(-DPWM_BACKLIGHT)
endif()
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  lcd_driver.cpp
  )

