string(TOLOWER ${PCB} BITMAP_TARGET_PREFIX)

add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/bmp_*.png" 480 "5/6/5")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_calibration_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/calibration/bmp_*.png" 480 "5/6/5")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_button_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/button/alpha_*.png" 480 "4/4/4/4")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_alpha_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/alpha_*.png" 480 "4/4/4/4")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_alpha_calibration_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/calibration/alpha_*.png" 480 "4/4/4/4")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_masks ${RADIO_SRC_DIRECTORY}/bitmaps/320x480/mask_*.png 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_slider_masks "${RADIO_SRC_DIRECTORY}/bitmaps/320x480/slider/*.png" 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_layouts_masks "${RADIO_SRC_DIRECTORY}/gui/320x480/layouts/*.png" 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_themes_bitmaps "${RADIO_SRC_DIRECTORY}/gui/320x480/themes/*.png" 480 "5/6/5")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_fonts ${RADIO_SRC_DIRECTORY}/fonts/320x480/*.png 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_volume_masks ${RADIO_SRC_DIRECTORY}/bitmaps/320x480/volume/*.png 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bootloader_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/320x480/bootloader/bmp_*.png 480 "5/6/5")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bootloader_icons ${RADIO_SRC_DIRECTORY}/bitmaps/320x480/bootloader/icon_*.png 480 8bits)

add_dependencies(${BITMAP_TARGET_PREFIX}_bitmaps ${BITMAP_TARGET_PREFIX}_calibration_bitmaps ${BITMAP_TARGET_PREFIX}_button_bitmaps ${BITMAP_TARGET_PREFIX}_alpha_bitmaps
                 ${BITMAP_TARGET_PREFIX}_alpha_calibration_bitmaps ${BITMAP_TARGET_PREFIX}_masks ${BITMAP_TARGET_PREFIX}_slider_masks ${BITMAP_TARGET_PREFIX}_layouts_masks
                 ${BITMAP_TARGET_PREFIX}_themes_bitmaps ${BITMAP_TARGET_PREFIX}_fonts ${BITMAP_TARGET_PREFIX}_volume_masks ${BITMAP_TARGET_PREFIX}_bootloader_bitmaps
                 ${BITMAP_TARGET_PREFIX}_bootloader_icons)
