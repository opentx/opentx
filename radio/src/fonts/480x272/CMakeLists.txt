set(FONT "DejaVu Sans")

add_truetype_font_target(480x272 tinsize ${FONT} 9 False)
add_truetype_font_target(480x272 smlsize ${FONT} 13 False)
add_truetype_font_target(480x272 stdsize ${FONT} 16 False)
add_truetype_font_target(480x272 stdsizebold ${FONT} 16 True)
add_truetype_font_target(480x272 midsize ${FONT} 24 False)
add_truetype_font_target(480x272 dblsize ${FONT} 32 True)
add_truetype_font_target(480x272 xxlsize ${FONT} 64 True)

add_custom_target(ttf_horus_fonts DEPENDS ${ttf_480x272_fonts_targets})
