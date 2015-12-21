add_bitmaps_target(taranis_xbm_1bit ${RADIO_SRC_DIRECTORY}/bitmaps/*.xbm 128 1bit)
add_bitmaps_target(taranis_fonts_1bit ${RADIO_SRC_DIRECTORY}/fonts/std/*.png 128 "")
add_bitmaps_target(taranis_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/taranis/*.png 212 4bits)
add_dependencies(taranis_bitmaps taranis_fonts_1bit taranis_xbm_1bit)