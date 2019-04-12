set(FONT "Roboto/Roboto-Regular")
set(FONT_BOLD "Roboto/Roboto-Bold")
set(FONT_OFFSET 1)

#set(FONT "Kanit/Kanit-Regular")
#set(FONT_BOLD "Kanit/Kanit-Bold")
#set(FONT_OFFSET -2)

#set(FONT "Ubuntu/Ubuntu-Regular")
#set(FONT_BOLD "Ubuntu/Ubuntu-Bold")
#set(FONT_OFFSET 1)

add_truetype_font_target(800x480 tinsize     ${FONT}      9  ${FONT_OFFSET})
add_truetype_font_target(800x480 smlsize     ${FONT}      13 ${FONT_OFFSET})
add_truetype_font_target(800x480 stdsize     ${FONT}      16 ${FONT_OFFSET})
add_truetype_font_target(800x480 stdsizebold ${FONT_BOLD} 16 ${FONT_OFFSET})
add_truetype_font_target(800x480 midsize     ${FONT}      24 ${FONT_OFFSET})
add_truetype_font_target(800x480 dblsize     ${FONT_BOLD} 32 ${FONT_OFFSET})
add_truetype_font_target(800x480 xxlsize     ${FONT_BOLD} 64 ${FONT_OFFSET})

add_custom_target(ttf_t16hd_fonts DEPENDS ${ttf_800x480_fonts_targets})
