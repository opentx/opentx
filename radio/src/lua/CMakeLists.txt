if(MSVC)
  set(INTTYPES_INCLUDE_OPTION -IC:/Programs/msinttypes)
endif()

macro(add_lua_export_target target)
  add_custom_command(
    OUTPUT lua_exports_${target}.inc
    COMMAND ${CMAKE_C_COMPILER} -E ${ARGN} -DEXPORT ${INTTYPES_INCLUDE_OPTION} ${RADIO_SRC_DIRECTORY}/myeeprom.h | grep LEXP > lua_exports_${target}.txt
    COMMAND python ${RADIO_DIRECTORY}/util/luaexport.py ${VERSION} lua_exports_${target}.txt lua_exports_${target}.inc lua_fields_${target}.txt
  )
  add_custom_target(lua_export_${target} DEPENDS lua_exports_${target}.inc)
endmacro(add_lua_export_target)

add_lua_export_target(taranis     -DCPUARM -DPCBTARANIS                   -DLUA -DVIRTUALINPUTS)
add_lua_export_target(taranis_x9e -DCPUARM -DPCBTARANIS -DREVPLUS -DREV9E -DLUA -DVIRTUALINPUTS)
add_lua_export_target(horus       -DCPUARM -DPCBHORUS                     -DLUA -DVIRTUALINPUTS)
