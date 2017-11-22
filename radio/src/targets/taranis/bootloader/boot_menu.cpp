#include "opentx.h"
#include "../../common/arm/stm32/bootloader/boot.h"

void bootloaderInitScreen()
{
}

void bootloaderDrawScreen(BootloaderState st, int opt)
{
    lcdClear();
    lcdDrawText(0, 0, BOOTLOADER_TITLE, INVERS);
        
    if (st == ST_START) {
        lcdDrawTextAlignedLeft(2*FH, "\010Write Firmware");
        lcdDrawTextAlignedLeft(3*FH, "\010Restore EEPROM");
        lcdDrawTextAlignedLeft(4*FH, "\010Exit");

#if LCD_W >= 212
        lcdDrawTextAlignedLeft(6*FH, "\001Curr FW:");
        lcdDrawText(50, 6*FH, getOtherVersion());
#else
        lcdDrawTextAlignedLeft(6*FH, "\001FW:");

        // Remove opentx- from string
        const char* other_ver = getOtherVersion();
        if (strstr(other_ver, "opentx-"))
          other_ver = other_ver+7;
        lcdDrawText(20, 6*FH, other_ver);
#endif

        lcdInvertLine(2+opt);
        lcdDrawTextAlignedLeft(7*FH, STR_OR_PLUGIN_USB_CABLE);
    }
    else if (st == ST_USB) {
        lcdDrawTextAlignedLeft(4*FH, STR_USB_CONNECTED);
    }
    else if (st == ST_FLASHING) {
        lcdDrawTextAlignedLeft(4*FH, CENTER "\015Writing...");
        
        lcdDrawRect(3, 6*FH+4, (LCD_W-8), 7);
        lcdDrawSolidHorizontalLine(5, 6*FH+6, opt, FORCE);
        lcdDrawSolidHorizontalLine(5, 6*FH+7, opt, FORCE);
        lcdDrawSolidHorizontalLine(5, 6*FH+8, opt, FORCE);
    }
    else if (st == ST_FLASH_DONE) {

        lcdDrawTextAlignedLeft(4*FH, CENTER "\007Writing complete");
    }
}

void bootloaderDrawMessage(const char* str, uint8_t line, bool invert)
{
    lcdDrawText(INDENT_WIDTH, (line + 2) * FH, str, invert ? INVERS : 0);
}
