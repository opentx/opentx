macro(add_bitmaps_target targetname filter width format)
  set(bitmaps_files)
  file(GLOB bitmaps ${filter})
  foreach(bitmap ${bitmaps})
    get_filename_component(target ${bitmap} NAME_WE)
    set(target ${target}.lbm)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/img2lbm.py ${bitmap} ${target} ${width} ${format} ${ARGN}
      DEPENDS ${bitmap} ${RADIO_DIRECTORY}/util/img2lbm.py
    )
    list(APPEND bitmaps_files ${target})
  endforeach()
  add_custom_target(${targetname} DEPENDS ${bitmaps_files})
endmacro()

macro(add_truetype_font_target name size subset font offset cn_font cn_offset)
  set(target ${RADIO_SRC_DIRECTORY}/fonts/truetype/font_${name})
  add_custom_command(
    OUTPUT ${target}.png ${target}.specs
    COMMAND ${PYTHON_EXECUTABLE} ${TOOLS_DIRECTORY}/build-font-bitmap.py --subset ${subset} --size ${size} --font ${font}:${offset} --cn-font ${cn_font}:${cn_offset} --output ${target}
    WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
    DEPENDS ${TOOLS_DIRECTORY}/build-font-bitmap.py ${TOOLS_DIRECTORY}/charset.py ${RADIO_SRC_DIRECTORY}/translations/cn.h.txt
  )
  add_custom_target(truetype_font_${name} DEPENDS ${target}.png ${target}.specs)
  set(truetype_fonts_targets ${truetype_fonts_targets} truetype_font_${name})
endmacro()
