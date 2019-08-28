#include "opentx.h"
#include "../../common/arm/stm32/bootloader/boot.h"
#include "../../common/arm/stm32/bootloader/bin_files.h"

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

const uint8_t LBM_ERROR[] = {
#include "icon_error.lbm"
};

const uint8_t LBM_OK[] = {
#include "icon_ok.lbm"
};

void bootloaderInitScreen()
{
  lcdColorTable[TEXT_COLOR_INDEX]      = BLACK;
  lcdColorTable[TEXT_BGCOLOR_INDEX]    = WHITE;
  lcdColorTable[LINE_COLOR_INDEX]      = RED;
  lcdColorTable[BARGRAPH1_COLOR_INDEX] = RED;
  lcdColorTable[BARGRAPH2_COLOR_INDEX] = RGB(73, 219, 62); // green
 
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

void bootloaderDrawScreen(BootloaderState st, int opt, const char* str)
{
    // clear screen
    lcdDrawSolidFilledRect(0, 0, LCD_W-1, LCD_H-1, TEXT_BGCOLOR);
    
    if (st == ST_START) {

        bootloaderDrawTitle(88, "HORUS BOOTLOADER");
        
        lcdDrawBitmapPattern(90, 72, LBM_FLASH, TEXT_COLOR);
        lcdDrawText(124,  75, "Write Firmware");

        lcdDrawBitmapPattern(90, 107, LBM_EXIT, TEXT_COLOR);
        lcdDrawText(124, 110, "Exit");

        lcdDrawSolidRect(119, (opt == 0) ? 72 : 107, 270, 26, 2, LINE_COLOR);
        
        lcd->drawBitmap(60, 166, &BMP_PLUG_USB);
        lcdDrawText(195, 175, "Or plug in a USB cable");
        lcdDrawText(195, 200, "for mass storage");

        bootloaderDrawFooter();
        lcdDrawText( 36, 242, "Current Firmware:");
        lcdDrawText(200, 242, getOtherVersion(nullptr));
    }
    else if (st == ST_USB) {

        lcd->drawBitmap(136, 98, &BMP_USB_PLUGGED);
        lcdDrawText(195, 128, "USB Connected");
    }
    else if (st == ST_FILE_LIST || st == ST_DIR_CHECK || st == ST_FLASH_CHECK ||
             st == ST_FLASHING || st == ST_FLASH_DONE) {

        bootloaderDrawTitle(126, "SD>FIRMWARE");
        lcdDrawBitmapPattern(87, 16, LBM_SD, TEXT_COLOR);

        if (st == ST_FLASHING || st == ST_FLASH_DONE) {

            LcdFlags color = BARGRAPH1_COLOR; // red

            if (st == ST_FLASH_DONE) {
                color = BARGRAPH2_COLOR/* green */;
                opt   = 100; // Completed > 100%
            }

            lcdDrawRect(70, 120, 340, 31, 2);
            lcdDrawSolidFilledRect(74, 124, (332 * opt) / 100, 23, color);
        }
        else if (st == ST_DIR_CHECK) {

            if (opt == FR_NO_PATH) {
                lcdDrawText(90, 168, "Directory is missing");
            }
            else {
                lcdDrawText(90, 168, "Directory is empty");
            }

            lcdDrawBitmapPattern(356, 158, LBM_ERROR, BARGRAPH1_COLOR);
        }
        else if (st == ST_FLASH_CHECK) {

            bootloaderDrawFilename(str, 0, true);

            if (opt == FC_ERROR) {
                lcdDrawText(94, 168, STR_INVALID_FIRMWARE);
                lcdDrawBitmapPattern(356, 158, LBM_ERROR, BARGRAPH1_COLOR);
            }
            else if (opt == FC_OK) {
                VersionTag tag;
                extractFirmwareVersion(&tag);

                lcdDrawText(168, 158, "Version:", RIGHT);
                lcdDrawText(174, 158, tag.version);
                
                lcdDrawText(168, 178, "Radio:", RIGHT);
                lcdDrawText(174, 178, tag.flavour);
                
                lcdDrawBitmapPattern(356, 158, LBM_OK, BARGRAPH2_COLOR);
            }
        }
        
        bootloaderDrawFooter();

        if ( st != ST_DIR_CHECK && (st != ST_FLASH_CHECK || opt == FC_OK)) {

            lcdDrawBitmapPattern(28, 242, LBM_FLASH, TEXT_COLOR);

            if (st == ST_FILE_LIST) {
                lcdDrawText(56, 244, "[ENT] to select file");
            }
            else if (st == ST_FLASH_CHECK && opt == FC_OK) {
                lcdDrawText(56, 244, "Hold [ENT] long to flash");
            }
            else if (st == ST_FLASHING) {
                lcdDrawText(56, 244, "Writing Firmware ...");
            }
            else if (st == ST_FLASH_DONE) {
                lcdDrawText(56, 244, "Writing Completed");
            }
        }

        if (st != ST_FLASHING) {
            lcdDrawBitmapPattern(305, 242, LBM_EXIT, TEXT_COLOR);
            lcdDrawText(335, 244, "[RTN] to exit");
        }        
    }
}

void bootloaderDrawFilename(const char* str, uint8_t line, bool selected)
{
    lcdDrawBitmapPattern(94, 76 + (line * 25), LBM_FILE, TEXT_COLOR);
    lcdDrawText(124, 75 + (line * 25), str);

    if (selected) {
        lcdDrawSolidRect(119, 72 + (line * 25), 278, 26, 2, LINE_COLOR);
    }
}
