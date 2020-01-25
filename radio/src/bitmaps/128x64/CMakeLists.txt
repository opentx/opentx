set(FONT "STD" CACHE STRING "Choose font : STD or SQT5")
set_property(CACHE FONT PROPERTY STRINGS "STD" "SQT5")
string(TOLOWER "${FONT}" FONT_DIR)

set(STICKS_ARGS --rows 4)

add_bitmaps_target(9x_xbm_1bit ${RADIO_SRC_DIR}/bitmaps/sticks.xbm 1bit "${STICKS_ARGS}")  # 1bit format, 1byte size, split into 4 rows
add_bitmaps_target(9x_bitmaps ${RADIO_SRC_DIR}/bitmaps/128x64/*.png 1bit "")  # 1bit format, 1byte size, 1 row
add_fonts_target(9x_fonts_1bit ${RADIO_SRC_DIR}/fonts/${FONT_DIR}/*.png)
add_dependencies(9x_bitmaps 9x_fonts_1bit 9x_xbm_1bit)
