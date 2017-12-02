#include "opentx.h"
#include "../../common/arm/stm32/bootloader/boot.h"

#define SELECTED_COLOR (INVERS | TEXT_COLOR)

#include "bmp_plug_usb.lbm"
#include "bmp_usb_plugged.lbm"

const uint8_t LBM_FLASH[] = {
#include "icon_flash.lbm"
};

const uint8_t LBM_EXIT[] = {
#include "icon_exit.lbm"
};

const uint8_t LBM_SD[] = {
#include "icon_sd.lbm"
};

const uint8_t LBM_FILE[] = {
#include "icon_file.lbm"
};

void bootloaderInitScreen()
{
  lcdColorTable[TEXT_COLOR_INDEX]            = BLACK;
  lcdColorTable[TEXT_BGCOLOR_INDEX]          = WHITE;
  lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
 
  backlightEnable(BACKLIGHT_LEVEL_MAX);
}

static void bootloaderDrawTitle(unsigned int x, const char* text)
{
    lcdDrawText(x, 28, text);
    lcdDrawSolidFilledRect(28, 56, 422, 2, TEXT_COLOR);
}

static void bootloaderDrawFooter()
{
    lcdDrawSolidFilledRect(28, 234, 422, 2, TEXT_COLOR);
}

void bootloaderDrawScreen(BootloaderState st, int opt)
{
    lcdDrawSolidFilledRect(0, 0, LCD_W-1, LCD_H-1, TEXT_BGCOLOR);
    
    if (st == ST_START) {

        bootloaderDrawTitle(88, "HORUS BOOTLOADER");
        
        lcdDrawBitmapPattern(90, 72, LBM_FLASH, TEXT_COLOR);
        lcdDrawText(124,  75, "Write Firmware");

        lcdDrawBitmapPattern(90, 107, LBM_EXIT, TEXT_COLOR);
        lcdDrawText(124, 110, "Exit");

        lcdDrawSolidRect(119, (opt == 0) ? 72 : 107, 270, 26, 2, TEXT_INVERTED_BGCOLOR);
        
        lcd->drawBitmap(60, 166, &BMP_PLUG_USB);
        lcdDrawText(195, 175, "Or plug in a USB cable");
        lcdDrawText(195, 200, "for mass storage");

        bootloaderDrawFooter();
        lcdDrawText( 36, 242, "Current Firmware:");
        lcdDrawText(200, 242, getOtherVersion());
    }
    else if (st == ST_USB) {

        lcd->drawBitmap(136, 98, &BMP_USB_PLUGGED);
        lcdDrawText(195, 128, "USB Connected");
    }
    else if (st == ST_FILE_LIST || st == ST_DIR_CHECK || st == ST_FLASH_CHECK) {

        bootloaderDrawTitle(126, "SD>FIRMWARE");
        lcdDrawBitmapPattern(87, 16, LBM_SD, TEXT_COLOR);

        bootloaderDrawFooter();

        lcdDrawBitmapPattern(88, 242, LBM_FLASH, TEXT_COLOR);
        lcdDrawText(116, 244, "Write Firmware");

        lcdDrawBitmapPattern(290, 242, LBM_EXIT, TEXT_COLOR);
        lcdDrawText(320, 244, "Exit");
    }
    else if (st == ST_FLASHING) {

        bootloaderDrawTitle(126, "SD>FIRMWARE");
        lcdDrawBitmapPattern(87, 16, LBM_SD, TEXT_COLOR);

        lcdDrawRect(70, 120, 340, 31, 2);
        lcdDrawSolidFilledRect(74, 124, (336 * opt) / 100, 23, TEXT_INVERTED_BGCOLOR);

        bootloaderDrawFooter();
        lcdDrawBitmapPattern(88, 242, LBM_FLASH, TEXT_COLOR);
        lcdDrawText(116, 244, "Writing Firmware...");
        
    }
    else if (st == ST_FLASH_DONE) {

        bootloaderDrawTitle(126, "SD>FIRMWARE");
        lcdDrawBitmapPattern(87, 16, LBM_SD, TEXT_COLOR);

        lcdDrawBitmapPattern(40, 124, LBM_FLASH, TEXT_COLOR);
        lcdDrawText(90, 127, "Writing complete");
    }
}

void bootloaderDrawMessage(BootloaderState st, const char* str, uint8_t line, bool invert)
{
    lcdDrawText(124, 75 + (line * 25), str);

    if (invert && (st == ST_FILE_LIST || st == ST_START)) {
        lcdDrawSolidRect(119, 72 + (line * 25), 278, 26, 2, TEXT_INVERTED_BGCOLOR);
    }

    if (st == ST_FILE_LIST) {
        lcdDrawBitmapPattern(94, 76 + (line * 25), LBM_FILE, TEXT_COLOR);
    }
}
