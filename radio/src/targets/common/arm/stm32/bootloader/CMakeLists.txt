# TODO if the files order is different (these 2 SRC sections exchanged), the bootloader hangs for 20s in USB init. Why?

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if(PCB STREQUAL X12S OR PCB STREQUAL X10)
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../f4/system_stm32f4xx.c
    ../../../../../targets/${TARGET_DIR}/startup_stm32f42_43xxx.s
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/misc.c
    )
elseif(PCB STREQUAL X9E)
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../f4/system_stm32f4xx.c
    ../../../../../targets/${TARGET_DIR}/startup_stm32f40_41xxx.s
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
    ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/misc.c
    )
else()
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../f2/system_stm32f2xx.c
    ../../../../../${STM32LIB_DIR}/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
    ../../../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
    ../../../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
    ../../../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
    ../../../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_i2c.c
    ../../../../../${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/src/misc.c
    )
endif()

set(BOOTLOADER_SRC
  ${BOOTLOADER_SRC}
  ../../../../../gui/${GUI_DIR}/lcd.cpp
  ../../../../../gui/${GUI_DIR}/fonts.cpp
  ../../../../../keys.cpp
  ../../../../../strhelpers.cpp
  ../../../../../stamp.cpp
  ../../../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_core.c
  ../../../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_dcd.c
  ../../../../../${STM32USB_DIR}/STM32_USB_OTG_Driver/src/usb_dcd_int.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_core.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Core/src/usbd_req.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
  ../../../../../${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
  ../../../../../${FATFS_DIR}/ff.c
  ../../../../../${FATFS_DIR}/option/ccsbcs.c
  ../../../../../targets/${TARGET_DIR}/${LCD_DRIVER}
  ../../../../../targets/${TARGET_DIR}/backlight_driver.cpp
  ../../../../../targets/${TARGET_DIR}/keys_driver.cpp
  ../../../../../targets/${TARGET_DIR}/diskio.cpp
  ../../../../../targets/${TARGET_DIR}/pwr_driver.cpp
  ../../../../../targets/${TARGET_DIR}/bootloader/boot_menu.cpp
  ../usbd_usr.cpp
  ../usbd_storage_msd.cpp
  ../delays.cpp
  ../usbd_desc.c
  ../usb_bsp.c
  ../usb_driver.cpp
  ../flash_driver.cpp
  init.c
  boot.cpp
  bin_files.cpp
  )

if(NOT (PCB STREQUAL X10 OR PCB STREQUAL X12S))
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../../../../../targets/${TARGET_DIR}/i2c_driver.cpp
    )

  remove_definitions(-DDEBUG)

else()
  set(BOOTLOADER_SRC
    ${BOOTLOADER_SRC}
    ../../../../../targets/${TARGET_DIR}/sdram_driver.c
    ../../../../../targets/common/arm/stm32/sdio_sd.c
    ../../../../../targets/${TARGET_DIR}/haptic_driver.cpp
    ../../../../../gui/${GUI_DIR}/bitmapbuffer.cpp
    ../../../../../syscalls.c
    )

  if(DEBUG)
    set(BOOTLOADER_SRC
      ${BOOTLOADER_SRC}
      ../../../../../${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
      ../../../../../serial.cpp
      ../serial2_driver.cpp
      )
  endif()
endif()

remove_definitions(-DDISK_CACHE)
remove_definitions(-DLUA)
remove_definitions(-DCLI)
remove_definitions(-DUSB_SERIAL)
add_definitions(-DBOOT)

set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU} -mthumb -nostartfiles -lm -T${RADIO_SRC_DIRECTORY}/targets/${TARGET_DIR}/stm32_ramboot.ld -Wl,-Map=bootloader.map,--cref,--no-warn-mismatch,--gc-sections")
# use smaller nano libc
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")


add_executable(bootloader ${BOOTLOADER_SRC})
add_dependencies(bootloader ${BITMAPS_TARGET} firmware_translations)

add_custom_command(
  TARGET bootloader POST_BUILD
  COMMAND arm-none-eabi-objcopy -O binary bootloader.elf bootloader.bin
)
if(PYTHONINTERP_FOUND)
  add_custom_command(
    TARGET bootloader POST_BUILD
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/bin2lbm.py bootloader.bin bootloader.lbm
  )
endif()

PrintTargetReport("bootloader")
