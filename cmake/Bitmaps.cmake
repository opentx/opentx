set(FONT "Roboto/Roboto-Regular")
set(FONT_BOLD "Roboto/Roboto-Bold")
set(FONT_OFFSET 1)
set(CJK_FONT "Noto/NotoSansCJKsc-Regular")
set(CJK_FONT_BOLD "Noto/NotoSansCJKsc-Bold")
set(CJK_FONT_OFFSET -3)

#set(FONT "Kanit/Kanit-Regular")
#set(FONT_BOLD "Kanit/Kanit-Bold")
#set(FONT_OFFSET -2)

#set(FONT "Ubuntu/Ubuntu-Regular")
#set(FONT_BOLD "Ubuntu/Ubuntu-Bold")
#set(FONT_OFFSET 1)

macro(add_bitmaps_target targetname filter format args)
  set(bitmaps_files)
  file(GLOB bitmaps ${filter})
  foreach(bitmap ${bitmaps})
    get_filename_component(target ${bitmap} NAME_WE)
    set(target ${target}.lbm)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${LIBOPENUI_TOOLS_DIR}/encode-bitmap.py --format ${format} ${args} ${bitmap} ${target}
      DEPENDS ${bitmap} ${LIBOPENUI_TOOLS_DIR}/encode-bitmap.py
    )
    list(APPEND bitmaps_files ${target})
  endforeach()
  add_custom_target(${targetname} DEPENDS ${bitmaps_files})
endmacro()

macro(add_fonts_target targetname filter)
  set(fonts_files)
  file(GLOB fonts ${filter})
  foreach(font ${fonts})
    get_filename_component(target ${font} NAME_WE)
    set(target ${target}.lbm)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${TOOLS_DIR}/encode-font.py ${font} ${target}
      DEPENDS ${font} ${TOOLS_DIR}/encode-font.py
    )
    list(APPEND fonts_files ${target})
  endforeach()
  add_custom_target(${targetname} DEPENDS ${fonts_files})
endmacro()

macro(add_truetype_font_target name size subset effect)
  set(target font_${name})
  if(${effect} STREQUAL "bold")
    set(font ${FONT_BOLD})
    set(cjk_font ${CJK_FONT_BOLD})
  else()
    set(font ${FONT})
    set(cjk_font ${CJK_FONT})
  endif()
  add_custom_command(
    OUTPUT ${target}.png ${target}.specs ${target}.lbm
    COMMAND ${PYTHON_EXECUTABLE} ${TOOLS_DIR}/build-font-bitmap.py --subset ${subset} --size ${size} --font ${font}:${FONT_OFFSET} --cjk-font ${cjk_font}:${CJK_FONT_OFFSET} --output ${target}
    COMMAND ${PYTHON_EXECUTABLE} ${LIBOPENUI_TOOLS_DIR}/encode-bitmap.py --format 8bits --rle --size-format 2 ${target}.png ${target}.lbm
    DEPENDS ${TOOLS_DIR}/build-font-bitmap.py ${TOOLS_DIR}/charset.py ${LIBOPENUI_TOOLS_DIR}/encode-bitmap.py ${RADIO_SRC_DIR}/translations/cn.h.txt
  )
  add_custom_target(truetype_font_${name} DEPENDS ${target}.png ${target}.specs)
  set(truetype_fonts_targets ${truetype_fonts_targets} truetype_font_${name})
endmacro()
