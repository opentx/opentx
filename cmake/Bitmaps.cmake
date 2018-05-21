macro(add_bitmaps_target targetname filter width format)
  set(bitmaps_files)
  file(GLOB bitmaps ${filter})
  foreach(bitmap ${bitmaps})
    get_filename_component(target ${bitmap} NAME_WE)
    set(target ${target}.lbm)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/img2lbm.py ${bitmap} ${target} ${width} ${format} ${ARGN}
      DEPENDS ${bitmap}
    )
    list(APPEND bitmaps_files ${target})
  endforeach()
  add_custom_target(${targetname} DEPENDS ${bitmaps_files})
endmacro(add_bitmaps_target)

macro(add_truetype_font_target radio name font size offset)
  set(target ${RADIO_SRC_DIRECTORY}/fonts/${radio}/font_${name})
  add_custom_target(ttf_${radio}_${name}
    COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/font2png.py ${font} ${size} ${offset} ${target}
    WORKING_DIRECTORY ${RADIO_SRC_DIRECTORY}
  )
  set(ttf_${radio}_fonts_targets ${ttf_${radio}_fonts_targets} ttf_${radio}_${name})
endmacro(add_truetype_font_target)
