#include "opentx.h"
#include "../../common/arm/stm32/bootloader/boot.h"

#define SELECTED_COLOR (INVERS | TEXT_COLOR)

void bootloaderInitScreen()
{
  // init screen / colors: should go to target / gui specific place
  lcdColorTable[TEXT_COLOR_INDEX]            = BLACK;
  lcdColorTable[TEXT_BGCOLOR_INDEX]          = WHITE;
  lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
  
  backlightEnable(BACKLIGHT_LEVEL_MAX);
}

void bootloaderDrawScreen(BootloaderState st, int opt)
{
    lcdDrawSolidFilledRect(0, 0, LCD_W-1, LCD_H-1, TEXT_BGCOLOR);
    lcdDrawText(0, 0, BOOTLOADER_TITLE, INVERS | TEXT_COLOR);
        
    if (st == ST_START) {
        lcdDrawText(0, 2*FH, "\010Write Firmware", opt == 0 ? SELECTED_COLOR : TEXT_COLOR);
        lcdDrawText(0, 3*FH, "\010Exit", opt == 1 ? SELECTED_COLOR : TEXT_COLOR);

        lcdDrawText(0, 7*FH, STR_OR_PLUGIN_USB_CABLE, TEXT_COLOR);
    }
    else if (st == ST_USB) {
        lcdDrawText(0, 4*FH, STR_USB_CONNECTED);
    }
    else if (st == ST_FLASHING) {
        lcdDrawText(0, 4*FH, CENTER "\015Writing...");
        
        lcdDrawRect(3, 6*FH+4, (LCD_W-8), 7);
        lcdDrawSolidHorizontalLine(5, 6*FH+6, opt, TEXT_COLOR);
        lcdDrawSolidHorizontalLine(5, 6*FH+7, opt, TEXT_COLOR);
        lcdDrawSolidHorizontalLine(5, 6*FH+8, opt, TEXT_COLOR);
    }
    else if (st == ST_FLASH_DONE) {

        lcdDrawText(0, 4*FH, CENTER "\007Writing complete", TEXT_COLOR);
    }
}

void bootloaderDrawMessage(const char* str, uint8_t line, bool invert)
{
    lcdDrawText(INDENT_WIDTH, (line + 2) * FH, str, invert ? SELECTED_COLOR : TEXT_COLOR);
}
