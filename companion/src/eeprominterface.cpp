#include <stdio.h>
#include <list>
#include "eeprominterface.h"
#include "firmwares/er9x/er9xinterface.h"
#include "firmwares/th9x/th9xinterface.h"
#include "firmwares/gruvin9x/gruvin9xinterface.h"
#include "firmwares/opentx/open9xinterface.h"
#include "firmwares/ersky9x/ersky9xinterface.h"
#include "qsettings.h"

QString EEPROMWarnings;

void setEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == '\0')
      dst[i] = ' ';
    else
      break;
  }
}

void getEEPROMString(char *dst, const char *src, int size)
{
  memcpy(dst, src, size);
  dst[size] = '\0';
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == ' ')
      dst[i] = '\0';
    else
      break;
  }
}

int RawSource::getDecimals(const ModelData & Model)
{
  if(type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case TELEMETRY_SOURCE_A1:
      case TELEMETRY_SOURCE_A2:
        return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].type==0 ? 2: 0);
      case TELEMETRY_SOURCE_CELL:
        return 2;
      case TELEMETRY_SOURCE_CELLS_SUM:
      case TELEMETRY_SOURCE_VFAS:
      case TELEMETRY_SOURCE_CURRENT:
        return 1;
      default:
        return 0;
    }
  }
  return 0;
}

double RawSource::getMin(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        /*case TELEMETRY_SOURCE_NONE:
        case TELEMETRY_SOURCE_TX_BATT:
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 0; */
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].type==0) {
            return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/2550.0;
          }
          else {
            return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/255.0;
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return -500;
        case TELEMETRY_SOURCE_T1:
        case TELEMETRY_SOURCE_T2:
          return -30;
        default:
          return 0;
      }
      break;
    default:
      return (Model.extendedLimits ? -125 :-100);
  }
}

double RawSource::getMax(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 765;
        case TELEMETRY_SOURCE_RSSI_TX:
        case TELEMETRY_SOURCE_RSSI_RX:
          return 100;
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].type==0) {
            return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio-(Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/255.0)/10;
          } else {
            return Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio-(Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/255.0;
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return 1540;
        case TELEMETRY_SOURCE_RPM:
          return 12750;
        case TELEMETRY_SOURCE_FUEL:
          return 100;        
        case TELEMETRY_SOURCE_T1:
        case TELEMETRY_SOURCE_T2:
          return 225;
        case TELEMETRY_SOURCE_SPEED:
          return 944;
        case TELEMETRY_SOURCE_DIST:
          return 2040;
        case TELEMETRY_SOURCE_CELL:
          return 5.1;
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
          return 25.5;
        case TELEMETRY_SOURCE_CURRENT:
          return 127.5;
        case TELEMETRY_SOURCE_CONSUMPTION:
          return 5100;
        case TELEMETRY_SOURCE_POWER:
          return 1275;
        default:
          return 125;
      }
      break;
    default:
      return (Model.extendedLimits ? 125 :100);
  }
}

double RawSource::getOffset(const ModelData & Model)
{
  if (type==SOURCE_TYPE_TELEMETRY) {
    switch (index) {
      case TELEMETRY_SOURCE_A1:
      case TELEMETRY_SOURCE_A2:
        if (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].type==0) {
          return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/2550.0;
        }
        else {
          return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].offset*Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio)/255.0;
        }
      case TELEMETRY_SOURCE_ALT:
      case TELEMETRY_SOURCE_GPS_ALT:
        return 524;
      case TELEMETRY_SOURCE_RPM:
        return 6400;
      case TELEMETRY_SOURCE_FUEL:
        return 0;
      case TELEMETRY_SOURCE_T1:
      case TELEMETRY_SOURCE_T2:
        return 98;
      case TELEMETRY_SOURCE_SPEED:
        return 474;
      case TELEMETRY_SOURCE_DIST:
        return 1024;
      case TELEMETRY_SOURCE_CELL:
        return 2.56;
      default:
        return 0;
    }
  }
  return 0;
}

int RawSource::getRawOffset(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
        case TELEMETRY_SOURCE_RSSI_TX:
        case TELEMETRY_SOURCE_RSSI_RX:
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
        case TELEMETRY_SOURCE_FUEL:
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
        case TELEMETRY_SOURCE_CURRENT:
        case TELEMETRY_SOURCE_CONSUMPTION:
        case TELEMETRY_SOURCE_POWER:
          return 128;
        default:
          return 0;
      }
   default:
      return 0;
  }
}

double RawSource::getStep(const ModelData & Model)
{
  switch (type) {
    case SOURCE_TYPE_TELEMETRY:
      switch (index) {
        case TELEMETRY_SOURCE_TIMER1:
        case TELEMETRY_SOURCE_TIMER2:
          return 3;
        case TELEMETRY_SOURCE_A1:
        case TELEMETRY_SOURCE_A2:
          if (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].type==0) {
            return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio/2550.0);
          }
          else {
            return (Model.frsky.channels[index-TELEMETRY_SOURCE_A1].ratio/255.0);
          }
        case TELEMETRY_SOURCE_ALT:
        case TELEMETRY_SOURCE_GPS_ALT:
          return 8;
        case TELEMETRY_SOURCE_RPM:
          return 50;
        case TELEMETRY_SOURCE_SPEED:
          return 4;
        case TELEMETRY_SOURCE_DIST:
          return 8;
        case TELEMETRY_SOURCE_CELL:
          return 0.02;
        case TELEMETRY_SOURCE_CELLS_SUM:
        case TELEMETRY_SOURCE_VFAS:
          return 0.1;
        case TELEMETRY_SOURCE_CURRENT:
          return 0.5;
        case TELEMETRY_SOURCE_CONSUMPTION:
          return 20;
        case TELEMETRY_SOURCE_POWER:
          return 5;
        default:
          return 1;
      }
      break;
   default:
      return 1;
  }
}

QString RawSource::toString()
{
  static const QString sticks[]      = { QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail") };

  static const QString trims[]       = { QObject::tr("TrmR"), QObject::tr("TrmE"), QObject::tr("TrmT"), QObject::tr("TrmA")};

  static const QString pots9X[]      = { QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3") };

  static const QString potsX9D[]     = { QObject::tr("S1"), QObject::tr("S2"), QObject::tr("LS"), QObject::tr("RS") };

  static const QString rotary[]      = { QObject::tr("REa"), QObject::tr("REb") };

  static const QString switches9X[]  = { QObject::tr("3POS"),
                            QObject::tr("THR"), QObject::tr("RUD"), QObject::tr("ELE"),
                            QObject::tr("AIL"), QObject::tr("GEA"), QObject::tr("TRN"),
                          };

  static const QString switchesX9D[] = { QObject::tr("SA"), QObject::tr("SB"), QObject::tr("SC"), QObject::tr("SD"),
                            QObject::tr("SE"), QObject::tr("SF"), QObject::tr("SG"), QObject::tr("SH"),
                          };

  static const QString telemetry[]   = { QObject::tr("Batt"), QObject::tr("Timer1"), QObject::tr("Timer2"),
                            (IS_TARANIS(GetEepromInterface()->getBoard()) ? QObject::tr("SWR") :  QObject::tr("Tx")), (IS_TARANIS(GetEepromInterface()->getBoard()) ? QObject::tr("RSSI") :  QObject::tr("Rx")), QObject::tr("A1"), QObject::tr("A2"), QObject::tr("Alt"), QObject::tr("Rpm"), QObject::tr("Fuel"), QObject::tr("T1"),
                            QObject::tr("T2"), QObject::tr("Speed"), QObject::tr("Dist"), QObject::tr("GPS Alt"), QObject::tr("Cell"), QObject::tr("Cels"), QObject::tr("Vfas"), QObject::tr("Curr"),
                            QObject::tr("Cnsp"), QObject::tr("Powr"), QObject::tr("AccX"), QObject::tr("AccY"), QObject::tr("AccZ"), QObject::tr("HDG "), QObject::tr("VSpd"), QObject::tr("A1-"),
                            QObject::tr("A2-"), QObject::tr("Alt-"), QObject::tr("Alt+"), QObject::tr("Rpm+"), QObject::tr("T1+"), QObject::tr("T2+"), QObject::tr("Spd+"), QObject::tr("Dst+"),
                            QObject::tr("Cur+"), QObject::tr("Pwr+"), QObject::tr("ACC "), QObject::tr("Time"),
                          };

  static const QString virtualSwitches[] = { QObject::tr("CS1"), QObject::tr("CS2"), QObject::tr("CS3"), QObject::tr("CS4"), QObject::tr("CS5"), QObject::tr("CS6"), QObject::tr("CS7"), QObject::tr("CS8"), QObject::tr("CS9"), QObject::tr("CSA"),
                            QObject::tr("CSB"), QObject::tr("CSC"), QObject::tr("CSD"), QObject::tr("CSE"), QObject::tr("CSF"), QObject::tr("CSG"), QObject::tr("CSH"), QObject::tr("CSI"), QObject::tr("CSJ"), QObject::tr("CSK"),
                            QObject::tr("CSL"), QObject::tr("CSM"), QObject::tr("CSN"), QObject::tr("CSO"), QObject::tr("CSP"), QObject::tr("CSQ"), QObject::tr("CSR"), QObject::tr("CSS"), QObject::tr("CST"), QObject::tr("CSU"),
                            QObject::tr("CSV"), QObject::tr("CSW")
                          };
  
  if (index<0) {
    return QObject::tr("----");
  }
  switch(type) {
    case SOURCE_TYPE_STICK:
      if (index < 4)
        return CHECK_IN_ARRAY(sticks, index);
      else
        return (IS_TARANIS(GetEepromInterface()->getBoard()) ? CHECK_IN_ARRAY(potsX9D, index-4) : CHECK_IN_ARRAY(pots9X, index-4));
    case SOURCE_TYPE_TRIM:
      return CHECK_IN_ARRAY(trims, index);
    case SOURCE_TYPE_ROTARY_ENCODER:
      return CHECK_IN_ARRAY(rotary, index);
    case SOURCE_TYPE_MAX:
      return QObject::tr("MAX");
    case SOURCE_TYPE_SWITCH:
      return (IS_TARANIS(GetEepromInterface()->getBoard()) ? CHECK_IN_ARRAY(switchesX9D, index) : CHECK_IN_ARRAY(switches9X, index));
    case SOURCE_TYPE_CUSTOM_SWITCH:
      return virtualSwitches[index];
    case SOURCE_TYPE_CYC:
      return QObject::tr("CYC%1").arg(index+1);
    case SOURCE_TYPE_PPM:
      return QObject::tr("PPM%1").arg(index+1);
    case SOURCE_TYPE_CH:
      if (index < GetEepromInterface()->getCapability(Outputs))
        return QObject::tr("CH%1%2").arg((index+1)/10).arg((index+1)%10);
      else
        return QObject::tr("X%1").arg(index-GetEepromInterface()->getCapability(Outputs)+1);
    case SOURCE_TYPE_TELEMETRY:
      return CHECK_IN_ARRAY(telemetry, index);
    case SOURCE_TYPE_GVAR:
      return QObject::tr("GV%1").arg(index+1);      
    default:
      return QObject::tr("----");
  }
}

QString SwitchUp(const char sw)
{
  const char result[] = {'S', sw, (char)0xE2, (char)0x86, (char)0x91, 0};
  return QString::fromUtf8(result);
}

QString SwitchDn(const char sw)
{
  const char result[] = {'S', sw, (char)0xE2, (char)0x86, (char)0x93, 0};
  return QString::fromUtf8(result);
}

QString RawSwitch::toString()
{
  static const QString switches9X[] = { QObject::tr("THR"), QObject::tr("RUD"), QObject::tr("ELE"),
                           QObject::tr("ID0"), QObject::tr("ID1"), QObject::tr("ID2"),
                           QObject::tr("AIL"), QObject::tr("GEA"), QObject::tr("TRN")
                         };

  static const QString switchesX9D[] = { SwitchUp('A'), QString::fromUtf8("SA-"), SwitchDn('A'),
                            SwitchUp('B'), QString::fromUtf8("SB-"), SwitchDn('B'),
                            SwitchUp('C'), QString::fromUtf8("SC-"), SwitchDn('C'),
                            SwitchUp('D'), QString::fromUtf8("SD-"), SwitchDn('D'),
                            SwitchUp('E'), QString::fromUtf8("SE-"), SwitchDn('E'),
                            SwitchUp('F'), SwitchDn('F'),
                            SwitchUp('G'), QString::fromUtf8("SG-"), SwitchDn('G'),
                            SwitchUp('H'), SwitchDn('H'),
                          };

  static const QString virtualSwitches[] = { QObject::tr("CS1"), QObject::tr("CS2"), QObject::tr("CS3"), QObject::tr("CS4"), QObject::tr("CS5"), QObject::tr("CS6"), QObject::tr("CS7"), QObject::tr("CS8"), QObject::tr("CS9"), QObject::tr("CSA"),
                                             QObject::tr("CSB"), QObject::tr("CSC"), QObject::tr("CSD"), QObject::tr("CSE"), QObject::tr("CSF"), QObject::tr("CSG"), QObject::tr("CSH"), QObject::tr("CSI"), QObject::tr("CSJ"), QObject::tr("CSK"),
                                             QObject::tr("CSL"), QObject::tr("CSM"), QObject::tr("CSN"), QObject::tr("CSO"), QObject::tr("CSP"), QObject::tr("CSQ"), QObject::tr("CSR"), QObject::tr("CSS"), QObject::tr("CST"), QObject::tr("CSU"),
                                             QObject::tr("CSV"), QObject::tr("CSW")
                                           };

  switch(type) {
    case SWITCH_TYPE_SWITCH:
      if (IS_TARANIS(GetEepromInterface()->getBoard()))
        return index > 0 ? CHECK_IN_ARRAY(switchesX9D, index-1) : QString("!") + CHECK_IN_ARRAY(switchesX9D, -index-1);
      else
        return index > 0 ? CHECK_IN_ARRAY(switches9X, index-1) : QString("!") + CHECK_IN_ARRAY(switches9X, -index-1);
    case SWITCH_TYPE_VIRTUAL:
      return index > 0 ? CHECK_IN_ARRAY(virtualSwitches, index-1) : QString("!") + CHECK_IN_ARRAY(virtualSwitches, -index-1);
    case SWITCH_TYPE_ON:
      return QObject::tr("ON");
    case SWITCH_TYPE_ONM:
      if (index==0) {
        return QObject::tr("ONE");
      }
      else if (index==1) {
        return QObject::tr("!ONE");
      }
      break;
    case SWITCH_TYPE_TRN:
      if (index==0) {
        if (IS_TARANIS(GetEepromInterface()->getBoard())) 
          return SwitchDn('H')+"s";
        else
          return QObject::tr("TRNs");
      } else if (index==1) {
        if (IS_TARANIS(GetEepromInterface()->getBoard())) 
          return SwitchDn('H')+"l";
        else
          return QObject::tr("TRNl");
      }
      break;
    case SWITCH_TYPE_REA:
      if (index==0) {
        if (GetEepromInterface()->getBoard() == BOARD_SKY9X) 
          return QObject::tr("REAs");
      } else if (index==1) {
        if (GetEepromInterface()->getBoard() == BOARD_SKY9X) 
          return QObject::tr("REAl");
      }
      break;
    case SWITCH_TYPE_OFF:
      return QObject::tr("OFF");
    case SWITCH_TYPE_MOMENT_SWITCH:
      if (IS_TARANIS(GetEepromInterface()->getBoard()))
        return index > 0 ? CHECK_IN_ARRAY(switchesX9D, index-1)+QString("t") : QString("!")+CHECK_IN_ARRAY(switchesX9D, -index-1)+QString("t");
      else
        return index > 0 ? CHECK_IN_ARRAY(switches9X, index-1)+QString("t") : QString("!")+CHECK_IN_ARRAY(switches9X, -index-1)+QString("t");
    case SWITCH_TYPE_MOMENT_VIRTUAL:
      return index > 0 ? CHECK_IN_ARRAY(virtualSwitches, index-1)+QString("t") : QString("!")+CHECK_IN_ARRAY(virtualSwitches, -index-1)+QString("t");
    default:
      break;
  }

  return QObject::tr("----");
}

GeneralSettings::GeneralSettings()
{
  memset(this, 0, sizeof(GeneralSettings));
  contrast  = 25;
  vBatWarn  = 90;
  for (int i=0; i<(NUM_STICKS+C9X_NUM_POTS ); ++i) {
    calibMid[i]     = 0x200;
    calibSpanNeg[i] = 0x180;
    calibSpanPos[i] = 0x180;
  }
  QSettings settings("companion9x", "companion9x");
  templateSetup = settings.value("default_channel_order", 0).toInt();
  stickMode = settings.value("default_mode", 1).toInt();
  int profile_id = settings.value("ActiveProfile", 0).toInt();
  if (profile_id>0) {
    settings.beginGroup("Profiles");
    QString profile=QString("profile%1").arg(profile_id);
    settings.beginGroup(profile);
    QString t_calib=settings.value("StickPotCalib","").toString();
    int potsnum=GetEepromInterface()->getCapability(Pots);
    if (t_calib.isEmpty()) {
      settings.endGroup();
      settings.endGroup();
      return;
    } else {
      QString t_trainercalib=settings.value("TrainerCalib","").toString();
      int8_t t_vBatCalib=(int8_t)settings.value("VbatCalib", vBatCalib).toInt();
      int8_t t_currentCalib=(int8_t)settings.value("currentCalib", currentCalib).toInt();
      int8_t t_PPM_Multiplier=(int8_t)settings.value("PPM_Multiplier", PPM_Multiplier).toInt();
      uint8_t t_stickMode=(uint8_t)settings.value("GSStickMode", stickMode).toUInt();
      uint8_t t_vBatWarn=(uint8_t)settings.value("vBatWarn",vBatWarn).toUInt();
      QString t_DisplaySet=settings.value("Display","").toString();
      QString t_BeeperSet=settings.value("Beeper","").toString();
      QString t_HapticSet=settings.value("Haptic","").toString();
      QString t_SpeakerSet=settings.value("Speaker","").toString();
      QString t_CountrySet=settings.value("countryCode","").toString();
      settings.endGroup();
      settings.endGroup();

      if ((t_calib.length()==(NUM_STICKS+potsnum)*12) && (t_trainercalib.length()==16)) {
        QString Byte;
        int16_t byte16;
        bool ok;
        for (int i=0; i<(NUM_STICKS+potsnum); i++) {
          Byte=t_calib.mid(i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            calibMid[i]=byte16;
          Byte=t_calib.mid(4+i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            calibSpanNeg[i]=byte16;
          Byte=t_calib.mid(8+i*12,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            calibSpanPos[i]=byte16;
        }
        for (int i=0; i<4; i++) {
          Byte=t_trainercalib.mid(i*4,4);
          byte16=(int16_t)Byte.toInt(&ok,16);
          if (ok)
            trainer.calib[i]=byte16;
        }
        currentCalib=t_currentCalib;
        vBatCalib=t_vBatCalib;
        vBatWarn=t_vBatWarn;
        PPM_Multiplier=t_PPM_Multiplier;
        stickMode = t_stickMode;
      }
      if ((t_DisplaySet.length()==6) && (t_BeeperSet.length()==4) && (t_HapticSet.length()==6) && (t_SpeakerSet.length()==6)) {
        uint8_t byte8u;
        int8_t byte8;
        bool ok;
        byte8=(int8_t)t_DisplaySet.mid(0,2).toInt(&ok,16);
        if (ok)
          optrexDisplay=(byte8==1 ? true : false);
        byte8u=(uint8_t)t_DisplaySet.mid(2,2).toUInt(&ok,16);
        if (ok)
          contrast=byte8u;
        byte8u=(uint8_t)t_DisplaySet.mid(4,2).toUInt(&ok,16);
        if (ok)
          backlightBright=byte8u;
        byte8=(int8_t)t_BeeperSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          beeperMode=(BeeperMode)byte8;
        byte8=(int8_t)t_BeeperSet.mid(2,2).toInt(&ok,16);
        if (ok)
          beeperLength=byte8;
        byte8=(int8_t)t_HapticSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          hapticMode=(BeeperMode)byte8;
        byte8u=(uint8_t)t_HapticSet.mid(2,2).toUInt(&ok,16);
        if (ok)
          hapticStrength=byte8u;
        byte8=(int8_t)t_HapticSet.mid(4,2).toInt(&ok,16);
        if (ok)
          hapticLength=byte8;
        byte8u=(uint8_t)t_SpeakerSet.mid(0,2).toUInt(&ok,16);
        if (ok)
          speakerMode=byte8u;
        byte8u=(uint8_t)t_SpeakerSet.mid(2,2).toUInt(&ok,16);
        if (ok)
          speakerPitch=byte8u;
        byte8u=(uint8_t)t_SpeakerSet.mid(4,2).toUInt(&ok,16);
        if (ok)
          speakerVolume=byte8u;
        if (t_CountrySet.length()==6) {
          byte8u=(uint8_t)t_CountrySet.mid(0,2).toUInt(&ok,16);
          if (ok)
            countryCode=byte8u;
          byte8u=(uint8_t)t_CountrySet.mid(2,2).toUInt(&ok,16);
          if (ok)
            imperial=byte8u;
          QString chars=t_CountrySet.mid(4,2);
          ttsLanguage[0]=chars[0].toAscii();
          ttsLanguage[1]=chars[1].toAscii();
        }      
      }
    }
  }
}

ModelData::ModelData()
{
  clear();
}

void ModelData::clearInputs()
{
  for (int i=0; i<C9X_MAX_EXPOS; i++)
    expoData[i].clear();
}

void ModelData::clearMixes()
{
  for (int i=0; i<C9X_MAX_MIXERS; i++)
    mixData[i].clear();
}

void ModelData::clear()
{
  memset(this, 0, sizeof(ModelData));
  moduleData[0].channelsCount = 8;
  moduleData[1].channelsStart = 0;
  moduleData[1].channelsCount = 8;
  moduleData[0].ppmDelay = 300;
  moduleData[1].ppmDelay = 300;
  moduleData[2].ppmDelay = 300;
  int board=GetEepromInterface()->getBoard();
  if (IS_TARANIS(board)) {
    moduleData[0].protocol=PXX_XJT_X16;
    moduleData[1].protocol=OFF;
  } else {
    moduleData[0].protocol=PPM;
    moduleData[1].protocol=OFF;      
  }
  for (int i=0; i<C9X_MAX_PHASES; i++)
    phaseData[i].clear();
  clearInputs();
  clearMixes();
  for(int i=0; i<4; i++){
    mixData[i].destCh = i+1;
    mixData[i].srcRaw = RawSource(SOURCE_TYPE_STICK, i);
    mixData[i].weight = 100;
  }
  for (int i=0; i<C9X_NUM_CHNOUT; i++)
    limitData[i].clear();
  for (int i=0; i<NUM_STICKS; i++)
    expoData[i].clear();
  for (int i=0; i<C9X_NUM_CSW; i++)
    customSw[i].clear();
  bool custom = GetEepromInterface()->getCapability(CustomCurves);
  for (int i=0; i<C9X_MAX_CURVES; i++) {
    if (!custom && i>=8)
      curves[i].clear(9);
    else
      curves[i].clear(5);
  }

  swashRingData.clear();
  frsky.clear();
}

bool ModelData::isempty()
{
  return !used;
}

void ModelData::setDefault(uint8_t id)
{
  clear();
  used = true;
  sprintf(name, "MODEL%02d", id+1);
}

unsigned int ModelData::getTrimFlightPhase(uint8_t idx, int8_t phase)
{
  // if (phase == -1) phase = getFlightPhase();

  for (uint8_t i=0; i<C9X_MAX_PHASES; i++) {
    if (phase == 0 || phaseData[phase].trimRef[idx] < 0) return phase;
    phase = phaseData[phase].trimRef[idx];
  }
  return 0;
}

void ModelData::removeGlobalVar(int & var)
{
  if (var >= 126 && var <= 130)
    var = phaseData[0].gvars[var-126];
  else if (var <= -126 && var >= -130)
    var = - phaseData[0].gvars[-126-var];
}

ModelData ModelData::removeGlobalVars()
{
  ModelData result = *this;

  for (int i=0; i<C9X_MAX_MIXERS; i++) {
    removeGlobalVar(mixData[i].weight);
    removeGlobalVar(mixData[i].differential);
    removeGlobalVar(mixData[i].sOffset);
  }

  for (int i=0; i<C9X_MAX_EXPOS; i++) {
    removeGlobalVar(expoData[i].weight);
    removeGlobalVar(expoData[i].expo);
  }

  return result;
}

QList<EEPROMInterface *> eepromInterfaces;
void RegisterEepromInterfaces()
{
  QSettings settings("companion9x", "companion9x");
  int rev4a = settings.value("rev4asupport",0).toInt();
  eepromInterfaces.push_back(new Open9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_M128));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_SKY9X));
  eepromInterfaces.push_back(new Open9xInterface(BOARD_TARANIS));
  if (rev4a)
    eepromInterfaces.push_back(new Open9xInterface(BOARD_TARANIS_REV4a));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_STOCK));
  eepromInterfaces.push_back(new Gruvin9xInterface(BOARD_GRUVIN9X));
  eepromInterfaces.push_back(new Ersky9xInterface());
  eepromInterfaces.push_back(new Th9xInterface());
  eepromInterfaces.push_back(new Er9xInterface());
}

QList<FirmwareInfo *> firmwares;
FirmwareVariant default_firmware_variant;
FirmwareVariant current_firmware_variant;

const char * ER9X_STAMP = "http://er9x.googlecode.com/svn/trunk/src/stamp-er9x.h";
const char * ERSKY9X_STAMP = "http://ersky9x.googlecode.com/svn/trunk/src/stamp-ersky9x.h";

void RegisterFirmwares()
{
  firmwares.push_back(new FirmwareInfo("th9x", QObject::tr("th9x"), new Th9xInterface(), "http://th9x.googlecode.com/svn/trunk/%1.bin", "http://th9x.googlecode.com/svn/trunk/src/stamp-th9x.h"));

  firmwares.push_back(new FirmwareInfo("er9x", QObject::tr("er9x"), new Er9xInterface(), "http://er9x.googlecode.com/svn/trunk/%1.hex", ER9X_STAMP));
  FirmwareInfo * er9x = firmwares.last();

  Option er9x_options[] = { { "noht", "", 0 }, { "frsky", "", 0 }, { "frsky-noht", "", 0 }, { "jeti", "", 0 }, { "ardupilot", "", 0 }, { "nmea", "", 0 }, { NULL } };
  er9x->addOptions(er9x_options);
  er9x->addOption("noht");

  RegisterOpen9xFirmwares();
#ifndef __APPLE__
  firmwares.push_back(new FirmwareInfo("ersky9x", QObject::tr("ersky9x"), new Ersky9xInterface(), "http://ersky9x.googlecode.com/svn/trunk/ersky9x_rom.bin", ERSKY9X_STAMP));
#endif
  default_firmware_variant = GetFirmwareVariant("opentx-stock-heli-templates-en");

  RegisterEepromInterfaces();
}

bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->load(radioData, eeprom, size))
      return true;
  }

  return false;
}

bool LoadBackup(RadioData &radioData, uint8_t *eeprom, int size, int index)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->loadBackup(radioData, eeprom, size, index))
      return true;
  }

  return false;
}


bool LoadEepromXml(RadioData &radioData, QDomDocument &doc)
{
  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    if (eepromInterface->loadxml(radioData, doc))
      return true;
  }

  return false;
}


FirmwareVariant GetFirmwareVariant(QString id)
{
  FirmwareVariant result;

  foreach(FirmwareInfo * firmware, firmwares) {
    
    if (id.contains(firmware->id+"-") || (!id.contains("-") && id.contains(firmware->id))) {
      result.id = id;
      result.firmware = firmware;
      result.variant = firmware->getVariant(id);
      return result;
    }
  }

  return default_firmware_variant;
}

void FirmwareInfo::addOption(const char *option, QString tooltip, uint32_t variant)
{
  Option options[] = { { option, tooltip, variant }, { NULL } };
  addOptions(options);
}

unsigned int FirmwareInfo::getVariant(const QString & variantId)
{
  unsigned int variant = variantBase;
  QStringList options = variantId.mid(id.length()+1).split("-", QString::SkipEmptyParts);
  foreach(QString option, options) {
    foreach(QList<Option> group, opts) {
      foreach(Option opt, group) {
        if (opt.name == option) {
          variant += opt.variant;
        }
      }
    }
  }
  return variant;
}

void FirmwareInfo::addLanguage(const char *lang)
{
  languages.push_back(lang);
}

void FirmwareInfo::addTTSLanguage(const char *lang)
{
  ttslanguages.push_back(lang);
}

void FirmwareInfo::addOptions(Option options[])
{
  QList<Option> opts;
  for (int i=0; options[i].name; i++) {
    opts.push_back(options[i]);
  }
  this->opts.push_back(opts);
}
