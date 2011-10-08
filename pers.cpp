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

#ifdef TRANSLATIONS
#include "eeprom_v4.h"
#include "eeprom_v3.h"
#endif

#include "gruvin9x.h"
#include "templates.h"

RlcFile theFile;  //used for any file operation

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

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
  int16_t sum=0;
  for(int i=0; i<12;i++) sum+=g_eeGeneral.calibMid[i];
  g_eeGeneral.chkSum = sum;
}

#ifdef TRANSLATIONS
uint8_t Translate()
{
  if (g_eeGeneral.myVers == 0) {
    if (theFile.readRlc1((uint8_t*)&g_eeGeneral, 1) != 1)
      return 0;
    theFile.openRlc(FILE_GENERAL);
  }

  if (g_eeGeneral.myVers == EEPROM_VER_r584 || (g_eeGeneral.myVers >= EEPROM_ER9X_MIN && g_eeGeneral.myVers <= EEPROM_ER9X_MAX)) {
    alert(g_eeGeneral.myVers == EEPROM_VER_r584 ? PSTR("EEprom Data v3") : PSTR("EEprom Data Er9x"), true);
    message(PSTR("EEPROM Converting"));
    theFile.readRlc1((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral));
    memset(&g_eeGeneral.frskyRssiAlarms, 0 , sizeof(g_eeGeneral.frskyRssiAlarms));
    if (g_eeGeneral.myVers == EEPROM_VER_r584) {
      // previous version had only 6 custom switches, OFF and ON values have to be shifted 6
      if (g_eeGeneral.lightSw == MAX_SWITCH-6)
        g_eeGeneral.lightSw += 6;
      if (g_eeGeneral.lightSw == -MAX_SWITCH+6)
        g_eeGeneral.lightSw -= 6;
    }
    else {
      g_eeGeneral.inactivityTimer += 10;
    }
    g_eeGeneral.view = 0; // will not translate the view index
    EEPROM_V3::EEGeneral *old = (EEPROM_V3::EEGeneral *)&g_eeGeneral;
    g_eeGeneral.disableMemoryWarning = old->disableMemoryWarning;
    g_eeGeneral.switchWarning = old->disableSwitchWarning ? 0 : -1;
    for (uint8_t i=0; i<4; i++) {
      g_eeGeneral.trainer.mix[i].srcChn = old->trainer.mix[i].srcChn;
      g_eeGeneral.trainer.mix[i].mode = old->trainer.mix[i].mode;
      g_eeGeneral.trainer.mix[i].studWeight = old->trainer.mix[i].studWeight * 13 / 4;
    }
    for (uint8_t id=0; id<MAX_MODELS; id++) {
      theFile.openRlc(FILE_MODEL(id));
      uint16_t sz = theFile.readRlc1((uint8_t*)&g_model, sizeof(EEPROM_V4::ModelData));
      if(sz > 0) {
        EEPROM_V4::ModelData *v4 = (EEPROM_V4::ModelData *)&g_model;
        EEPROM_V3::ModelData *v3 = (EEPROM_V3::ModelData *)&g_model;
        SwashRingData swashR;
        swashR.invertELE = v4->swashInvertELE;
        swashR.invertAIL = v4->swashInvertAIL;
        swashR.invertCOL = v4->swashInvertCOL;
        swashR.type = v4->swashType;
        swashR.collectiveSource = v4->swashCollectiveSource;
        swashR.value = v4->swashRingValue;
        int8_t trims[4];
        memcpy(&trims[0], &v3->trim[0], 4);
        int8_t trimSw = v3->trimSw;
        for (uint8_t i=0; i<10; i++)
          g_model.name[i] = char2idx(g_model.name[i]);
        g_model.timer1.mode = v3->tmrMode;
        g_model.timer1.val = v3->tmrVal;
        g_model.timer1.persistent = 0;
        g_model.timer1.dir = v3->tmrDir;
        g_model.protocol = v3->protocol;
        g_model.ppmNCH = v3->ppmNCH;
        g_model.thrTrim = v3->thrTrim;
        g_model.thrExpo = v3->thrExpo;
        g_model.trimInc = v3->trimInc;
        g_model.spare1 = 0;
        g_model.pulsePol = v3->pulsePol;
        if (g_eeGeneral.myVers == EEPROM_VER_r584)
          g_model.extendedLimits = 0;
        else
          g_model.extendedLimits = v4->extendedLimits;
        g_model.extendedTrims = 0;
        g_model.spare2 = 0;
        g_model.ppmDelay = v3->ppmDelay;
        g_model.beepANACenter = v3->beepANACenter;
        g_model.timer2.mode = 0;
        g_model.timer2.val = 0;
        g_model.timer2.persistent = 0;
        g_model.timer2.dir = 0;
        for (uint8_t i=0; i<MAX_MIXERS; i++) {
          memmove(&g_model.mixData[i], &v3->mixData[i], sizeof(MixData)); // MixData size changed!
          g_model.mixData[i].mixWarn = g_model.mixData[i].phase;
          g_model.mixData[i].phase = 0;
        }
        assert((char *)&g_model.limitData[0] < (char *)&v3->limitData[0]);
        memmove(&g_model.limitData[0], &v3->limitData[0], sizeof(LimitData)*NUM_CHNOUT);
        assert((char *)&g_model.expoData[0] < (char *)v3->expoData);
        EEPROM_V4::ExpoData expo4[4];
        memcpy(&expo4[0], &v4->expoData[0], sizeof(expo4));
        memset(&g_model.expoData[0], 0, sizeof(expo4));
        uint8_t e = 0;
        for (uint8_t ch=0; ch<4 && e<MAX_EXPOS; ch++) {
          for (int8_t dr=2; dr>=0 && e<MAX_EXPOS; dr--) {
            if ((dr==2 && !expo4[ch].drSw1) ||
                (dr==1 && !expo4[ch].drSw2) ||
                (dr==0 && !expo4[ch].expo[0][0][0] && !expo4[ch].expo[0][0][1] && !expo4[ch].expo[0][1][0] && !expo4[ch].expo[2][1][1])) continue;
            g_model.expoData[e].swtch = (dr == 0 ? expo4[ch].drSw1 : (dr == 1 ? expo4[ch].drSw2 : 0));
            g_model.expoData[e].chn = ch;
            g_model.expoData[e].expo = expo4[ch].expo[dr][0][0];
            g_model.expoData[e].weight = 100 + expo4[ch].expo[dr][1][0];
            if (expo4[ch].expo[dr][0][0] == expo4[ch].expo[dr][0][1] && expo4[ch].expo[dr][1][0] == expo4[ch].expo[dr][1][1]) {
              g_model.expoData[e++].mode = 3;
            }
            else {
              g_model.expoData[e].mode = 1;
              if (e < MAX_EXPOS-1) {
                g_model.expoData[e+1].swtch = g_model.expoData[e].swtch;
                g_model.expoData[++e].chn = ch;
                g_model.expoData[e].mode = 2;
                g_model.expoData[e].expo = expo4[ch].expo[dr][0][1];
                g_model.expoData[e++].weight = 100 + expo4[ch].expo[dr][1][1];
              }
            }
          }
        }
        assert((char *)&g_model.curves5[0][0] < (char *)&v3->curves5[0][0]);
        memmove(&g_model.curves5[0][0], &v3->curves5[0][0], 5*MAX_CURVE5);
        assert((char *)&g_model.curves9[0][0] < (char *)&v3->curves9[0][0]);
        memmove(&g_model.curves9[0][0], &v3->curves9[0][0], 9*MAX_CURVE9);
        if (g_eeGeneral.myVers == EEPROM_VER_r584) {
          memmove(&g_model.customSw[0], &v3->customSw[0], sizeof(CustomSwData)*6);
          memset(&g_model.customSw[6], 0, sizeof(CustomSwData)*6);
          memset(&g_model.safetySw[0], 0, sizeof(SafetySwData)*NUM_CHNOUT + sizeof(SwashRingData) + sizeof(FrSkyData));
        }
        else {
          assert((char *)&g_model.customSw[0] < (char *)&v4->customSw[0]);
          memmove(&g_model.customSw[0], &v4->customSw[0], sizeof(CustomSwData)*12);
          assert((char *)&g_model.safetySw[0] < (char *)&v4->safetySw[0]);
          memmove(&g_model.safetySw[0], &v4->safetySw[0], sizeof(SafetySwData)*NUM_CHNOUT);
          memcpy(&g_model.swashR, &swashR, sizeof(SwashRingData));
          for (uint8_t i=0; i<2; i++) {
            // TODO this conversion is bad
            // assert(&g_model.frsky.channels[i].ratio < &v4->frsky.channels[i].ratio);
            g_model.frsky.channels[i].ratio = v4->frsky.channels[i].ratio;
            g_model.frsky.channels[i].type = v4->frsky.channels[i].type;
            g_model.frsky.channels[i].offset = 0;
            g_model.frsky.channels[i].alarms_value[0] = v4->frsky.channels[i].alarms_value[0];
            g_model.frsky.channels[i].alarms_value[1] = v4->frsky.channels[i].alarms_value[1];
            g_model.frsky.channels[i].alarms_level = v4->frsky.channels[i].alarms_level;
            g_model.frsky.channels[i].alarms_greater = v4->frsky.channels[i].alarms_greater;
            g_model.frsky.channels[i].barMin = 0;
            g_model.frsky.channels[i].barMax = 0;
          }
        }
        memset(&g_model.phaseData[0], 0, sizeof(g_model.phaseData));
        memset(&g_model.funcSw[0], 0, sizeof(g_model.funcSw));
        if (trimSw) {
          g_model.funcSw[0].swtch = trimSw;
          g_model.funcSw[0].func = FUNC_INSTANT_TRIM;
        }
        for (uint8_t i=0; i<NUM_STICKS; i++)
          setTrimValue(0, i, trims[i]);
        theFile.writeRlc(FILE_MODEL(id), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), 200);
      }
    }
    g_eeGeneral.myVers = EEPROM_VER;
    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), 200);
    return sizeof(EEGeneral);
  }

  return 0;
}
#endif


bool eeLoadGeneral()
{
  theFile.openRlc(FILE_GENERAL);
  uint8_t sz = 0;

  if (theFile.readRlc((uint8_t*)&g_eeGeneral, 1) == 1) {
    theFile.openRlc(FILE_GENERAL); // TODO include this openRlc inside readRlc
    if (g_eeGeneral.myVers == EEPROM_VER) {
      sz = theFile.readRlc((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral));
    }
#ifdef TRANSLATIONS
    else {
      sz = Translate();
    }
#endif
  }

  if (sz == sizeof(EEGeneral)) {
    uint16_t sum=0;
    for(int i=0; i<12;i++) sum+=g_eeGeneral.calibMid[i];
    return g_eeGeneral.chkSum == sum;
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
    md->srcRaw = i+1;
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

    if (sz != sizeof(ModelData)) {
      // alert("Error Loading Model");
      modelDefault(id);
    }

    resetTimer1();
    resetTimer2();
#ifdef FRSKY
    resetTelemetry();
    FRSKY_setModelAlarms();
#endif
  }
}

int8_t eeDuplicateModel(uint8_t id, bool down)
{
  int8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!EFile::exists(FILE_MODEL(i))) break;
    if (i == id) return -1; // no free space in directory left
  }

  EFile theFile2;
  theFile2.openRd(FILE_MODEL(id));

#ifdef EEPROM_ASYNC_WRITE
  theFile.create(FILE_MODEL(i), FILE_TYP_MODEL, true);
#else
  theFile.create(FILE_MODEL(i), FILE_TYP_MODEL, 600);
#endif
  uint8_t buf[15];
  uint8_t len;
  while((len=theFile2.read(buf, 15)))
  {
    theFile.write(buf, len);
    if (errno() != 0) {
      return false;
    }
  }
  theFile.close();
  return i;
}

void eeReadAll()
{
  if(!EeFsOpen() ||
     EeFsck() < 0 ||
     !eeLoadGeneral()
  )
  {
    alert(PSTR("Bad EEprom Data"), true);
    message(PSTR("EEPROM Formatting"));
    EeFsFormat();
    //alert(PSTR("format ok"));
    generalDefault();
    //alert(PSTR("default ok"));

#ifdef EEPROM_ASYNC_WRITE
    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL,(uint8_t*)&g_eeGeneral,sizeof(EEGeneral), true);
#else
    uint16_t sz = theFile.writeRlc(FILE_GENERAL,FILE_TYP_GENERAL,(uint8_t*)&g_eeGeneral,sizeof(EEGeneral), 200);
    if(sz!=sizeof(EEGeneral)) alert(PSTR("genwrite error"));
#endif

    modelDefault(0);
    //alert(PSTR("modef ok"));
#ifdef EEPROM_ASYNC_WRITE
    theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
#else
    theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model),200);
#endif
    //alert(PSTR("modwrite ok"));
  }

  eeLoadModel(g_eeGeneral.currModel);
}


uint8_t  s_eeDirtyMsk;
#ifndef EEPROM_ASYNC_WRITE
static uint16_t s_eeDirtyTime10ms;
#define WRITE_DELAY_10MS 100
#endif
void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
#ifndef EEPROM_ASYNC_WRITE
  if (msk)
    s_eeDirtyTime10ms  = get_tmr10ms();
#endif
}

void eeCheck(bool immediately)
{
#ifdef EEPROM_ASYNC_WRITE
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
#else
  uint8_t msk  = s_eeDirtyMsk;
  if (!msk) return;
  if( !immediately && ((get_tmr10ms() - s_eeDirtyTime10ms) < WRITE_DELAY_10MS)) return;
  s_eeDirtyMsk = 0;

  if(msk & EE_GENERAL){
    if(theFile.writeRlc(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral,
                        sizeof(EEGeneral),20) == sizeof(EEGeneral))
    {
      EFile::swap(FILE_GENERAL,FILE_TMP);
    }else{
      if(errno()==ERR_TMO){
        s_eeDirtyMsk |= EE_GENERAL; //try again
        s_eeDirtyTime10ms = get_tmr10ms() - WRITE_DELAY_10MS;
      }else{
        alert(PSTR("EEPROM overflow"));
      }
    }
    //first finish GENERAL, then MODEL !!avoid Toggle effect
  }
  if(msk & EE_MODEL){
    if(theFile.writeRlc(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&g_model,
                        sizeof(g_model),20) == sizeof(g_model))
    {
      EFile::swap(FILE_MODEL(g_eeGeneral.currModel),FILE_TMP);
    }else{
      if(errno()==ERR_TMO){
        s_eeDirtyMsk |= EE_MODEL; //try again
        s_eeDirtyTime10ms = get_tmr10ms() - WRITE_DELAY_10MS;
      }else{
        alert(PSTR("EEPROM overflow"));
      }
    }
  }
  //beepWarn1();
#endif
}
