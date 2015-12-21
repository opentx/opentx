add_bitmaps_target(9x_xbm_1bit ${RADIO_SRC_DIRECTORY}/bitmaps/*.xbm 128 1bit)
add_bitmaps_target(9x_fonts_1bit ${RADIO_SRC_DIRECTORY}/fonts/std/*.png 128 "")
add_bitmaps_target(9x_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/9x/*.png 128 1bit)
add_dependencies(9x_bitmaps 9x_fonts_1bit 9x_xbm_1bit)
