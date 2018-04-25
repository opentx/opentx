
macro(add_lua_export_target target)
  add_custom_command(
    OUTPUT lua_exports_${target}.inc
    COMMAND ${CMAKE_C_COMPILER} -E ${ARGN} -DEXPORT ${RADIO_SRC_DIRECTORY}/dataconstants.h > lua_exports_${target}.txt
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/luaexport.py ${VERSION} lua_exports_${target}.txt lua_exports_${target}.inc lua_fields_${target}.txt
    )
  add_custom_target(lua_export_${target} DEPENDS lua_exports_${target}.inc)
endmacro(add_lua_export_target)

set(LUA_INCLUDES
  -DCPUARM -DLUA -DVIRTUAL_INPUTS -DLUA_INPUTS
  -I${RADIO_SRC_DIRECTORY}/targets/${TARGET_DIR}
  -I${RADIO_SRC_DIRECTORY}/thirdparty
  -I${RADIO_SRC_DIRECTORY}/targets/common/arm/stm32
  -I${RADIO_SRC_DIRECTORY}/${STM32LIB_DIR}/CMSIS/include
  -I${RADIO_SRC_DIRECTORY}/${STM32USB_DIR}/STM32_USB_OTG_Driver/inc
  -I${RADIO_SRC_DIRECTORY}/${STM32USB_DIR}/STM32_USB_Device_Library/Core/inc
  -I${RADIO_SRC_DIRECTORY}/${STM32USB_DIR}/STM32_USB_Device_Library/Class/cdc/inc
  -I${RADIO_SRC_DIRECTORY}/${STM32USB_DIR}/STM32_USB_Device_Library/Class/msc/inc
  -I${RADIO_SRC_DIRECTORY}/${STM32USB_DIR}/STM32_USB_Device_Library/Class/hid/inc
  )

if(WIN32 AND WIN_INCLUDE_DIRS)
  foreach(include ${WIN_INCLUDE_DIRS})
    set(LUA_INCLUDES "${LUA_INCLUDES} -I${include}")
  endforeach()
endif()

set(LUA_INCLUDES_STM32F2
  ${LUA_INCLUDES}
  -DSTM32F2
  -I${RADIO_SRC_DIRECTORY}/${STM32LIB_DIR}/CMSIS/Device/ST/STM32F2xx/Include
  )

set(LUA_INCLUDES_STM32F4
  ${LUA_INCLUDES}
  -DSTM32F4
  -I${RADIO_SRC_DIRECTORY}/${STM32LIB_DIR}/CMSIS/Device/ST/STM32F4xx/Include
  )

if(PYTHONINTERP_FOUND)
  add_lua_export_target(x7      ${LUA_INCLUDES_STM32F2} -DPCBTARANIS -DPCBX7)
  add_lua_export_target(xlite   ${LUA_INCLUDES_STM32F2} -DPCBTARANIS -DPCBXLITE)
  add_lua_export_target(x9d     ${LUA_INCLUDES_STM32F2} -DPCBTARANIS -DPCBX9D)
  add_lua_export_target(x9e     ${LUA_INCLUDES_STM32F4} -DPCBTARANIS -DPCBX9E  -DSTM32F40_41xxx)
  add_lua_export_target(x10     ${LUA_INCLUDES_STM32F4} -DPCBHORUS   -DPCBX10  -DSTM32F40_41xxx)
  add_lua_export_target(x12s    ${LUA_INCLUDES_STM32F4} -DPCBHORUS   -DPCBX12S -DSTM32F40_41xxx)
endif()
