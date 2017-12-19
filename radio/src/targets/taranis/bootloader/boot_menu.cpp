#include "opentx.h"
#include "../../common/arm/stm32/bootloader/boot.h"
#include "../../common/arm/stm32/bootloader/bin_files.h"

extern MemoryType memoryType;

void bootloaderInitScreen()
{
}

static void bootloaderDrawMsg(unsigned int x, const char* str, uint8_t line, bool inverted)
{
    lcdDrawSizedText(x, (line + 2) * FH, str, DISPLAY_CHAR_WIDTH, inverted ? INVERS : 0);
}

void bootloaderDrawFilename(const char* str, uint8_t line, bool selected)
{
    bootloaderDrawMsg(INDENT_WIDTH, str, line, selected);
}

void bootloaderDrawScreen(BootloaderState st, int opt, const char* str)
{
    lcdClear();
    lcdDrawText(0, 0, BOOTLOADER_TITLE, INVERS);
        
    if (st == ST_START) {
        lcdDrawTextAlignedLeft(2*FH, "\010Write Firmware");
        lcdDrawTextAlignedLeft(3*FH, "\010Restore EEPROM");
        lcdDrawTextAlignedLeft(4*FH, "\010Exit");

#if LCD_W >= 212
        lcdDrawTextAlignedLeft(6*FH, "\001Curr FW:");
        lcdDrawText(50, 6*FH, getOtherVersion(nullptr));
#else
        lcdDrawTextAlignedLeft(6*FH, "\001FW:");

        // Remove opentx- from string
        const char* other_ver = getOtherVersion(nullptr);
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
    else if (st == ST_DIR_CHECK) {
        if (opt == FR_NO_PATH) {
            bootloaderDrawMsg(INDENT_WIDTH, "Directory is missing!", 1, false);
            bootloaderDrawMsg(INDENT_WIDTH, getBinaryPath(memoryType), 2, false);
        }
        else {
            bootloaderDrawMsg(INDENT_WIDTH, "Directory is empty!", 1, false);
        }
    }
    else if (st == ST_FLASH_CHECK) {
        if (opt == FC_ERROR) {

            if (memoryType == MEM_FLASH)
                bootloaderDrawMsg(0, STR_INVALID_FIRMWARE, 2, false);
            else
                bootloaderDrawMsg(0, STR_INVALID_EEPROM, 2, false);
        }
        else if (opt == FC_OK) {

            const char* vers = getOtherVersion((char*)Block_buffer);
#if LCD_W < 212
            // Remove opentx- from string
            if (strstr(vers, "opentx-"))
                vers = vers+7;
#endif
            bootloaderDrawMsg(INDENT_WIDTH, vers, 0, false);
            bootloaderDrawMsg(0, STR_HOLD_ENTER_TO_START, 2, false);
        }
    }
    else if (st == ST_FLASHING) {
        lcdDrawTextAlignedLeft(4*FH, CENTER "\015Writing...");

        lcdDrawRect(3, 6*FH+4, (LCD_W-8), 7);
        lcdDrawSolidHorizontalLine(5, 6*FH+6, (LCD_W-12) * opt / 100, FORCE);
        lcdDrawSolidHorizontalLine(5, 6*FH+7, (LCD_W-12) * opt / 100, FORCE);
        lcdDrawSolidHorizontalLine(5, 6*FH+8, (LCD_W-12) * opt / 100, FORCE);
    }
    else if (st == ST_FLASH_DONE) {

        lcdDrawTextAlignedLeft(4*FH, CENTER "\007Writing complete");
    }
}
