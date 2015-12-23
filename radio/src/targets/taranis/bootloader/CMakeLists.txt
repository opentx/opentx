# TODO if the files order is different (these 2 SRC sections exchanged), the bootloader hangs for 20s in USB init. Why?

if(PCBREV STREQUAL REV9E)
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../system_stm32f4xx.c
    ../../../${STM32LIB_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
    ../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
    ../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
    ../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
    ../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
    ../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/misc.c
    )
else()
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../system_stm32f2xx.c
    ../../../${STM32LIB_DIR}/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
    ../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
    ../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
    ../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
    ../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_i2c.c
    ../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/misc.c
    )
endif()

set(BOOTLOADER_SRC
  ${BOOTLOADER_SRC}
  ../../../gui/taranis/lcd.cpp
  ../../../gui/taranis/fonts.cpp
  ../../../keys.cpp
  ../../../strhelpers.cpp
  ../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_core.c
  ../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_dcd.c
  ../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_dcd_int.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_core.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_req.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
  ../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
  ../../../${FATFS_DIR}/ff.c
  ../../../${FATFS_DIR}/option/ccsbcs.c
  ../lcd_driver.cpp
  ../configure_pins.cpp
  ../keys_driver.cpp
  ../i2c_driver.cpp
  ../flash_driver.cpp
  ../diskio.cpp
  ../usbd_usr.cpp
  ../usbd_storage_msd.cpp
  ../delays.c
  ../usbd_desc.c
  ../aspi.c
  ../usb_bsp.c
  ../usb_driver.c
  ../pwr_driver.c
  init.c
  boot.cpp
  )

remove_definitions(-DUSB_JOYSTICK -DUSB_SERIAL -DDEBUG)
add_definitions(-DBOOT -DUSB_MASS_STORAGE)

set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU} -mthumb -nostartfiles -lm -T${RADIO_SRC_DIRECTORY}/targets/taranis/stm32_ramboot.ld -Wl,-Map=bootloader.map,--cref,--no-warn-mismatch,--gc-sections")

add_executable(bootloader.elf ${BOOTLOADER_SRC})
add_dependencies(bootloader.elf taranis_fonts firmware_translations)
