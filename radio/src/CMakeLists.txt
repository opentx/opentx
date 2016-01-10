include(CMakeForceCompiler)
include(Bitmaps)

option(PCB "Radio type")
option(HELI "Heli menu" ON)
option(FLIGHT_MODES "Flight Modes" ON)
option(CURVES "Curves" ON)
option(GVARS "Global variables")
option(GUI "GUI enabled" ON)
option(SPLASH "Splash enabled" ON)
option(PPM_UNIT "PPM display unit (US/PERCENT_PREC1/PERCENT_PREC0)" PERCENT_PREC0)
option(PPM_CENTER_ADJUSTABLE "PPM center adjustable" OFF)
option(PPM_LIMITS_SYMETRICAL "PPM limits symetrical" OFF)
option(OVERRIDE_CHANNEL_FUNCTION "OverrideChannel function available" OFF)
option(DANGEROUS_MODULE_FUNCTIONS "Dangerous module functions (RangeCheck / Bind / Module OFF, etc.) available" OFF)
option(FAI "Competition mode (no telemetry)" OFF)
option(AUTOSOURCE "Automatic source detection in menus" ON)
option(AUTOSWITCH "Automatic switch detection in menus" ON)

enable_language(ASM)
set(OPT s)

set(THIRDPARTY_DIR thirdparty)
set(LUA_DIR ${THIRDPARTY_DIR}/Lua/src)
set(COOS_DIR ${THIRDPARTY_DIR}/CoOS)
set(FATFS_DIR ${THIRDPARTY_DIR}/FatFs)
set(RADIO_BIN_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(translations)
add_subdirectory(bitmaps)
add_subdirectory(lua)

set(EEPROM_VARIANT  0)
set(GVARS_VARIANT   1)
set(FRSKY_VARIANT   2)
set(3POS_VARIANT    4)
set(MAVLINK_VARIANT 8)
set(M2561_VARIANT   16384)
set(M128_VARIANT    32768)

set(FIRMWARE_DEPENDENCIES firmware_translations)

if(PCB STREQUAL HORUS)
  set(CPU_TYPE STM32F4)
  set(HSE_VALUE 12000000)
  set(SDCARD YES)
  set(EEPROM SDCARD)
  set(HAPTIC YES)
  set(GUI_DIR horus)
  set(TARGET_DIR horus)
  set(LINKER_SCRIPT targets/horus/stm32f4_flash.ld)
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} horus_bitmaps)
  set(LUA_EXPORT lua_export_horus)
  set(FLAVOUR horus)
  add_definitions(-DPCBHORUS -DCOLORLCD -DSTM32F429_439xx)
  add_definitions(-DEEPROM_VARIANT=0 -DAUDIO -DVOICE -DRTCLOCK)
  add_definitions(-DVIRTUALINPUTS -DLUAINPUTS -DXCURVES -DVARIO)
  include_directories(${RADIO_SRC_DIRECTORY}/fonts/horus)
  set(GUI_SRC
    ${GUI_SRC}
    curves.cpp
    bitmaps.cpp
    view_channels.cpp
    view_about.cpp
    view_text.cpp
    view_telemetry.cpp # TODO remove it
    )
  set(SRC ${SRC} bmp.cpp targets/taranis/rtc_driver.cpp)
  set(TARGET_SRC ${TARGET_SRC} board_horus.cpp)
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    sdio_sd.c
    lcd_driver.cpp
    delays.c
    i2c_driver.cpp
    usb_driver.c
    pwr_driver.c
    sdram_driver.c
    led_driver.cpp
    )
  set(STM32LIB_SRC
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
    )
elseif(PCB STREQUAL TARANIS)
  option(PCBREV "PCB Revision" REVPLUS)
  option(SHUTDOWN_CONFIRMATION "Shutdown confirmation" OFF)
  option(LCD_DUAL_BUFFER "Dual LCD Buffer" OFF)
  option(TARANIS_INTERNAL_PPM "Taranis internal module hack to output PPM" OFF)
  if(PCBREV STREQUAL REV9E)
    set(CPU_TYPE STM32F4)
    set(LINKER_SCRIPT targets/taranis/stm32f4_flash.ld)
    set(HAPTIC YES)
    set(LUA_EXPORT lua_export_taranis_x9e)
    set(FLAVOUR taranis-x9e)
    add_definitions(-DSTM32F40_41xxx -DREVPLUS -DREV9E)
    add_definitions(-DEEPROM_VARIANT=32768)
    set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} top_lcd_driver.cpp bluetooth_driver.cpp)
  elseif(PCBREV STREQUAL REVPLUS)
    set(CPU_TYPE STM32F2)
    set(LINKER_SCRIPT targets/taranis/stm32f2_flash.ld)
    set(HAPTIC YES)
    set(LUA_EXPORT lua_export_taranis)
    set(FLAVOUR taranis-plus)
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
  add_definitions(-DPCBTARANIS)
  add_definitions(-DAUDIO -DVOICE -DRTCLOCK)
  add_definitions(-DDBLKEYS -DVIRTUALINPUTS -DLUAINPUTS -DXCURVES -DVARIO)
  set(SRC ${SRC} bmp.cpp)
  set(GUI_SRC ${GUI_SRC} menu_general_diagkeys.cpp menu_general_diaganas.cpp menu_general_hardware.cpp view_channels.cpp view_telemetry.cpp view_text.cpp view_about.cpp)
  set(TARGET_SRC ${TARGET_SRC} board_taranis.cpp rtc_driver.cpp)
  set(FIRMWARE_SRC ${FIRMWARE_SRC} loadboot.cpp)
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    lcd_driver.cpp
    delays.c
    i2c_driver.cpp
    usb_driver.c
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
elseif(PCB STREQUAL SKY9X OR PCB STREQUAL 9XRPRO OR PCB STREQUAL AR9X)
  set(ARCH ARM)
  if(PCB STREQUAL 9XRPRO)
    add_definitions(-Dat91sam3s8 -DREVX)
    set(FLAVOUR 9xrpro)
    set(LINKER_SCRIPT targets/sky9x/sam3s8c_flash.ld)
  elseif(PCB STREQUAL AR9X)
    add_definitions(-Dat91sam3s8 -DAR9X)
    set(FLAVOUR ar9x)
    set(LINKER_SCRIPT targets/sky9x/sam3s8c_flash.ld)
  else()
    set(FLAVOUR sky9x)
    option(PCBREV "PCB Revision" REVC)
    set(TARGET_SRC ${TARGET_SRC} coproc_driver.cpp rotenc_driver.cpp)
    add_definitions(-DROTARY_ENCODERS=1)
    if(PCBREV STREQUAL REVA)
      add_definitions(-DDREVA)
      set(LINKER_SCRIPT targets/sky9x/sam3s2c_flash.ld)
    elseif(PCBREV STREQUAL REVB)
      add_definitions(-DDREVB -DCOPROCESSOR)
      set(LINKER_SCRIPT targets/sky9x/sam3s4c_flash.ld)
    else()
      add_definitions(-DDREVC -DCOPROCESSOR)
      set(LINKER_SCRIPT targets/sky9x/sam3s4c_flash.ld)
    endif()
  endif()
  set(MCU cortex-m3)
  set(SDCARD YES)
  set(HAPTIC YES)
  set(EEPROM EEPROM_RAW)
  set(GUI_DIR 9x)
  set(TARGET_DIR sky9x)
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} 9x_bitmaps)
  add_definitions(-DPCBSKY9X)
  add_definitions(-DAUDIO -DVOICE -DRTCLOCK)
  add_definitions(-DDBLKEYS -DVARIO)
  add_definitions(-DEEPROM_VARIANT=0)
  set(GUI_SRC ${GUI_SRC} menu_general_diagkeys.cpp menu_general_diaganas.cpp menu_general_hardware.cpp view_telemetry.cpp view_text.cpp view_about.cpp)
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} core_cm3.c board_lowlevel.c crt.c vectors_sam3s.c)
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} lcd_driver.cpp
    usb/device/core/USBD_UDP.c usb/device/core/USBDDriver.c
    usb/device/massstorage/MSDDriver.c usb/device/massstorage/MSDDStateMachine.c usb/device/massstorage/MSDLun.c
    usb/device/massstorage/MSDDriverDescriptors.c usb/device/massstorage/SBCMethods.c usb/common/core/USBEndpointDescriptor.c
    usb/common/core/USBGenericRequest.c usb/common/core/USBFeatureRequest.c usb/common/core/USBInterfaceRequest.c
    usb/common/core/USBGetDescriptorRequest.c usb/common/core/USBSetAddressRequest.c usb/common/core/USBSetConfigurationRequest.c
    usb/common/core/USBConfigurationDescriptor.c usb/common/core/USBGenericDescriptor.c MEDSdcard.c
    )
  set(TARGET_SRC
    ${TARGET_SRC}
    rtc_driver.cpp
    board_sky9x.cpp
    telemetry_driver.cpp
    serial2_driver.cpp
    pwr_driver.cpp
    adc_driver.cpp
    eeprom_driver.cpp
    pulses_driver.cpp
    keys_driver.cpp audio_driver.cpp buzzer_driver.cpp haptic_driver.cpp sdcard_driver.cpp massstorage.cpp
    )
  set(SRC ${SRC} debug.cpp)
  set(FIRMWARE_SRC ${FIRMWARE_SRC} loadboot.cpp)
elseif(PCB STREQUAL 9X OR PCB STREQUAL 9XR)
  set(ARCH AVR)
  set(MCU atmega64)
  string(TOLOWER ${PCB} FLAVOUR)
  set(EEPROM_VARIANT_NEEDED ON)
  set(EEPROM EEPROM_RLC)
  set(GUI_DIR 9x)
  set(TARGET_DIR 9x)
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} 9x_bitmaps)
  add_definitions(-DPCBSTD -DPCB${PCB} -DCPUM64)
  set(TARGET_SRC ${TARGET_SRC} board_stock.cpp)
  set(GUI_SRC ${GUI_SRC} menu_general_diagkeys.cpp menu_general_diaganas.cpp)
else()
  message(FATAL_ERROR "Unknown PCB '${PCB}'")
endif()

if(CPU_TYPE STREQUAL STM32F4)
  set(CPU_FAMILY STM32)
  set(MCU cortex-m4)
  set(STM32LIB_DIR ${THIRDPARTY_DIR}/STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries)
  include_directories(${STM32LIB_DIR}/STM32F4xx_StdPeriph_Driver/inc)
  include_directories(${STM32LIB_DIR}/CMSIS/Device/ST/STM32F4xx/Include)
  include_directories(${STM32LIB_DIR}/CMSIS/include)
  set(STM32LIB_SRC
    ${STM32LIB_SRC}
    CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
    STM32F4xx_StdPeriph_Driver/src/misc.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
    STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
    )
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} system_stm32f4xx.c)
endif()

if(CPU_TYPE STREQUAL STM32F2)
  set(CPU_FAMILY STM32)
  set(MCU cortex-m3)
  set(STM32LIB_DIR ${THIRDPARTY_DIR}/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries)
  include_directories(${STM32LIB_DIR}/STM32F2xx_StdPeriph_Driver/inc)
  include_directories(${STM32LIB_DIR}/CMSIS/Device/ST/STM32F2xx/Include)
  include_directories(${STM32LIB_DIR}/CMSIS/include)
  set(STM32LIB_SRC
    ${STM32LIB_SRC}
    CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
    STM32F2xx_StdPeriph_Driver/src/misc.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dbgmcu.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_i2c.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dma.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rtc.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_pwr.c
    STM32F2xx_StdPeriph_Driver/src/stm32f2xx_usart.c
    )
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} system_stm32f2xx.c)
endif()

if(CPU_FAMILY STREQUAL STM32)
  option(LUA "Lua scripts")
  option(USB "USB option (JOYSTICK/MASSSTORAGE/SERIAL)" JOYSTICK)
  set(ARCH ARM)
  set(STM32USB_DIR ${THIRDPARTY_DIR}/STM32_USB-Host-Device_Lib_V2.1.0/Libraries)
  add_definitions(-DCPUSTM32)
  add_definitions(-DSBUS -DCROSSFIRE)
  include_directories(${STM32USB_DIR}/STM32_USB_OTG_Driver/inc)
  include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Core/inc)
  include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/inc)
  include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/hid/inc)
  include_directories(${STM32USB_DIR}/STM32_USB_Device_Library/Class/cdc/inc)
  set(STM32USB_SRC
    STM32_USB_OTG_Driver/src/usb_core.c
    STM32_USB_OTG_Driver/src/usb_dcd.c
    STM32_USB_OTG_Driver/src/usb_dcd_int.c
    STM32_USB_Device_Library/Core/src/usbd_core.c
    STM32_USB_Device_Library/Core/src/usbd_ioreq.c
    STM32_USB_Device_Library/Core/src/usbd_req.c
    )
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} usb_bsp.c usbd_desc.c usbd_usr.cpp)
  if(USB STREQUAL SERIAL)
    add_definitions(-DUSB_SERIAL)
    set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} usbd_cdc.cpp)
    set(STM32USB_SRC ${STM32USB_SRC} STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c)
  elseif(USB STREQUAL MASSSTORAGE)
    add_definitions(-DUSB_MASS_STORAGE)
    set(STM32USB_SRC
      ${STM32USB_SRC}
      STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
      STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
      STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
      STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
      )
    set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} usbd_storage_msd.cpp)
  else()
    add_definitions(-DUSB_JOYSTICK)
    set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} usbd_hid_joystick.c)
  endif()
  if(GVARS)
    set(GUI_SRC ${GUI_SRC} menu_model_gvars.cpp)
  endif()
  set(FIRMWARE_SRC
    ${FIRMWARE_SRC}
    serial.cpp
    syscalls.c
    targets/taranis/configure_pins.cpp
    )
  foreach(FILE ${STM32LIB_SRC})
    set(FIRMWARE_SRC ${FIRMWARE_SRC} ${STM32LIB_DIR}/${FILE})
  endforeach()
  foreach(FILE ${STM32USB_SRC})
    set(FIRMWARE_SRC ${FIRMWARE_SRC} ${STM32USB_DIR}/${FILE})
  endforeach()
endif()

if(ARCH STREQUAL ARM)
  option(TIMERS "Timers count" 2)
  option(CLI "Command Line Interface" OFF)
  option(DEBUG "Debug mode" OFF)
  option(SPORT_FILE_LOG "S.PORT Logs on SD card" OFF)
  option(TRACE_SD_CARD "Traces SD enabled" OFF)
  option(TRACE_FATFS "Traces FatFS enabled" OFF)
  option(TRACE_AUDIO "Traces audio enabled" OFF)
  option(DEBUG_TRACE_BUFFER "Debug Trace Screen" OFF)
  option(SUPPORT_D16_EU_ONLY "XJT module only supports D16-EU and LR12-EU" OFF) # TODO rename to XJT_EU_ONLY
  if(TIMERS EQUAL 3)
    add_definitions(-DTIMERS=3)
  else()
    add_definitions(-DTIMERS=2)
  endif()
  if(SPORT_FILE_LOG)
    add_definitions(-DSPORT_FILE_LOG)
  endif()
  if(TRACE_SD_CARD)
    add_definitions(-DTRACE_SD_CARD)
    set(DEBUG ON)
    set(DEBUG_TRACE_BUFFER ON)
  endif()
  if(TRACE_FATFS)
    add_definitions(-DTRACE_FATFS)
    set(DEBUG ON)
    set(DEBUG_TRACE_BUFFER ON)
  endif()
  if(TRACE_AUDIO)
    add_definitions(-DTRACE_AUDIO)
    set(DEBUG ON)
    set(DEBUG_TRACE_BUFFER ON)
  endif()
  if(DEBUG_TRACE_BUFFER)
    add_definitions(-DDEBUG_TRACE_BUFFER)
  endif()
  if(CLI)
    add_definitions(-DCLI)
    set(SRC ${SRC} cli.cpp dump.cpp)
  elseif(DEBUG)
    set(SRC ${SRC} dump.cpp)
  endif()
  if(DEBUG)
    add_definitions(-DDEBUG)
  endif()
  if(SUPPORT_D16_EU_ONLY)
    add_definitions(-DMODULE_D16_EU_ONLY_SUPPORT)
  endif()
  if(HAPTIC)
    add_definitions(-DHAPTIC)
    set(SRC ${SRC} haptic.cpp)
    set(TARGET_SRC ${TARGET_SRC} haptic_driver.cpp)
  endif()
  add_definitions(-DCPUARM)
  add_definitions(-DFRSKY -DFRSKY_SPORT -DFRSKY_HUB -DGPS -DPXX -DDSM2)
  add_definitions(-DBOLD_FONT -DBATTGRAPH -DTHRTRACE)
  include_directories(targets/${TARGET_DIR} ${THIRDPARTY_DIR})
  include_directories(${COOS_DIR} ${COOS_DIR}/kernel ${COOS_DIR}/portable)
  foreach(LANGUAGE cz de en es fr it pt sk se pl hu) # TODO add nl
    set(SRC ${SRC} translations/tts_${LANGUAGE}.cpp)
    string(TOUPPER LANGUAGE_UPPER LANGUAGE)
    if(TRANSLATIONS STREQUAL LANGUAGE_CAPITALIZED)
      add_definitions(-DTRANSLATIONS_${LANGUAGE_UPPER})
    endif()
  endforeach()
  set(SRC
    ${SRC}
    main_arm.cpp
    tasks_arm.cpp
    audio_arm.cpp
    sbus.cpp
    telemetry/telemetry.cpp
    telemetry/frsky.cpp
    telemetry/frsky_d_arm.cpp
    telemetry/frsky_sport.cpp
    telemetry/telemetry.cpp
    crc16.cpp
    vario.cpp
    )
  set(FIRMWARE_TARGET_SRC
    ${FIRMWARE_TARGET_SRC}
    diskio.cpp
  )
  set(TARGET_SRC
    ${TARGET_SRC}
    pulses_driver.cpp
    keys_driver.cpp
    adc_driver.cpp
    trainer_driver.cpp
    audio_driver.cpp
    serial2_driver.cpp
    telemetry_driver.cpp
    )
  set(FIRMWARE_SRC
    ${FIRMWARE_SRC}
    ${FATFS_DIR}/ff.c
    ${FATFS_DIR}/fattime.c
    ${FATFS_DIR}/option/ccsbcs.c
    ${COOS_DIR}/kernel/core.c
    ${COOS_DIR}/kernel/hook.c
    ${COOS_DIR}/kernel/task.c
    ${COOS_DIR}/kernel/event.c
    ${COOS_DIR}/kernel/time.c
    ${COOS_DIR}/kernel/timer.c
    ${COOS_DIR}/kernel/flag.c
    ${COOS_DIR}/kernel/mutex.c
    ${COOS_DIR}/kernel/serviceReq.c
    ${COOS_DIR}/portable/GCC/port.c
    ${COOS_DIR}/portable/arch.c
    )
  set(PULSES_SRC
    pulses_arm.cpp
    ppm_arm.cpp
    pxx_arm.cpp
    dsm2_arm.cpp
    crossfire.cpp
    )
else()
  option(EXT "Telemetry extension" OFF)
  option(AUDIO "Audio support" OFF)
  option(VOICE "Voice support" OFF)
  option(GRAPHICS "Additional graphics" ON)
  option(BOLD "Bold font" ON)
  option(BATTGRAPH "Battery graph" OFF)
  set(PULSES_SRC pulses_avr.cpp)
  set(SRC ${SRC} main_avr.cpp)
  set(FIRMWARE_TARGET_SRC ${FIRMWARE_TARGET_SRC} lcd_driver.cpp)
  set(FIRMWARE_SRC ${FIRMWARE_SRC} targets/common_avr/adc_driver.cpp)
  if(AUDIO)
    add_definitions(-DAUDIO)
    set(SRC ${SRC} audio_avr.cpp)
    set(BUZZER OFF)
  else()
    set(BUZZER ON)
  endif()
  if(BUZZER)
    add_definitions(-DBUZZER)
    set(SRC ${SRC} buzzer.cpp)
  endif()
  if(VOICE)
    add_definitions(-DVOICE)
    set(TARGET_SRC ${TARGET_SRC} voice.cpp)
  endif()
  if(GRAPHICS)
    add_definitions(-DGRAPHICS)
  endif()
  if(BOLD)
    add_definitions(-DBOLD_FONT)
  endif()
  if(BATTGRAPH)
    add_definitions(-DBATTGRAPH)
  endif()
  if(${EXT} STREQUAL OFF)
    add_definitions(-DEXTSTD)
  elseif(EXT STREQUAL ARDUPILOT)
    add_definitions(-DARDUPILOT)
    set(SRC ${SRC} telemetry/ardupilot.cpp)
  elseif(EXT STREQUAL NMEA)
    add_definitions(-DNMEA)
    set(SRC ${SRC} telemetry/nmea.cpp)
  elseif(EXT STREQUAL JETI)
    add_definitions(-DJETI)
    set(SRC ${SRC} telemetry/jeti.cpp)
  elseif(EXT STREQUAL MAVLINK)
    add_definitions(-DMAVLINK)
    include_directories(${THIRDPARTY} targets/common_avr)
    set(SRC ${SRC} telemetry/mavlink.cpp targets/common_avr/serial_driver.cpp)
    set(GUI_SRC ${GUI_SRC} view_mavlink.cpp)
    math(EXPR EEPROM_VARIANT ${EEPROM_VARIANT}+${MAVLINK_VARIANT})
  elseif(EXT STREQUAL TELEMETREZ)
    add_definitions(-DTELEMETREZ)
    set(SRC ${SRC} telemetry/jeti.cpp)
  elseif(EXT STREQUAL FRSKY_SPORT)
    add_definitions(-DFRSKY_SPORT)
    set(SRC ${SRC} crc16.cpp telemetry/frsky_sport.cpp)
  endif()
  if(EXT STREQUAL FRSKY OR EXT STREQUAL FRSKY_SPORT OR EXT STREQUAL TELEMETREZ)
    option(FRSKY_HUB "FrSky Hub support" ON)
    option(WS_HOW_HIGH "Winged Shadow sensors support" ON)
    option(GAUGES "Telemetry gauges" OFF)
    option(GPS "GPS support" ON)
    option(VARIO "Vario support" ON)
    add_definitions(-DFRSKY)
    set(SRC ${SRC} telemetry/frsky.cpp telemetry/frsky_d.cpp)
    set(GUI_SRC ${GUI_SRC} view_telemetry.cpp)
    if(FRSKY_HUB)
      add_definitions(-DFRSKY_HUB)
    endif()
    if(WS_HOW_HIGH)
      add_definitions(-DWS_HOW_HIGH)
    endif()
    if(GAUGES)
      add_definitions(-DGAUGES)
    endif()
    if(GPS)
      add_definitions(-DGPS)
    endif()
    if(VARIO)
      add_definitions(-DVARIO)
      set(SRC ${SRC} vario.cpp)
    endif()
    math(EXPR EEPROM_VARIANT ${EEPROM_VARIANT}+${FRSKY_VARIANT})
  endif()
  add_definitions(-DTIMERS=2)
  if(EEPROM_VARIANT_NEEDED)
    add_definitions(-DEEPROM_VARIANT=${EEPROM_VARIANT})
  endif()
endif()

set(SRC ${SRC} debug.cpp)

if(${EEPROM} STREQUAL SDCARD)
  set(SRC ${SRC} storage/storage_common.cpp storage/sdcard_raw.cpp)
elseif(${EEPROM} STREQUAL EEPROM_RLC)
  set(SRC ${SRC} storage/storage_common.cpp storage/eeprom_common.cpp storage/eeprom_rlc.cpp)
  add_definitions(-DEEPROM -DEEPROM_RLC)
else()
  set(SRC ${SRC} storage/storage_common.cpp storage/eeprom_common.cpp storage/eeprom_raw.cpp)
  add_definitions(-DEEPROM)
endif()

if(NOT ${EEPROM} STREQUAL SDCARD AND ARCH STREQUAL ARM)
  set(SRC ${SRC} storage/eeprom_conversions.cpp)
endif()

add_definitions(-DFLAVOUR="${FLAVOUR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/bitmaps/${GUI_DIR})

set(GUI_SRC
  ${GUI_SRC}
  lcd.cpp
  splash.cpp
  fonts.cpp
  helpers.cpp
  navigation.cpp
  popups.cpp
  widgets.cpp
  menus.cpp
  )

set(GUI_SRC
  ${GUI_SRC}
  menu_model.cpp
  menu_model_select.cpp
  menu_model_setup.cpp
  menu_model_inputs_mixes.cpp
  menu_model_limits.cpp
  menu_model_logical_switches.cpp
  menu_model_custom_functions.cpp
  menu_model_telemetry.cpp
  menu_general.cpp
  menu_general_setup.cpp
  menu_general_trainer.cpp
  menu_general_version.cpp
  menu_general_calib.cpp
  view_main.cpp
  view_statistics.cpp
  )

if(LUA)
  add_definitions(-DLUA)
  include_directories(${LUA_DIR})
  set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} ${LUA_EXPORT})
  if(LUA STREQUAL ON)
    add_definitions(-DLUA_MODEL_SCRIPTS)
    set(GUI_SRC ${GUI_SRC} menu_model_custom_scripts.cpp)
  endif()
  set(SRC ${SRC} lua/interface.cpp lua/api_general.cpp lua/api_lcd.cpp lua/api_model.cpp)
  set(LUA_SRC lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c
    lstate.c lstring.c ltable.c lrotable.c ltm.c lundump.c lvm.c lzio.c linit.c
    lbaselib.c lmathlib.c lbitlib.c loadlib.c lauxlib.c ltablib.c lcorolib.c liolib.c lstrlib.c
    )
  foreach(FILE ${LUA_SRC})
    set(SRC ${SRC} ${LUA_DIR}/${FILE})
  endforeach()
endif()

if(HELI)
  add_definitions(-DHELI)
  set(GUI_SRC ${GUI_SRC} menu_model_heli.cpp)
endif()

if(FLIGHT_MODES)
  add_definitions(-DFLIGHT_MODES)
  set(GUI_SRC ${GUI_SRC} menu_model_flightmodes.cpp)
endif()

if(CURVES)
  add_definitions(-DCURVES)
  set(GUI_SRC ${GUI_SRC} menu_model_curves.cpp)
endif()

if(GVARS)
  add_definitions(-DGVARS)
  math(EXPR EEPROM_VARIANT ${EEPROM_VARIANT}+${GVARS_VARIANT})
endif()

if(FAI STREQUAL YES)
  add_definitions(-DFAI)
elseif(FAI STREQUAL CHOICE)
  add_definitions(-DFAI_CHOICE)
endif()

if(AUTOSOURCE)
  add_definitions(-DAUTOSOURCE)
endif()

if(AUTOSWITCH)
  add_definitions(-DAUTOSWITCH)
endif()

if(SDCARD STREQUAL YES)
  add_definitions(-DSDCARD)
  include_directories(${FATFS_DIR} ${FATFS_DIR}/option)
  set(SRC ${SRC} sdcard.cpp rtc.cpp logs.cpp)
  set(GUI_SRC ${GUI_SRC} menu_general_sdmanager.cpp)
endif()

if(SHUTDOWN_CONFIRMATION)
  add_definitions(-DSHUTDOWN_CONFIRMATION)
endif()

if(PPM_UNIT STREQUAL US)
  add_definitions(-DPPM_UNIT_US)
elseif(PPM_UNIT STREQUAL PERCENT_PREC1)
  add_definitions(-DPPM_UNIT_PERCENT_PREC1)
else()
  add_definitions(-DPPM_UNIT_PERCENT_PREC0)
endif()

if(PPM_CENTER_ADJUSTABLE)
  add_definitions(-DPPM_CENTER_ADJUSTABLE)
endif()

if(PPM_LIMITS_SYMETRICAL)
  add_definitions(-DPPM_LIMITS_SYMETRICAL)
endif()

if(OVERRIDE_CHANNEL_FUNCTION)
  add_definitions(-DOVERRIDE_CHANNEL_FUNCTION)
endif()

if(DANGEROUS_MODULE_FUNCTIONS)
  add_definitions(-DDANGEROUS_MODULE_FUNCTIONS)
endif()

if(SPLASH)
  add_definitions(-DSPLASH)
endif()

set(SRC
  ${SRC}
  opentx.cpp
  functions.cpp
  strhelpers.cpp
  switches.cpp
  curves.cpp
  mixer.cpp
  stamp.cpp
  timers.cpp
  trainer_input.cpp
  )

if(GUI)
  add_definitions(-DGUI)
  set(SRC
    ${SRC}
    keys.cpp
    maths.cpp
    translations.cpp
    gui/gui_helpers.cpp
    )
  foreach(FILE ${GUI_SRC})
    set(SRC ${SRC} gui/${GUI_DIR}/${FILE})
  endforeach()
endif()

foreach(FILE ${TARGET_SRC})
  set(SRC targets/${TARGET_DIR}/${FILE} ${SRC})
endforeach()

foreach(FILE ${FIRMWARE_TARGET_SRC})
  set(FIRMWARE_SRC ${FIRMWARE_SRC} targets/${TARGET_DIR}/${FILE})
endforeach()

foreach(FILE ${PULSES_SRC})
  set(SRC ${SRC} pulses/${FILE})
endforeach()

add_definitions(-DCORRECT_NEGATIVE_SHIFTS)

if(NOT WIN32)
  add_subdirectory(targets/simu)
  add_subdirectory(tests)
endif()

set(SRC ${SRC} ${FIRMWARE_SRC})

set(WARNING_FLAGS "-Wall -Wno-strict-aliasing -Wformat -Wreturn-type -Wunused -Wuninitialized -Wunknown-pragmas -Wno-switch -Wtype-limits")

# trick to remove the -rdynamic issue
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

if(ARCH STREQUAL ARM)
  cmake_force_c_compiler(arm-none-eabi-gcc GNU)
  cmake_force_cxx_compiler(arm-none-eabi-gcc GNU)
  set(CMAKE_ASM_COMPILER arm-none-eabi-as)

  set(CMAKE_SYSTEM_NAME Generic)
  set(CMAKE_SYSTEM_VERSION 1)
  set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> -o <OBJECT> <SOURCE>")

  set(COMMON_FLAGS "-mcpu=${MCU} -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=firmware.lst -O${OPT} -gdwarf-2 -DHSE_VALUE=${HSE_VALUE} -fno-exceptions -fdata-sections -ffunction-sections ${WARNING_FLAGS}")
  set(CMAKE_C_FLAGS "${COMMON_FLAGS} -Wimplicit")
  set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "-mcpu=${MCU} -mthumb -nostartfiles -lm -T${RADIO_SRC_DIRECTORY}/${LINKER_SCRIPT} -Wl,-Map=firmware.map,--cref,--no-warn-mismatch,--gc-sections")

  if(PCB STREQUAL TARANIS)
    add_subdirectory(targets/${TARGET_DIR}/bootloader)
    add_custom_command(
      OUTPUT bootloader.bin bootloader.lbm
      COMMAND arm-none-eabi-objcopy -O binary targets/${TARGET_DIR}/bootloader/bootloader.elf bootloader.bin
      COMMAND python ${RADIO_DIRECTORY}/util/bin2lbm.py bootloader.bin bootloader.lbm
      DEPENDS bootloader.elf
      )
    add_custom_target(bootloader DEPENDS bootloader.lbm)
    set(FIRMWARE_DEPENDENCIES ${FIRMWARE_DEPENDENCIES} bootloader)
  else()
    file(DOWNLOAD http://ersky9x.googlecode.com/svn/trunk/src/bootloader/bootflash8.lbm ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lbm)
  endif()

  add_executable(firmware.elf ${SRC} ${FIRMWARE_HEADERS})
  add_dependencies(firmware.elf ${FIRMWARE_DEPENDENCIES})

  add_custom_target(firmware.bin
    COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
    DEPENDS firmware.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
  cmake_force_c_compiler(avr-gcc GNU)
  cmake_force_cxx_compiler(avr-gcc GNU)

  set(CMAKE_CXX_FLAGS "-mmcu=${MCU} -DF_CPU=16000000UL -O${OPT} -gdwarf-2 -fno-exceptions -flto -fwhole-program -fno-inline-small-functions -mstrict-X ${WARNING_FLAGS}")

  include_directories(storage gui/${GUI_DIR})

  set(ALLSRC ${PROJECT_BINARY_DIR}/allsrc.cpp)
  foreach(FILE ${SRC})
    file(READ ${FILE} FILE_CONTENT)
    set(ALLSRC_CONTENT "${ALLSRC_CONTENT}# 1 \"${FILE}\"\n${FILE_CONTENT}")
  endforeach()

  file(WRITE ${ALLSRC} "${ALLSRC_CONTENT}")

  add_executable(firmware.elf ${ALLSRC})
  add_dependencies(firmware.elf ${FIRMWARE_DEPENDENCIES})

  add_custom_target(firmware.hex
    COMMAND avr-objcopy -O ihex -R .eeprom firmware.elf firmware.hex
    DEPENDS firmware.elf
    )

  add_custom_target(firmware.bin
    COMMAND avr-objcopy -O binary firmware.elf firmware.bin
    DEPENDS firmware.elf
    )

  add_custom_target(firmware
    COMMAND avr-size --mcu=${MCU} --format=avr firmware.elf
    DEPENDS firmware.hex
    )
endif()

add_custom_target(firmware.lss
  COMMAND avr-objdump -h -S firmware.elf > firmware.lss
  DEPENDS firmware.elf
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

if(CPU_FAMILY STREQUAL STM32)
  add_custom_target(flash
    COMMAND dfu-util --alt 0 --dfuse-address 0x08000000 -D firmware.bin
    DEPENDS firmware.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

