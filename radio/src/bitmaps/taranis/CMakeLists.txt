add_bitmaps_target(taranis_xbm ${RADIO_SRC_DIRECTORY}/bitmaps/sticks.xbm 128 1bit 4)
add_bitmaps_target(taranis_fonts ${RADIO_SRC_DIRECTORY}/fonts/std/*.png 128 "")
add_bitmaps_target(taranis_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/taranis/*.png 212 4bits)
add_dependencies(taranis_bitmaps taranis_fonts taranis_xbm)