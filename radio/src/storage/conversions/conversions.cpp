/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

void ConvertModel(int id, int version)
{
  eeLoadModelData(id);

  if (version == 216) {
    version = 217;
    ConvertModel_216_to_217(g_model);
  }
  if (version == 217) {
    version = 218;
    ConvertModel_217_to_218(g_model);
  }

  uint8_t currModel = g_eeGeneral.currModel;
  g_eeGeneral.currModel = id;
  storageDirty(EE_MODEL);
  storageCheck(true);
  g_eeGeneral.currModel = currModel;
}

bool eeConvert()
{
  const char *msg = NULL;

  if (g_eeGeneral.version == 216) {
    msg = "EEprom Data v216";
  }
  else if (g_eeGeneral.version == 217) {
    msg = "EEprom Data v217";
  }
  else {
    return false;
  }

  int conversionVersionStart = g_eeGeneral.version;

  // Information to the user and wait for key press
#if defined(PCBSKY9X)
  g_eeGeneral.optrexDisplay = 0;
#endif
  g_eeGeneral.backlightMode = e_backlight_mode_on;
  g_eeGeneral.backlightBright = 0;
  g_eeGeneral.contrast = 25;

  ALERT(STR_STORAGE_WARNING, msg, AU_BAD_RADIODATA);

  RAISE_ALERT(STR_STORAGE_WARNING, STR_EEPROM_CONVERTING, NULL, AU_NONE);

  // General Settings conversion
  eeLoadGeneralSettingsData();
  int version = conversionVersionStart;
  if (version == 216) {
    version = 217;
    ConvertRadioData_216_to_217(g_eeGeneral);
  }
  if (version == 217) {
    version = 218;
    ConvertRadioData_217_to_218(g_eeGeneral);
  }
  storageDirty(EE_GENERAL);
  storageCheck(true);

#if defined(COLORLCD)
#elif LCD_W >= 212
  lcdDrawRect(60, 6*FH+4, 132, 3);
#else
  lcdDrawRect(10, 6*FH+4, 102, 3);
#endif

  // Models conversion
  for (uint8_t id=0; id<MAX_MODELS; id++) {
#if defined(COLORLCD)
#elif LCD_W >= 212
    lcdDrawSolidHorizontalLine(61, 6*FH+5, 10+id*2, FORCE);
#else
    lcdDrawSolidHorizontalLine(11, 6*FH+5, 10+(id*3)/2, FORCE);
#endif
    lcdRefresh();
    if (eeModelExists(id)) {
      ConvertModel(id, conversionVersionStart);
    }

  }

  return true;
}
