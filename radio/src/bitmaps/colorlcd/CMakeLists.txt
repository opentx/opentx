if(PCB STREQUAL X12S)
  set(BITMAP_TARGET_PREFIX x12s)
  set(BITMAP_FMT_SUFFIX "")
else()
  set(BITMAP_TARGET_PREFIX x10)
  set(BITMAP_FMT_SUFFIX "-R")
endif()

add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/bmp_*.png" 480 "5/6/5${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_calibration_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/calibration/bmp_*.png" 480 "5/6/5${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_button_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/button/alpha_*.png" 480 "4/4/4/4${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_alpha_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/alpha_*.png" 480 "4/4/4/4${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_alpha_calibration_bitmaps "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/calibration/alpha_*.png" 480 "4/4/4/4${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_masks ${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/mask_*.png 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_slider_masks "${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/slider/*.png" 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_layouts_masks "${RADIO_SRC_DIRECTORY}/gui/colorlcd/layouts/*.png" 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_themes_bitmaps "${RADIO_SRC_DIRECTORY}/gui/colorlcd/themes/*.png" 480 "5/6/5${BITMAP_FMT_SUFFIX}")
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_fonts ${RADIO_SRC_DIRECTORY}/fonts/colorlcd/*.png 480 8bits rle)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_volume_masks ${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/volume/*.png 480 8bits)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bootloader_bitmaps ${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/bootloader/bmp_*.png 480 "5/6/5${BITMAP_FMT_SUFFIX}" rle)
add_bitmaps_target(${BITMAP_TARGET_PREFIX}_bootloader_icons ${RADIO_SRC_DIRECTORY}/bitmaps/colorlcd/bootloader/icon_*.png 480 8bits)

add_dependencies(${BITMAP_TARGET_PREFIX}_bitmaps ${BITMAP_TARGET_PREFIX}_calibration_bitmaps ${BITMAP_TARGET_PREFIX}_button_bitmaps ${BITMAP_TARGET_PREFIX}_alpha_bitmaps
                 ${BITMAP_TARGET_PREFIX}_alpha_calibration_bitmaps ${BITMAP_TARGET_PREFIX}_masks ${BITMAP_TARGET_PREFIX}_slider_masks ${BITMAP_TARGET_PREFIX}_layouts_masks
                 ${BITMAP_TARGET_PREFIX}_themes_bitmaps ${BITMAP_TARGET_PREFIX}_fonts ${BITMAP_TARGET_PREFIX}_volume_masks ${BITMAP_TARGET_PREFIX}_bootloader_bitmaps
                 ${BITMAP_TARGET_PREFIX}_bootloader_icons)
