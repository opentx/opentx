/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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

#include "open9x.h"
#include "templates.h"

RlcFile theFile;  //used for any file operation

void generalDefault()
{
  memset(&g_eeGeneral,0,sizeof(g_eeGeneral));
  g_eeGeneral.myVers   =  EEPROM_VER;
  g_eeGeneral.currModel=  0;
  g_eeGeneral.contrast = 25;
  g_eeGeneral.vBatWarn = 90;
#ifdef DEFAULTMODE1
  g_eeGeneral.stickMode=  0; // default to mode 1
#else
  g_eeGeneral.stickMode=  2; // default to mode 2
#endif
  for (int i = 0; i < 7; ++i) {
    g_eeGeneral.calibMid[i]     = 0x200;
    g_eeGeneral.calibSpanNeg[i] = 0x180;
    g_eeGeneral.calibSpanPos[i] = 0x180;
  }
  g_eeGeneral.chkSum = (0x200 * 7) + (0x180 * 5);
}

bool eeLoadGeneral()
{
  theFile.openRlc(FILE_GENERAL);
  if (theFile.readRlc((uint8_t*)&g_eeGeneral, 1) == 1 && g_eeGeneral.myVers == EEPROM_VER) {
    theFile.openRlc(FILE_GENERAL); // TODO include this openRlc inside readRlc
    if (theFile.readRlc((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral)) <= sizeof(EEGeneral)) {
      uint16_t sum=0;
      for (int i=0; i<12;i++) sum += g_eeGeneral.calibMid[i];
      if (g_eeGeneral.chkSum == sum) {
        return true;
      }
    }
  }
  return false;
}

#ifndef TEMPLATES
inline void applyDefaultTemplate()
{
  for (int i=0; i<NUM_STICKS; i++) {
    MixData *md = mixaddress(i);
    md->destCh = i+1;
    md->weight = 100;
    md->srcRaw = channel_order(i+1);
  }

  STORE_MODELVARS;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));
  applyDefaultTemplate();
}

uint16_t eeLoadModelName(uint8_t id, char *name)
{
  memset(name, 0, sizeof(g_model.name));
  if (id<MAX_MODELS) {
    theFile.openRlc(FILE_MODEL(id));
    if (theFile.readRlc((uint8_t*)name, sizeof(g_model.name)) == sizeof(g_model.name)) {
      return theFile.size();
    }
  }
  return 0;
}

bool eeModelExists(uint8_t id)
{
    return EFile::exists(FILE_MODEL(id));
}

void eeLoadModel(uint8_t id)
{
  if(id<MAX_MODELS)
  {
    theFile.openRlc(FILE_MODEL(id));
    uint16_t sz = theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));

#ifdef SIMU
    if (sz > 0 && sz != sizeof(g_model)) {
      printf("Model data read=%d bytes vs %d bytes\n", sz, (int)sizeof(ModelData));
    }
#endif

    if (sz == 0) {
      // alert("Error Loading Model");
      modelDefault(id);
      eeCheck(true);
    }

    // TODO optim: resetAll()
    // TODO s_traceCnt to be reset?
    resetTimer(0);
    resetTimer(1);
    resetProto();
#ifdef LOGS
    initLogs();
#endif
  }
}

void resetProto() // TODO inline this if !DSM2_SERIAL
{
#if defined(DSM2_SERIAL)
  if (g_model.protocol == PROTO_DSM2) {
    cli();
#if defined(FRSKY)
    DSM2_Init();
#endif
    sei();
  }
  else {
    cli();
#if defined(FRSKY)
    FRSKY_Init();
#else
    DSM2_Done();
#endif
    sei();
#if defined(FRSKY)
    FRSKY_setModelAlarms();
#endif
  }
#elif defined(FRSKY)
  resetTelemetry();
  FRSKY_setModelAlarms();
#endif
}

int8_t eeFindEmptyModel(uint8_t id, bool down)
{
  int8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!EFile::exists(FILE_MODEL(i))) break;
    if (i == id) return -1; // no free space in directory left
  }
  return i;
}

void eeReadAll()
{
  if(!EeFsOpen() ||
     EeFsck() < 0 ||
     !eeLoadGeneral())
  {
    alert(STR_BADEEPROMDATA, true);
    message(STR_EEPROMFORMATTING);
    EeFsFormat();
    //alert(PSTR("format ok"));
    generalDefault();
    //alert(PSTR("default ok"));

    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL,(uint8_t*)&g_eeGeneral,sizeof(EEGeneral), true);

    modelDefault(0);
    //alert(PSTR("modef ok"));
    theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
    //alert(PSTR("modwrite ok"));
  }

  eeLoadModel(g_eeGeneral.currModel);
}


uint8_t  s_eeDirtyMsk;
void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
}

void eeCheck(bool immediately)
{
  if (immediately) {
    eeFlush();
  }
  if (s_eeDirtyMsk & EE_GENERAL) {
    s_eeDirtyMsk -= EE_GENERAL;
    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), immediately);
    if (!immediately) return;
  }
  if (s_eeDirtyMsk & EE_MODEL) {
    s_eeDirtyMsk = 0;
    theFile.writeRlc(FILE_MODEL(g_eeGeneral.currModel), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), immediately);
  }
}
