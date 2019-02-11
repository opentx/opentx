#include "opentx.h"

const char *warningText = NULL;
const char *warningInfoText;
uint8_t     warningInfoLength;
uint8_t     warningType;
uint8_t     warningResult = 0;
uint8_t     warningInfoFlags;
int16_t     warningInputValue;
int16_t     warningInputValueMin;
int16_t     warningInputValueMax;
void        (*popupFunc)(uint16_t event) = NULL;
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t     s_menu_item = 0;
uint16_t    popupMenuItemsCount = 0;
uint16_t    popupMenuOffset = 0;
uint8_t     popupMenuOffsetType = 0;
void        (*popupMenuHandler)(const char * result);
uint8_t noHighlightCounter;

void checkLuaMemoryUsage()
{
}

void runPopupWarning(uint16_t event)
{
}

void drawFatalErrorScreen(const char * message)
{
}

bool isTelemetryFieldAvailable(int index)
{
  return false;
}

void loadCustomScreens()
{
}

const LanguagePack * const languagePacks[] = {
  // alphabetical order
  NULL
};

void showAlertBox(const char * title, const char * text, const char * action, uint8_t sound)
{
}

void lcdNextLayer()
{

}

void DMACopy(void * src, void * dest, unsigned size)
{

}

void showMessageBox(const char * title)
{

}

bool menuFirstCalib(event_t event)
{
  return false;
}

void drawAlertBox(const char * title, const char * text, const char * action)
{

}

uint8_t menuCalibrationState;
uint8_t menuLevel;
uint8_t menuPageCount;
uint8_t menuPageIndex;
uint8_t linesDisplayed;
uint16_t linesCount;
int menuVerticalOffset;

void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags, const char * prefix, const char * suffix)
{

}

bool isExternalModuleAvailable(int module)
{
  return false;
}

bool isInternalModuleAvailable(int module)
{
  return false;
}

bool isSourceAvailable(int source)
{
  return true;
}