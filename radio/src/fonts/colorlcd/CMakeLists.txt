set(FONT "Roboto/Roboto-Regular")
set(FONT_BOLD "Roboto/Roboto-Bold")
set(FONT_OFFSET 1)

#set(FONT "Kanit/Kanit-Regular")
#set(FONT_BOLD "Kanit/Kanit-Bold")
#set(FONT_OFFSET -2)

#set(FONT "Ubuntu/Ubuntu-Regular")
#set(FONT_BOLD "Ubuntu/Ubuntu-Bold")
#set(FONT_OFFSET 1)

add_truetype_font_target(480x272 tinsize     ${FONT}      9  ${FONT_OFFSET})
add_truetype_font_target(480x272 smlsize     ${FONT}      13 ${FONT_OFFSET})
add_truetype_font_target(480x272 stdsize     ${FONT}      16 ${FONT_OFFSET})
add_truetype_font_target(480x272 stdsizebold ${FONT_BOLD} 16 ${FONT_OFFSET})
add_truetype_font_target(480x272 midsize     ${FONT}      24 ${FONT_OFFSET})
add_truetype_font_target(480x272 dblsize     ${FONT_BOLD} 32 ${FONT_OFFSET})
add_truetype_font_target(480x272 xxlsize     ${FONT_BOLD} 64 ${FONT_OFFSET})

add_custom_target(ttf_horus_fonts DEPENDS ${ttf_480x272_fonts_targets})
