set(FONT "Roboto/Roboto-Regular")
set(FONT_BOLD "Roboto/Roboto-Bold")
set(FONT_OFFSET 1)

#set(FONT "Kanit/Kanit-Regular")
#set(FONT_BOLD "Kanit/Kanit-Bold")
#set(FONT_OFFSET -2)

#set(FONT "Ubuntu/Ubuntu-Regular")
#set(FONT_BOLD "Ubuntu/Ubuntu-Bold")
#set(FONT_OFFSET 1)

add_truetype_font_target(9      ${FONT}      9  ${FONT_OFFSET})
add_truetype_font_target(13     ${FONT}      13 ${FONT_OFFSET})
add_truetype_font_target(15     ${FONT}      15 ${FONT_OFFSET})
add_truetype_font_target(16     ${FONT}      16 ${FONT_OFFSET})
add_truetype_font_target(bold16 ${FONT_BOLD} 16 ${FONT_OFFSET})
add_truetype_font_target(17     ${FONT}      17 ${FONT_OFFSET})
add_truetype_font_target(bold17 ${FONT_BOLD} 17 ${FONT_OFFSET})
add_truetype_font_target(24     ${FONT}      24 ${FONT_OFFSET})
add_truetype_font_target(32     ${FONT_BOLD} 32 ${FONT_OFFSET})
add_truetype_font_target(64     ${FONT_BOLD} 64 ${FONT_OFFSET})

add_custom_target(truetype_fonts DEPENDS ${truetype_fonts_targets})
