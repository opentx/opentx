set(LUA "NO" CACHE STRING "Lua scripts (YES/NO/NO_MODEL_SCRIPTS)")
set(USB "JOYSTICK" CACHE STRING "USB option (JOYSTICK/MASSSTORAGE/SERIAL)")
set(ARCH ARM)
set(STM32USB_DIR ${THIRDPARTY_DIR}/STM32_USB-Host-Device_Lib_V2.2.0/Libraries)
add_definitions(-DSTM32 -DLUA_INPUTS -DVARIO -DSBUS -DCROSSFIRE)
include_directories(${RADIO_SRC_DIRECTORY}/targets/common/arm/stm32)
include_directories(${STM32USB_DIR}/STM32_USB_OTG_Driver/inc)
include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Core/inc)
include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/inc)
include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/hid/inc)
include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/cdc/inc)
include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Core/inc/)
include_directories(${STM32USB_DIR}/STM32_USB_OTG_Driver/inc)
set(STM32USB_SRC
  STM32_USB_OTG_Driver/src/usb_core.c
  STM32_USB_OTG_Driver/src/usb_dcd.c
  STM32_USB_OTG_Driver/src/usb_dcd_int.c
  STM32_USB_Device_Library/Core/src/usbd_core.c
  STM32_USB_Device_Library/Core/src/usbd_ioreq.c
  STM32_USB_Device_Library/Core/src/usbd_req.c
  )
set(TARGET_SRC
  ${TARGET_SRC}
  ../common/arm/stm32/rtc_driver.cpp
  ../common/arm/stm32/cpu_id.cpp
  ${SERIAL2_DRIVER}
  )
set(FIRMWARE_TARGET_SRC
  ${FIRMWARE_TARGET_SRC}
  ../common/arm/stm32/delays.cpp
  ../common/arm/stm32/usb_bsp.c
  ../common/arm/stm32/usbd_desc.c
  ../common/arm/stm32/usbd_usr.cpp
  ../common/arm/stm32/usb_driver.c
  )
if(USB STREQUAL SERIAL)
  add_definitions(-DUSB_SERIAL)
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    ../common/arm/stm32/usbd_cdc.cpp
    )
  set(STM32USB_SRC
    ${STM32USB_SRC}
    STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c
    )
elseif(USB STREQUAL MASSSTORAGE)
  add_definitions(-DUSB_MASS_STORAGE)
  set(STM32USB_SRC
    ${STM32USB_SRC}
    STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
    STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
    STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
    STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
    )
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    ../common/arm/stm32/usbd_storage_msd.cpp
    )
else()
  add_definitions(-DUSB_JOYSTICK)
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    ../common/arm/stm32/usbd_hid_joystick.c
    )
endif()
if(GVARS)
  set(GUI_SRC
    ${GUI_SRC}
    ${GVAR_SCREEN}
    )
endif()
set(FIRMWARE_SRC
  ${FIRMWARE_SRC}
  serial.cpp
  syscalls.c
  )
foreach(FILE ${STM32LIB_SRC})
  set(FIRMWARE_SRC
    ${FIRMWARE_SRC}
    ${STM32LIB_DIR}/${FILE}
    )
endforeach()
foreach(FILE ${STM32USB_SRC})
  set(FIRMWARE_SRC
    ${FIRMWARE_SRC}
    ${STM32USB_DIR}/${FILE}
    )
endforeach()
set(SRC
  ${SRC}
  sbus.cpp
  telemetry/crossfire.cpp
  io/bootloader_flash.cpp
  )
set(PULSES_SRC
  ${PULSES_SRC}
  crossfire.cpp
  )
