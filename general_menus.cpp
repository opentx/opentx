/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "menus.h"
#include "sticks.lbm"

enum EnumTabDiag {
  e_Setup,
#if defined(PCBV3)
  e_FrskyTime,
#endif
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  e_Calib
};

void menuProcSetup(uint8_t event);
#if defined(PCBV3)
void menuProcTime(uint8_t event);
#endif
void menuProcTrainer(uint8_t event);
void menuProcDiagVers(uint8_t event);
void menuProcDiagKeys(uint8_t event);
void menuProcDiagAna(uint8_t event);
void menuProcDiagCalib(uint8_t event);

MenuFuncP_PROGMEM APM menuTabDiag[] = {
  menuProcSetup,
#if defined(PCBV3)
  menuProcTime,
#endif
  menuProcTrainer,
  menuProcDiagVers,
  menuProcDiagKeys,
  menuProcDiagAna,
  menuProcDiagCalib
};

void menuProcSetup(uint8_t event)
{
#define COUNT_ITEMS 19
#define PARAM_OFS   17*FW

  SIMPLE_MENU("RADIO SETUP", menuTabDiag, e_Setup, COUNT_ITEMS+1);

  int8_t  sub    = m_posVert;

  // last 2 lines (radio mode) are non break-able
  if(s_pgOfs==COUNT_ITEMS-7) s_pgOfs= sub<(COUNT_ITEMS-4) ? COUNT_ITEMS-8 : COUNT_ITEMS-6;

  uint8_t y = 1*FH;

  uint8_t subN = 1;
  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Beeper"));
    lcd_putsnAtt(PARAM_OFS - FW, y, PSTR("Quiet""NoKey""Norm ""Long ""xLong")+5*g_eeGeneral.beeperVal,5,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.beeperVal, 0, 4);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Contrast"));
    lcd_outdezAtt(PARAM_OFS,y,g_eeGeneral.contrast,(sub==subN ? INVERS : 0)|LEFT);
    if(sub==subN) {
      CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, 10, 45);
      lcdSetRefVolt(g_eeGeneral.contrast);
    }
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Battery warning"));
    putsVolts(PARAM_OFS, y, g_eeGeneral.vBatWarn, (sub==subN ? INVERS : 0)|LEFT);
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 40, 120); //4-12V
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Inactivity alarm"));
    lcd_outdezAtt(PARAM_OFS, y, g_eeGeneral.inactivityTimer, (sub==subN ? INVERS : 0)|LEFT);
    lcd_putc(lcd_lastPos, y, 'm');
    if(sub==subN) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Filter ADC"));
    lcd_putsnAtt(PARAM_OFS, y, PSTR("SINGOSMPFILT")+4*g_eeGeneral.filterInput,4,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.filterInput, 0, 2);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Throttle reverse"));
    menu_lcd_onoff( PARAM_OFS, y, g_eeGeneral.throttleReversed, sub==subN ) ;
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.throttleReversed, 0, 1);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Minute beep"));
    menu_lcd_onoff( PARAM_OFS, y, g_eeGeneral.minuteBeep, sub==subN ) ;
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.minuteBeep, 0, 1);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Beep countdown"));
    menu_lcd_onoff( PARAM_OFS, y, g_eeGeneral.preBeep, sub==subN ) ;
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.preBeep, 0, 1);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      lcd_puts_P(0, y,PSTR("Flash on beep"));
      menu_lcd_onoff( PARAM_OFS, y, g_eeGeneral.flashBeep, sub==subN ) ;
      if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.flashBeep, 0, 1);
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Light switch"));
    putsSwitches(PARAM_OFS,y,g_eeGeneral.lightSw,sub==subN ? INVERS : 0);
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightSw, -MAX_SWITCH, MAX_SWITCH);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P(0, y,PSTR("Light off after"));
    if(g_eeGeneral.lightAutoOff) {
      lcd_outdezAtt(PARAM_OFS, y, g_eeGeneral.lightAutoOff*5,LEFT|(sub==subN ? INVERS : 0));
      lcd_putc(lcd_lastPos, y, 's');
    }
    else
      lcd_putsnAtt(PARAM_OFS, y, PSTR("OFF"),3,(sub==subN ? INVERS:0));
    if(sub==subN) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
    if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t b = 1-g_eeGeneral.disableSplashScreen;
      lcd_puts_P(0, y,PSTR("Splash screen"));
      menu_lcd_onoff( PARAM_OFS, y, b, sub==subN ) ;
      if(sub==subN)
      {
          CHECK_INCDEC_GENVAR(event, b, 0, 1);
          g_eeGeneral.disableSplashScreen = 1-b;
      }
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t b = 1-g_eeGeneral.disableThrottleWarning;
      lcd_puts_P(0, y,PSTR("Throttle Warning"));
      menu_lcd_onoff( PARAM_OFS, y, b, sub==subN ) ;
      if(sub==subN)
      {
          CHECK_INCDEC_GENVAR(event, b, 0, 1);
          g_eeGeneral.disableThrottleWarning = 1-b;
      }
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      lcd_puts_P(0, y,PSTR("Switch Warning"));
      lcd_putsnAtt(PARAM_OFS, y, PSTR("Down""OFF ""Up  ")+4*(1+g_eeGeneral.switchWarning),4,(sub==subN ? INVERS:0));
      if(sub==subN)
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.switchWarning, -1, 1);
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
      lcd_puts_P(0, y,PSTR("Memory Warning"));
      menu_lcd_onoff( PARAM_OFS, y, b, sub==subN ) ;
      if(sub==subN)
      {
          CHECK_INCDEC_GENVAR(event, b, 0, 1);
          g_eeGeneral.disableMemoryWarning = 1-b;
      }
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
      lcd_puts_P(0, y,PSTR("Alarm Warning"));
      menu_lcd_onoff( PARAM_OFS, y, b, sub==subN ) ;
      if(sub==subN)
      {
          CHECK_INCDEC_GENVAR(event, b, 0, 1);
          g_eeGeneral.disableAlarmWarning = 1-b;
      }
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t b = g_eeGeneral.enableTelemetryAlarm;
      lcd_puts_P(0, y,PSTR("NO DATA Alarm"));
      menu_lcd_onoff( PARAM_OFS, y, b, sub==subN ) ;
      if(sub==subN)
      {
          CHECK_INCDEC_GENVAR(event, b, 0, 1);
          g_eeGeneral.enableTelemetryAlarm = b;
      }
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
      uint8_t attr = sub==subN?INVERS:0;
      lcd_puts_P(0, y,PSTR("Rx Channel Ord"));//   RAET->AETR
      for (uint8_t i=1; i<=4; i++)
        putsChnLetter((16+i)*FW, y, i, attr);
      if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
      if((y+=FH)>7*FH) return;
  }subN++;

  if(s_pgOfs<subN) {
    lcd_puts_P( 1*FW, y, PSTR("Mode"));//sub==3?INVERS:0);
    if(y<7*FH) {for(uint8_t i=0; i<4; i++) lcd_img((6+4*i)*FW, y, sticks,i,0); }
    if((y+=FH)>7*FH) return;

    lcd_putcAtt( 3*FW, y, '1'+g_eeGeneral.stickMode,sub==subN?INVERS:0);
    for(uint8_t i=0; i<4; i++) putsChnRaw( (6+4*i)*FW, y,i+1,0);//sub==3?INVERS:0);

    if(sub==subN) CHECK_INCDEC_GENVAR(event,g_eeGeneral.stickMode,0,3);
    if((y+=FH)>7*FH) return;
  }
}


#if defined(PCBV3)
// SD card interface contains Real-Time-Clock chip
void menuProcTime(uint8_t event)
{
  MENU("DATE AND TIME", menuTabDiag, e_FrskyTime, 3, {0, 2/*, 2*/});

  int8_t  sub    = m_posVert - 1; // vertical position (1 = page counter, top/right)
  uint8_t subSub = m_posHorz;     // horizontal position
  static struct tm t;
  struct tm *at = &t;

  switch(event)
  {
    case EVT_KEY_LONG(KEY_MENU):
      // get data time from RTC chip (may not implement)
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_MENU):
      if (sub >= 0 && !s_editMode) // set the date and time into RTC chip
      {
        g_ms100 = 0; // start of next second begins now
        g_unixTime = mktime(&t); // update local timestamp and get wday calculated

        RTC rtc;
        rtc.year = t.tm_year + 1900;
        rtc.month = t.tm_mon + 1;
        rtc.mday = t.tm_mday;
        rtc.hour = t.tm_hour;
        rtc.min = t.tm_min;
        rtc.sec = t.tm_sec;
        rtc.wday = t.tm_wday + 1;

        rtc_settime(&rtc);

      }
      break;
  }

  if (!s_editMode) filltm(&g_unixTime, &t);

  lcd_putc(FW*10+2, FH*2, '-'); lcd_putc(FW*13, FH*2, '-');
  lcd_putc(FW*10+1, FH*4, ':'); lcd_putc(FW*13-1, FH*4, ':');

  for(uint8_t i=0; i<2; i++) // 2 rows, date then time
  {
    uint8_t y=(i*2+2)*FH;

    lcd_putsnAtt(0, y, PSTR("DATE:""TIME:")+i*5, 5, 0);

    for(uint8_t j=0; j<3;j++) // 3 settings each for date and time (YMD and HMS)
    {
      uint8_t attr = (sub==i && subSub==j) ? (s_editMode ? BLINK : INVERS) : 0;
      switch(i)
      {
        case 0: // DATE
          switch(j)
          {
            case 0:
              lcd_outdezAtt(FW*10+2, y, at->tm_year+1900, attr);
              if(attr && (s_editMode || p1valdiff)) at->tm_year = checkIncDec( event, at->tm_year, 110, 200, 0);
              break;
            case 1:
              lcd_outdezNAtt(FW*13, y, at->tm_mon+1, attr|LEADING0, 2);
              if(attr && (s_editMode || p1valdiff)) at->tm_mon = checkIncDec( event, at->tm_mon, 0, 11, 0);
              break;
            case 2:
              lcd_outdezNAtt(FW*16-2, y, at->tm_mday, attr|LEADING0, 2);
              if(attr && (s_editMode || p1valdiff)) at->tm_mday = checkIncDec( event, at->tm_mday, 1, 31, 0);
              break;
          }
          break;

        case 1:
          switch (j)
          {
            case 0:
              lcd_outdezNAtt(FW*10+1, y, at->tm_hour, attr|LEADING0, 2);
              if(attr && (s_editMode || p1valdiff)) at->tm_hour = checkIncDec( event, at->tm_hour, 0, 23, 0);
              break;
            case 1:
              lcd_outdezNAtt(FW*13-1, y, at->tm_min, attr|LEADING0, 2);
              if(attr && (s_editMode || p1valdiff)) at->tm_min = checkIncDec( event, at->tm_min, 0, 59, 0);
              break;
            case 2:
              lcd_outdezNAtt(FW*16-2, y, at->tm_sec, attr|LEADING0, 2);
              if(attr && (s_editMode || p1valdiff)) at->tm_sec = checkIncDec( event, at->tm_sec, 0, 59, 0);
              break;
          }
          break;

      }
    }
  }
}
#endif

void menuProcTrainer(uint8_t event)
{
  MENU("TRAINER", menuTabDiag, e_Trainer, 7, {0, 2, 2, 2, 2, 0/*, 0*/});

  int8_t  sub    = m_posVert;
  uint8_t subSub = m_posHorz;
  uint8_t y;
  bool    edit;
  uint8_t blink ;

  if (SLAVE_MODE) { // i am the slave
    lcd_puts_P(7*FW, 3*FH, PSTR("Slave"));
    return;
  }

  lcd_puts_P(3*FW, 1*FH, PSTR("mode   % src"));

  sub--;
  y = 2*FH;
  blink = s_editMode ? BLINK : INVERS ;

  for (uint8_t i=0; i<4; i++) {
    uint8_t chan = chout_ar[g_eeGeneral.templateSetup][i]; // G: Issue 30.

    volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

    putsChnRaw(0, y, chan, 0);

    edit = (sub==i && subSub==0);
    lcd_putsnAtt(4*FW, y, PSTR("off += :=")+3*td->mode, 3, edit ? blink : 0);
    if (edit && s_editMode)
      CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);

    edit = (sub==i && subSub==1);
    lcd_outdezAtt(11*FW, y, td->studWeight, edit ? blink : 0);
    if (edit && s_editMode)
      CHECK_INCDEC_GENVAR(event, td->studWeight, -100, 100);

    edit = (sub==i && subSub==2);
    lcd_putsnAtt(12*FW, y, PSTR("ch1ch2ch3ch4")+3*td->srcChn, 3, edit ? blink : 0);
    if (edit && s_editMode)
      CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);

    edit = (sub==i && subSub==3);

    y += FH;
  }

  lcd_puts_P(0*FW, y, PSTR("Multiplier"));
  lcd_outdezAtt(13*FW, y, g_eeGeneral.PPM_Multiplier+10, (sub==4 ? INVERS : 0)|PREC1);
  if(sub==4) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);
  y += FH;

  edit = (sub==5);
  lcd_putsAtt(0*FW, y, PSTR("Cal"), edit ? INVERS : 0);
  for (uint8_t i=0; i<4; i++) {
    uint8_t x = (i*8+16)*FW/2;
#if defined (DECIMALS_DISPLAYED)
    lcd_outdezAtt(x , y, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
    lcd_outdezAtt(x , y, (g_ppmIns[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
  }

  if (edit) {
    if (event==EVT_KEY_FIRST(KEY_MENU)){
      memcpy(g_eeGeneral.trainer.calib, g_ppmIns, sizeof(g_eeGeneral.trainer.calib));
      eeDirty(EE_GENERAL);
      beepKey();
    }
  }
}

void menuProcDiagVers(uint8_t event)
{
  SIMPLE_MENU("VERSION", menuTabDiag, e_Vers, 1);

  lcd_puts_P(0, 2*FH,stamp4 );
  lcd_puts_P(0, 3*FH,stamp1 );
  lcd_puts_P(0, 4*FH,stamp5 );
  lcd_puts_P(0, 5*FH,stamp2 );
  lcd_puts_P(0, 6*FH,stamp3 );
  lcd_puts_P(0, 7*FH,PSTR("EEPROM v"));
  lcd_outdezAtt(8*FW, 7*FH, g_eeGeneral.myVers, LEFT);
}

void menuProcDiagKeys(uint8_t event)
{
  SIMPLE_MENU("DIAG", menuTabDiag, e_Keys, 1);

  for(uint8_t i=0; i<9; i++)
  {
    uint8_t y=i*FH; //+FH;
    if(i>(SW_ID0-SW_BASE_DIAG)) y-=FH; //overwrite ID0
    bool t=keyState((EnumKeys)(SW_BASE_DIAG+i));
    putsSwitches(8*FW, y, i+1, 0); //ohne off,on
    lcd_putcAtt(11*FW+2, y, t+'0', t ? INVERS : 0);
  }

  for(uint8_t i=0; i<6; i++)
  {
    uint8_t y=(5-i)*FH+2*FH;
    bool t=keyState((EnumKeys)(KEY_MENU+i));
    lcd_putsn_P(0, y, PSTR(" Menu Exit Down   UpRight Left")+5*i, 5);
    lcd_putcAtt(5*FW+2, y, t+'0', t);
  }

  lcd_putsn_P(14*FW, 3*FH, PSTR("Trim- +"), 7);
  for(uint8_t i=0; i<4; i++)
  {
    uint8_t y=i*FH+FH*4;
    lcd_img(14*FW, y, sticks,i,0);
    bool tm=keyState((EnumKeys)(TRM_BASE+2*i));
    bool tp=keyState((EnumKeys)(TRM_BASE+2*i+1));
    lcd_putcAtt(18*FW,  y, tm+'0',tm ? INVERS : 0);
    lcd_putcAtt(20*FW,  y, tp+'0',tp ? INVERS : 0);
  }
}

void menuProcDiagAna(uint8_t event)
{
  SIMPLE_MENU("ANA", menuTabDiag, e_Ana, 2);

  int8_t  sub    = m_posVert ;

  for(uint8_t i=0; i<8; i++)
  {
    uint8_t y=i*FH;
    lcd_putsn_P( 4*FW, y,PSTR("A1A2A3A4A5A6A7A8")+2*i,2);
    lcd_outhex4( 8*FW, y,anaIn(i));
    if(i<7)  lcd_outdez8(17*FW, y, (int32_t)calibratedStick[i]*100/1024);
    if(i==7) putsVolts(17*FW, y, g_vbat100mV, (sub==1 ? INVERS : 0));
  }
  // lcd_outdezAtt( 21*FW, 3*FH, g_eeGeneral.vBatCalib, 0) ;
  // lcd_outdezAtt( 21*FW, 4*FH, abRunningAvg, 0) ;
  // Display raw BandGap result (debug)
  lcd_putsn_P( 19*FW, 5*FH,PSTR("BG"),2) ;
  lcd_outdezAtt(21*FW, 6*FH, BandGap, 0);
  lcd_outdezAtt(21*FW, 7*FH, anaIn(7)*35/512, PREC1);

  if(sub==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatCalib, -127, 127);

}

void menuProcDiagCalib(uint8_t event)
{
  SIMPLE_MENU("CALIBRATION", menuTabDiag, e_Calib, 4);

  int8_t  sub    = m_posVert ;
  static int16_t midVals[7];
  static int16_t loVals[7];
  static int16_t hiVals[7];

  for(uint8_t i=0; i<7; i++) { //get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    loVals[i] = min(vt,loVals[i]);
    hiVals[i] = max(vt,hiVals[i]);
    //if(i>=4) midVals[i] = (loVals[i] + hiVals[i])/2;
  }

  switch(event)
  {
    case EVT_ENTRY:
      for(uint8_t i=0; i<7; i++) loVals[i] = 15000;
      break;
    case EVT_KEY_BREAK(KEY_DOWN): // !! achtung sub schon umgesetzt
      switch(sub)
      {
        case 2: //get mid
          for(uint8_t i=0; i<7; i++)midVals[i] = anaIn(i);
          beepKey();
          break;
        case 3:
          for(uint8_t i=0; i<7; i++)
            if(abs(loVals[i]-hiVals[i])>50) {
              g_eeGeneral.calibMid[i]  = midVals[i];
              int16_t v = midVals[i] - loVals[i];
              g_eeGeneral.calibSpanNeg[i] = v - v/64;
              v = hiVals[i] - midVals[i];
              g_eeGeneral.calibSpanPos[i] = v - v/64;
            }
          int16_t sum=0;
          for(uint8_t i=0; i<12;i++) sum+=g_eeGeneral.calibMid[i];
          g_eeGeneral.chkSum = sum;
          eeDirty(EE_GENERAL); //eeWriteGeneral();
          beepKey();
          break;
      }
      break;
  }
  for(uint8_t i=1; i<4; i++)
  {
    uint8_t y=i*FH+FH;
    lcd_putsnAtt( 0, y,PSTR("SetMid SetSpanDone   ")+7*(i-1),7,
                    sub==i ? INVERS : 0);
  }
  for(uint8_t i=0; i<7; i++)
  {
    uint8_t y=i*FH;
    lcd_puts_P( 11*FW,   y+1*FH, PSTR("<    >"));
    lcd_outhex4( 8*FW-3, y+1*FH, sub==2 ? loVals[i]  : g_eeGeneral.calibSpanNeg[i]);
    lcd_outhex4(12*FW,   y+1*FH, sub==1 ? anaIn(i) : (sub==2 ? midVals[i] : g_eeGeneral.calibMid[i]));
    lcd_outhex4(17*FW,   y+1*FH, sub==2 ? hiVals[i]  : g_eeGeneral.calibSpanPos[i]);
  }

}
