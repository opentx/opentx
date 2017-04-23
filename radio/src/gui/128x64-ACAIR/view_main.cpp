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

#include "opentx.h"

#define BIGSIZE       DBLSIZE
#if defined (PCBX7)
  #define LBOX_CENTERX  (LCD_W/4 + 14)
  #define RBOX_CENTERX  (3*LCD_W/4 - 13)
#else
  #define LBOX_CENTERX  (LCD_W/4 + 10)
  #define RBOX_CENTERX  (3*LCD_W/4 - 10)
#endif
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW-1)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define BAR_HEIGHT    (BOX_WIDTH-1l) // don't remove the l here to force 16bits maths on 9X
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)
#define TRIM_LH_NEG   (TRIM_LH_X+1*FW)
#define TRIM_LH_POS   (TRIM_LH_X-4*FW)
#define TRIM_RH_NEG   (TRIM_RH_X+1*FW)
#define TRIM_RH_POS   (TRIM_RH_X-4*FW)

#define TRIM_LEN      23

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedAnalogs[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX+20, calibratedAnalogs[CONVERT_MODE(0)], calibStickVert);

#if NUM_STICKS > 2
  calibStickVert = calibratedAnalogs[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedAnalogs[CONVERT_MODE(3)], calibStickVert);
#endif
}

#if defined(PCBACAIR)
void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<3*NUM_STICKS; i++) {
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);

    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }

    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    }
    else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    }
    else {
      val /= 4;
    }

    if (i >= 3) {
      ym = 36;
      xm = LCD_W - 20 + 8 * (i - 3);
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }

      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber(dir>0 ? 12 : 40, xm-2, -abs(dir/5), TINSIZE|VERTICAL);
        }
      }
    }
    else {
      ym = 44 + (8 * (2-i));
      xm = TRIM_LEN + 4;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
#endif
#if defined(CPUARM)
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber((stickIndex==0 ? (dir>0 ? TRIM_LH_POS : TRIM_LH_NEG) : (dir>0 ? TRIM_RH_POS : TRIM_RH_NEG)), ym-2, -abs(dir/5), TINSIZE);
        }
      }
#endif
    }
    lcdDrawSquare(xm-3, ym-3, 7, att);
  }
}
#else
void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    static coord_t x[NUM_STICKS] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[NUM_STICKS] = {0,1,1,0};
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];
    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }
#endif
    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    }
    else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    }
    else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }
#endif
#if defined(CPUARM)
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber(dir>0 ? 12 : 40, xm-2, -abs(dir/5), TINSIZE|VERTICAL);
        }
      }
#endif
    }
    else {
      ym = 60;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
#endif
#if defined(CPUARM)
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber((stickIndex==0 ? (dir>0 ? TRIM_LH_POS : TRIM_LH_NEG) : (dir>0 ? TRIM_RH_POS : TRIM_RH_NEG)), ym-2, -abs(dir/5), TINSIZE);
        }
      }
#endif
    }
    lcdDrawSquare(xm-3, ym-3, 7, att);
  }
}
#endif


void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, RIGHT);
  lcdDrawSolidFilledRect(VBATT_X-25, VBATT_Y+9, 21, 5);
  lcdDrawSolidVerticalLine(VBATT_X-4, VBATT_Y+10, 3);
  uint8_t count = GET_TXBATT_BARS();
  for (uint8_t i=0; i<count; i+=2)
    lcdDrawSolidVerticalLine(VBATT_X-24+i, VBATT_Y+10, 3);
  if (!IS_TXBATT_WARNING() || BLINK_ON_PHASE)
    lcdDrawSolidFilledRect(VBATT_X-26, VBATT_Y, 24, 15);
#else
  LcdFlags att = (IS_TXBATT_WARNING() ? BLINK|INVERS : 0) | BIGSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
  lcdDrawChar(VBATT_X, VBATTUNIT_Y, 'V');
#endif
}

#if defined(PCBSKY9X)
void displayVoltageOrAlarm()
{
  if (g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
    drawValueWithUnit(6*FW-1, 2*FH, getTemperature(), UNIT_TEMPERATURE, BLINK|INVERS|DBLSIZE|RIGHT);
  }
  else if (g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.txCurrentCalibration)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
    drawValueWithUnit(7*FW-1, 2*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.txCurrentCalibration)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE|RIGHT);
  }
  else {
    displayBattVoltage();
  }
}
#else
  #define displayVoltageOrAlarm() displayBattVoltage()
#endif

#if defined(PCBACAIR)
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_LONG(KEY_ENTER)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_PAGE)
#define EVT_KEY_NEXT_PAGE              EVT_KEY_BREAK(KEY_RIGHT)
#define EVT_KEY_PREVIOUS_PAGE          EVT_KEY_BREAK(KEY_LEFT)
#define EVT_KEY_MODEL_MENU             EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_PAGE)
#elif defined(PCBX7)
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_LONG(KEY_ENTER)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_PAGE)
#define EVT_KEY_NEXT_PAGE              EVT_ROTARY_RIGHT
#define EVT_KEY_PREVIOUS_PAGE          EVT_ROTARY_LEFT
#define EVT_KEY_MODEL_MENU             EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_PAGE)
#else
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_BREAK(KEY_UP)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_DOWN)
#define EVT_KEY_NEXT_PAGE              EVT_KEY_BREAK(KEY_RIGHT)
#define EVT_KEY_PREVIOUS_PAGE          EVT_KEY_BREAK(KEY_LEFT)
#define EVT_KEY_MODEL_MENU             EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_LAST_MENU              EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS             EVT_KEY_LONG(KEY_UP)
#endif

void displayRssiLine()
{
  if (moduleFlag[INTERNAL_MODULE] & MODULE_BIND) {
    lcdDrawText(40, 20, "BIND", INVERS);
  }
  else if (g_eeGeneral.inactivityTimer && inactivity.counter > g_eeGeneral.inactivityTimer*60) {
    lcdDrawText(40, 20, "INACTIVITY", INVERS);
  }
  else if (TELEMETRY_STREAMING()) {
    uint8_t rssi;
    rssi = min((uint8_t)99, TELEMETRY_RSSI());
    lcdDrawText(40, 16, "RSSI : ", SMLSIZE);
    lcdDrawNumber(68, 16, rssi, LEADING0 | SMLSIZE, 2);
    lcdDrawRect(40, 24, 38, 7);
    uint8_t v = 4*rssi/11;
    lcdDrawFilledRect(41, 25, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcdDrawText(40, 20, STR_NODATA, INVERS);
  }
}

void menuMainView(event_t event)
{
  switch (event) {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    case EVT_KEY_MODEL_MENU:
      pushMenu(menuTabModel[0]);
      killEvents(event);
      break;
  }

  {
    // Model Name
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.header.name, g_eeGeneral.currModel, BIGSIZE);

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Trims sliders
    displayTrims(0);

    // RSSI
    displayRssiLine();
  }

  for (uint8_t i=0; i<2; i++) {
    int16_t val = channelOutputs[i];
    uint8_t y = LCD_H/2+FH+2+i*(FH+2);
    drawStringWithIndex(LCD_W/2-FW-3, y, "CH", i+1, SMLSIZE | INVERS);
    lcdDrawNumber(lcdLastRightPos + 1, y, PPM_CH_CENTER(i)+val/2);
    lcdDrawText(lcdLastRightPos, y, "us");
  }

  if (unexpectedShutdown) {
    lcdDrawChar(REBOOT_X, 0*FH, '!', INVERS);
  }
}

#undef EVT_KEY_CONTEXT_MENU
#undef EVT_KEY_PREVIOUS_VIEW
#undef EVT_KEY_NEXT_VIEW
#undef EVT_KEY_NEXT_PAGE
#undef EVT_KEY_PREVIOUS_PAGE
#undef EVT_KEY_MODEL_MENU
#undef EVT_KEY_GENERAL_MENU
#undef EVT_KEY_LAST_MENU
#undef EVT_KEY_TELEMETRY
#undef EVT_KEY_STATISTICS
