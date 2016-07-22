set(FONT "DejaVu Sans")

add_truetype_font_target(horus tinsize ${FONT} 9 False)
add_truetype_font_target(horus smlsize ${FONT} 13 False)
add_truetype_font_target(horus stdsize ${FONT} 16 False)
add_truetype_font_target(horus stdsizebold ${FONT} 16 True)
add_truetype_font_target(horus midsize ${FONT} 24 False)
add_truetype_font_target(horus dblsize ${FONT} 32 True)
add_truetype_font_target(horus xxlsize ${FONT} 48 False)

add_custom_target(ttf_horus_fonts DEPENDS ${ttf_horus_fonts_targets})
