#include <QtGui>
#include "helpers.h"

QString getPhaseName(int val, char * phasename)
{
  if (!val) return "---";
  if (!phasename) {
    return QString(val < 0 ? "!" : "") + QObject::tr("FM%1").arg(abs(val) - 1);
  }
  else {
    QString phaseName;
    phaseName.append(phasename);
    if (phaseName.isEmpty()) {
      return QString(val < 0 ? "!" : "") + QObject::tr("FM%1").arg(abs(val) - 1);
    } else {
      return QString(val < 0 ? "!" : "") + phaseName;
    }
  }
}

QString getStickStr(int index)
{
  return RawSource(SOURCE_TYPE_STICK, index).toString();
}

void populateGvSourceCB(QComboBox *b, int value)
{
  QString strings[] = { QObject::tr("---"), QObject::tr("Rud Trim"), QObject::tr("Ele Trim"), QObject::tr("Thr Trim"), QObject::tr("Ail Trim"), QObject::tr("Rot Enc"), QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  b->clear();
  for (int i=0; i<= 12; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populateVoiceLangCB(QComboBox *b, QString language)
{
  QString strings[] = { QObject::tr("English"), QObject::tr("French"), QObject::tr("Italian"), QObject::tr("German"), QObject::tr("Czech"), QObject::tr("Slovak"), QObject::tr("Spanish"), QObject::tr("Portuguese"), QObject::tr("Swedish"), NULL};
  QString langcode[] = { "en", "fr", "it", "de", "cz", "sk", "es", "pt", "se", NULL};
  
  b->clear();
  for (int i=0; strings[i]!=NULL; i++) {
    b->addItem(strings[i],langcode[i]);
    if (language==langcode[i]) {
      b->setCurrentIndex(b->count()-1);
    }
  }
}

void populateTTraceCB(QComboBox *b, int value)
{
  const QString strings9x[] = { QObject::tr("THR"), QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3")};
  const QString stringstaranis[] = { QObject::tr("THR"), QObject::tr("S1"), QObject::tr("S2"), QObject::tr("LS"), QObject::tr("RS")};
  b->clear();
  if (IS_TARANIS(GetEepromInterface()->getBoard())) {
    for (int i=0; i< 5; i++) {
      b->addItem(stringstaranis[i]);
    }
  } else {
    for (int i=0; i< 4; i++) {
      b->addItem(strings9x[i]);
    }
  }
  int channels=(IS_ARM(GetEepromInterface()->getBoard()) ? 32 : 16);
  for (int i=1; i<= channels; i++) {
    b->addItem(QObject::tr("CH")+QString("%1").arg(i,2,10,QChar('0')));
  }
  b->setCurrentIndex(value);
}

void populateRotEncCB(QComboBox *b, int value, int renumber)
{
  QString strings[] = { QObject::tr("No"), QObject::tr("RotEnc A"), QObject::tr("Rot Enc B"), QObject::tr("Rot Enc C"), QObject::tr("Rot Enc D"), QObject::tr("Rot Enc E")};
  
  b->clear();
  for (int i=0; i<= renumber; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populateCustomScreenFieldCB(QComboBox *b, unsigned int value, bool last=false, int hubproto=0)
{
  int telem_hub[] = {0,0,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,0,0,2,2,1,1,1,1,1,1};
  b->clear();

  b->addItem(RawSource(SOURCE_TYPE_NONE, 0).toString());

  for (unsigned int i = 0; i < (last ? TELEMETRY_SOURCES_DISPLAY_COUNT : TELEMETRY_SOURCES_STATUS_COUNT); i++) {
    b->addItem(RawSource(SOURCE_TYPE_TELEMETRY, i).toString());
    if (!(i>=sizeof(telem_hub)/sizeof(int) || telem_hub[i]==0 || ((telem_hub[i]>=hubproto) && hubproto!=0))) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }

  if (value>=sizeof(telem_hub)/sizeof(int))
    b->setCurrentIndex(0);
  else if (telem_hub[value]==0 || ((telem_hub[value]>=hubproto) && hubproto!=0)) {
    b->setCurrentIndex(value);
  }

  b->setMaxVisibleItems(10);
}

QString getRepeatString(unsigned int val) 
{
  if (val==0) {
    return QObject::tr("No repeat");
  } else {
    unsigned int step = IS_ARM(GetEepromInterface()->getBoard()) ? 5 : 10;
    return QObject::tr("%1 sec").arg(step*val);
  }
}

QString getFuncName(unsigned int val)
{
  if (val < NUM_SAFETY_CHNOUT) {
    return QObject::tr("Safety %1").arg(RawSource(SOURCE_TYPE_CH, val).toString());
  }
  else if (val == FuncTrainer)
    return QObject::tr("Trainer");
  else if (val == FuncTrainerRUD)
    return QObject::tr("Trainer RUD");
  else if (val == FuncTrainerELE)
    return QObject::tr("Trainer ELE");
  else if (val == FuncTrainerTHR)
    return QObject::tr("Trainer THR");
  else if (val == FuncTrainerAIL)
    return QObject::tr("Trainer AIL");
  else if (val == FuncInstantTrim)
    return QObject::tr("Instant Trim");
  else if (val == FuncPlaySound)
    return QObject::tr("Play Sound");
  else if (val == FuncPlayHaptic)
    return QObject::tr("Play Haptic");
  else if (val == FuncReset)
    return QObject::tr("Reset");
  else if (val == FuncVario)
    return QObject::tr("Vario");
  else if (val == FuncPlayPrompt)
    return QObject::tr("Play Track");
  else if (val == FuncPlayBoth)
    return QObject::tr("Play Both");
  else if (val == FuncPlayValue)
    return QObject::tr("Play Value");
  else if (val == FuncLogs)
    return QObject::tr("Start Logs");
  else if (val == FuncVolume)
    return QObject::tr("Volume");
  else if (val == FuncBacklight)
    return QObject::tr("Backlight");
  else if (val == FuncBackgroundMusic)
    return QObject::tr("Background Music");
  else if (val == FuncBackgroundMusicPause)
    return QObject::tr("Background Music Pause");
  else if (val == FuncAdjustGV1)
    return QObject::tr("Adjust GV1");
  else if (val == FuncAdjustGV2)
    return QObject::tr("Adjust GV2");
  else if (val == FuncAdjustGV3)
    return QObject::tr("Adjust GV3");
  else if (val == FuncAdjustGV4)
    return QObject::tr("Adjust GV4");
  else if (val == FuncAdjustGV5)
    return QObject::tr("Adjust GV5");
  else {
    return QString("???"); // Highlight unknown functions with output of question marks.(BTW should not happen that we do not know what a function is)
  }
}

QString getCustomSwitchStr(CustomSwData * customSw, const ModelData & model)
{
  QString result = "";

  if (!customSw->func)
    return result;
  if (customSw->andsw!=0) {
    result +="( ";
  }
  switch (getCSFunctionFamily(customSw->func)) {
    case CS_FAMILY_TIMERS:
      result = QString("TIM(%1 , %2)").arg(ValToTim(customSw->val1)).arg(ValToTim(customSw->val2));
      break;     
    case CS_FAMILY_VOFS: {
      RawSource source = RawSource(customSw->val1);
      if (customSw->val1)
        result += source.toString();
      else
        result += "0";
      result.remove(" ");
      if (customSw->func == CS_FN_APOS || customSw->func == CS_FN_ANEG)
        result = "|" + result + "|";
      else if (customSw->func == CS_FN_DAPOS)
        result = "|d(" + result + ")|";
      else if (customSw->func == CS_FN_DPOS) result = "d(" + result + ")";
      if (customSw->func == CS_FN_APOS || customSw->func == CS_FN_VPOS || customSw->func == CS_FN_DAPOS || customSw->func == CS_FN_DPOS)
        result += " &gt; ";
      else if (customSw->func == CS_FN_ANEG || customSw->func == CS_FN_VNEG)
        result += " &lt; ";
      result += QString::number(source.getStep(model) * (customSw->val2 + source.getRawOffset(model)) + source.getOffset(model));
      break;
    }
    case CS_FAMILY_VBOOL:
      result = RawSwitch(customSw->val1).toString();
      switch (customSw->func) {
        case CS_FN_AND:
          result += " AND ";
          break;
        case CS_FN_OR:
          result += " OR ";
          break;
        case CS_FN_XOR:
          result += " XOR ";
          break;
        default:
          break;
      }
      result += RawSwitch(customSw->val2).toString();
      break;

    case CS_FAMILY_VCOMP:
      if (customSw->val1)
        result += RawSource(customSw->val1).toString();
      else
        result += "0";
      switch (customSw->func) {
        case CS_FN_EQUAL:
          result += " = ";
          break;
        case CS_FN_NEQUAL:
          result += " != ";
          break;
        case CS_FN_GREATER:
          result += " &gt; ";
          break;
        case CS_FN_LESS:
          result += " &lt; ";
          break;
        case CS_FN_EGREATER:
          result += " &gt;= ";
          break;
        case CS_FN_ELESS:
          result += " &lt;= ";
          break;
        default:
          break;
      }
      if (customSw->val2)
        result += RawSource(customSw->val2).toString();
      else
        result += "0";
      break;
  }

  if (customSw->andsw!=0) {
    result +=" ) AND ";
    result += RawSwitch(customSw->andsw).toString();
  }

  if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
    if (customSw->delay)
      result += QObject::tr(" Delay %1 sec").arg(customSw->delay/2.0);
    if (customSw->duration)
      result += QObject::tr(" Duration %1 sec").arg(customSw->duration/2.0);
  }

  return result;
}

QString getProtocolStr(const int proto)
{
  static const char *strings[] = { "OFF",
                                   "PPM",
                                   "Silverlit A", "Silverlit B", "Silverlit C",
                                   "CTP1009",
                                   "LP45", "DSM2", "DSMX",
                                   "PPM16", "PPMsim",
                                   "FrSky XJT - D16", "FrSky XJT - D8", "FrSky XJT - LR12", "FrSky DJT",
  };

  return CHECK_IN_ARRAY(strings, proto);
}

void populateFuncCB(QComboBox *b, unsigned int value)
{
  b->clear();
  for (unsigned int i = 0; i < FuncCount; i++) {
    b->addItem(getFuncName(i));
    if (!GetEepromInterface()->getCapability(HasVolume)) {
      if (i==FuncVolume || i==FuncBackgroundMusic || i==FuncBackgroundMusicPause) {
        QModelIndex index = b->model()->index(i, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
    }
    if ((i==FuncPlayHaptic) && !GetEepromInterface()->getCapability(Haptic)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);      
    }
    if ((i==FuncPlayBoth) && !GetEepromInterface()->getCapability(HasBeeper)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);      
    }
    if ((i==FuncLogs) && !GetEepromInterface()->getCapability(HasSDLogs)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);      
    }
  }
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

QString FuncParam(uint function, int value, QString paramT,unsigned int adjustmode)
{
  QStringList qs;
  if (function <= FuncInstantTrim) {
    return QString("%1").arg(value);
  }
  else if (function==FuncPlaySound) {
    qs <<"Beep 1" << "Beep 2" << "Beep 3" << "Warn1" << "Warn2" << "Cheep" << "Ratata" << "Tick" << "Siren" << "Ring" ;
    qs << "SciFi" << "Robot" << "Chirp" << "Tada" << "Crickt"  << "AlmClk"  ;
    if (value>=0 && value<(int)qs.count())
      return qs.at(value);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (function==FuncPlayHaptic) {
    qs << "0" << "1" << "2" << "3";
    if (value>=0 && value<(int)qs.count())
      return qs.at(value);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if (function==FuncReset) {
    qs.append( QObject::tr("Timer1"));
    qs.append( QObject::tr("Timer2"));
    qs.append( QObject::tr("All"));
    qs.append( QObject::tr("Telemetry"));
    if (value>=0 && value<(int)qs.count())
      return qs.at(value);
    else
      return QObject::tr("<font color=red><b>Inconsistent parameter</b></font>");
  }
  else if ((function==FuncVolume)|| (function==FuncPlayValue)) {
    RawSource item(value);
    return item.toString();
  }
  else if ((function==FuncPlayPrompt) || (function==FuncPlayBoth)) {
    if ( GetEepromInterface()->getCapability(VoicesAsNumbers)) {
      return QString("%1").arg(value);
    } else {
      return paramT;
    }
  } else if ((function>FuncBackgroundMusicPause) && (function<FuncCount)) {
    switch (adjustmode) {
      case 0:
        return QObject::tr("Value ")+QString("%1").arg(value);
        break;
      case 1:
        return RawSource(value).toString();
        break;
      case 2:
        return RawSource(value).toString();
        break;
      case 3:
        if (value==0) {
          return QObject::tr("Decr:")+QString(" -1");
        } else {
          return QObject::tr("Incr:")+QString(" +1");
        }
        break;
      default:
        return "";
    } 
  }
  return "";
}

void populateFuncParamArmTCB(QComboBox *b, ModelData * g_model, char * value, QStringList & paramsList)
{
  b->clear();
  b->addItem("----");

  QString currentvalue(value);
  foreach ( QString entry, paramsList ) {
    b->addItem(entry);
    if (entry==currentvalue) {
      b->setCurrentIndex(b->count()-1);
    }
  }
}

void populateFuncParamCB(QComboBox *b, uint function, unsigned int value, unsigned int adjustmode)
{
  QStringList qs;
  b->clear();
  if (function==FuncPlaySound) {
    qs <<"Beep 1" << "Beep 2" << "Beep 3" << "Warn1" << "Warn2" << "Cheep" << "Ratata" << "Tick" << "Siren" << "Ring" ;
    qs << "SciFi" << "Robot" << "Chirp" << "Tada" << "Crickt"  << "AlmClk"  ;
    b->addItems(qs);
    b->setCurrentIndex(value);
  }
  else if (function==FuncPlayHaptic) {
    qs << "0" << "1" << "2" << "3";
    b->addItems(qs);
    b->setCurrentIndex(value);
  }
  else if (function==FuncReset) {
    qs.append( QObject::tr("Timer1"));
    qs.append( QObject::tr("Timer2"));
    qs.append( QObject::tr("All"));
    qs.append( QObject::tr("Telemetry"));
    b->addItems(qs);
    b->setCurrentIndex(value);
  }
  else if (function==FuncVolume) {
    populateSourceCB(b, RawSource(value), POPULATE_SOURCES|POPULATE_TRIMS);
  }
  else if (function==FuncPlayValue) {
    populateSourceCB(b, RawSource(value), POPULATE_SOURCES|POPULATE_SWITCHES|POPULATE_GVARS|POPULATE_TRIMS|POPULATE_TELEMETRYEXT);
  }
  else if (function>=FuncAdjustGV1 && function<=FuncAdjustGV5 ) {
    switch (adjustmode) {
      case 1:
        populateSourceCB(b, RawSource(value), POPULATE_SOURCES|POPULATE_TRIMS|POPULATE_SWITCHES);
        break;
      case 2:
        populateSourceCB(b, RawSource(value), POPULATE_GVARS);
        break;
      case 3:
        b->clear();
        b->addItem("-1", 0);
        b->addItem("+1", 1);
        b->setCurrentIndex(value);
        break;
    }
  }

  else {
    b->hide();
  }
}

void populateRepeatCB(QComboBox *b, unsigned int value)
{
  b->clear();
  b->addItem(QObject::tr("No repeat", 0));
  unsigned int step = IS_ARM(GetEepromInterface()->getBoard()) ? 5 : 10;
  for (unsigned int i=step; i<=60; i+=step) {
    b->addItem(QObject::tr("%1s").arg(i), i);
    if (i==value) b->setCurrentIndex(b->count()-1);
  }
}

void populateGVmodeCB(QComboBox *b, unsigned int value)
{
  b->clear();
  b->addItem(QObject::tr("Value"));
  b->addItem(QObject::tr("Source"));
  b->addItem(QObject::tr("GVAR"));
  b->addItem(QObject::tr("Increment"));
  b->setCurrentIndex(value);
}

void populatePhasesCB(QComboBox *b, int value)
{
  for (int i=-GetEepromInterface()->getCapability(FlightPhases); i<=GetEepromInterface()->getCapability(FlightPhases); i++) {
    if (i < 0)
      b->addItem(QObject::tr("!Flight mode %1").arg(-i-1), i);
    else if (i > 0)
      b->addItem(QObject::tr("Flight mode %1").arg(i-1), i);
    else
      b->addItem(QObject::tr("----"), 0);
  }
  b->setCurrentIndex(value + GetEepromInterface()->getCapability(FlightPhases));
}

void populateCurvesCB(QComboBox *b, int value)
{
  b->clear();
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  for (int i = -(numcurves)*GetEepromInterface()->getCapability(HasNegCurves); i < CURVE_BASE + numcurves; i++) {
    if ((i==0) && GetEepromInterface()->getCapability(DiffMixers)) {
      b->addItem(QObject::tr("Diff"));
    } else {
      b->addItem(getCurveStr(i));
    }
  }
  b->setCurrentIndex(value+numcurves*GetEepromInterface()->getCapability(HasNegCurves));
  b->setMaxVisibleItems(10);
}

void populateExpoCurvesCB(QComboBox *b, int value)
{
  b->clear();
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  if (GetEepromInterface()->getCapability(ExpoIsCurve)) {
      b->addItem(QObject::tr("Expo"));
  } else {
      b->addItem(getCurveStr(0));
  }
  for (int i = 1; i < CURVE_BASE + numcurves; i++)    
    b->addItem(getCurveStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
  /* TODO
  if (GetEepromInterface()->getCapability(ExpoCurve5)) {
    int curve5=GetEepromInterface()->getCapability(ExpoCurve5);
    for (int i=CURVE_BASE+curve5; i < CURVE_BASE + MAX_CURVE5; i++) {
      // Get the index of the value to disable
      QModelIndex index = b->model()->index(i, 0);

      // This is the effective 'disable' flag
      QVariant v(0);

      //the magic
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
  if (GetEepromInterface()->getCapability(ExpoCurve9)) {
    int curve9=GetEepromInterface()->getCapability(ExpoCurve9);
    for (int i=CURVE_BASE+MAX_CURVE5+curve9; i < CURVE_BASE + MAX_CURVE5+ MAX_CURVE9; i++) {
      // Get the index of the value to disable
      QModelIndex index = b->model()->index(i, 0);

      // This is the effective 'disable' flag
      QVariant v(0);

      //the magic
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
  */
}

void populateTrimUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem(QObject::tr("Own trim"));
  unsigned int num_phases = GetEepromInterface()->getCapability(FlightPhases);
  if (num_phases>0) {
    for (unsigned int i = 0; i < num_phases; i++) {
      if (i != phase) {
        b->addItem(QObject::tr("Flight mode %1 trim").arg(i));
      }
    }
  }
}

void populateGvarUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem(QObject::tr("Own value"));
  unsigned int num_phases = GetEepromInterface()->getCapability(FlightPhases);
  if (num_phases>0) {
    for (unsigned int i = 0; i < num_phases; i++) {
      if (i != phase) {
        b->addItem(QObject::tr("Flight mode %1 value").arg(i));
      }
    }
  }
}

void populateTimerSwitchCB(QComboBox *b, int value, int extrafields)
{
  b->clear();
  uint8_t endvalue=128;
  uint8_t count=0;
  if (extrafields==2)
    endvalue=192;
  for (int i=-128; i<endvalue; i++) {
    QString timerMode = getTimerMode(i);
    if (!timerMode.isEmpty()) {
      b->addItem(getTimerMode(i), i);
      if (i==value)
        b->setCurrentIndex(b->count()-1);
      if (extrafields==2 && (i<0 || (i>3 && i <TMRMODE_FIRST_CHPERC))) {
        QModelIndex index = b->model()->index(count, 0);
        // This is the effective 'disable' flag
        QVariant v(0);
        //the magic
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
      count++;
    }
  }
  b->setMaxVisibleItems(10);
}

void populateTimerSwitchBCB(QComboBox *b, int value, int extrafields)
{
  b->clear();
  if (extrafields!=2) {
    int startvalue=-128;
    int endvalue=128;
    if (extrafields==1) {
      startvalue=-25;
      endvalue=26;
    }
    for (int i=startvalue; i<endvalue; i++) {
      QString timerMode = getTimerMode(i);
      if (!timerMode.isEmpty()) {
        b->addItem(getTimerMode(i), i);
        if (i==value)
          b->setCurrentIndex(b->count()-1);
      }
    }
  } else {
    for (int i=-33; i<66; i++) {
      QString timerMode = getTimerModeB(i);
      if (!timerMode.isEmpty()) {
        b->addItem(timerMode, i);
        if (i==value)
          b->setCurrentIndex(b->count()-1);
      }
    }
  }
  b->setMaxVisibleItems(10);
}

QString getTimerMode(int tm) {

  QString stt = "OFFABSTHsTH%THt";

  QString s;

  if (tm >= 0 && tm <= TMRMODE_THt) {
    return stt.mid(abs(tm)*3, 3);
  }

  int tma = abs(tm);

  if (tma >= TMRMODE_FIRST_SWITCH && tma < TMRMODE_FIRST_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions)) {
    s = RawSwitch(SWITCH_TYPE_SWITCH, tma - TMRMODE_FIRST_SWITCH + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions) && tma < TMRMODE_FIRST_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions) + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = RawSwitch(SWITCH_TYPE_VIRTUAL, tma - TMRMODE_FIRST_SWITCH - GetEepromInterface()->getCapability(SwitchesPositions) + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_MOMENT_SWITCH && tma < TMRMODE_FIRST_MOMENT_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions)) {
    s =  RawSwitch(SWITCH_TYPE_SWITCH, tma - TMRMODE_FIRST_MOMENT_SWITCH + 1).toString()+"t";
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_MOMENT_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions) && tma < TMRMODE_FIRST_MOMENT_SWITCH + GetEepromInterface()->getCapability(SwitchesPositions) + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = RawSwitch(SWITCH_TYPE_VIRTUAL, tma - TMRMODE_FIRST_MOMENT_SWITCH - GetEepromInterface()->getCapability(SwitchesPositions) + 1).toString()+"t";
    if (tm < 0) s.prepend("!");
    return s;
  }
  if (tma >=TMRMODE_FIRST_CHPERC && tma <TMRMODE_FIRST_CHPERC+16) {
    s = QString("CH%1%").arg(tma-TMRMODE_FIRST_CHPERC+1);
    return s;
  }
  return "";
}

QString getTimerModeB(int tm) {

  QString stt = "---THRRUDELEIDOID1ID2AILGEATRN";

  QString s;
  int tma = abs(tm);
  if (tma>33) {
    tma-=32;
  }
  if (tma < 10) {
    s=stt.mid(abs(tma)*3, 3);
  } else if (tma <19) {
    s=QString("SW%1").arg(tma-9);
  } else  {
    s=QString("SW")+QChar('A'+tma-19);
  }
  if (tm<0) {
    s.prepend("!");
  } else if (tm>33) {
    s.append("m");
  }
  return s;
}

void populateBacklightCB(QComboBox *b, const uint8_t value)
{
  QString strings[] = { QObject::tr("OFF"), QObject::tr("Keys"), QObject::tr("Sticks"), QObject::tr("Keys + Sticks"), QObject::tr("ON"), NULL };

  b->clear();

  for (int i=0; !strings[i].isNull(); i++) {
    b->addItem(strings[i], 0);
    if (value == i) b->setCurrentIndex(b->count()-1);
  }
}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr, UseContext context)
{
  RawSwitch item;

  b->clear();

  if (attr & POPULATE_AND_SWITCHES) {
    if (GetEepromInterface()->getCapability(HasNegAndSwitches)) {
      for (int i=-GetEepromInterface()->getCapability(CustomAndSwitches); i<=-1; i++) {
        item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
      for (int i=-GetEepromInterface()->getCapability(SwitchesPositions); i<=-1; i++) {
        item = RawSwitch(SWITCH_TYPE_SWITCH, i);
        if (GetEepromInterface()->isAvailable(item, context)) {
          b->addItem(item.toString(), item.toValue());
          if (item == value) b->setCurrentIndex(b->count()-1);
        }
      }
    }
    item = RawSwitch(SWITCH_TYPE_NONE);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    for (int i=1; i<=GetEepromInterface()->getCapability(SwitchesPositions); i++) {
      item = RawSwitch(SWITCH_TYPE_SWITCH, i);
      if (GetEepromInterface()->isAvailable(item, context)) {
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
    for (int i=1; i<=GetEepromInterface()->getCapability(CustomAndSwitches); i++) {
      item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    return;
  }

  if (attr & POPULATE_MSWITCHES) {
    if (attr & POPULATE_ONOFF) {
      item = RawSwitch(SWITCH_TYPE_ONM, 1);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    
    for (int i=-GetEepromInterface()->getCapability(CustomSwitches); i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    for (int i=-GetEepromInterface()->getCapability(SwitchesPositions); i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_OFF);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetEepromInterface()->getCapability(CustomSwitches); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetEepromInterface()->getCapability(SwitchesPositions); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  item = RawSwitch(SWITCH_TYPE_NONE);
  if (GetEepromInterface()->isAvailable(item, context)) {
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(SwitchesPositions); i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_MSWITCHES) {
    for (int i=1; i<=GetEepromInterface()->getCapability(SwitchesPositions); i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }

    for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  if (attr & POPULATE_MSWITCHES) {
    if (attr & POPULATE_ONOFF) {
      item = RawSwitch(SWITCH_TYPE_ONM);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    if (IS_ARM(GetEepromInterface()->getBoard())) {
      item = RawSwitch(SWITCH_TYPE_TRN,0);
      if (GetEepromInterface()->isAvailable(item, context)) {
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
      item = RawSwitch(SWITCH_TYPE_TRN,1);
      if (GetEepromInterface()->isAvailable(item, context)) {
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
    if (GetEepromInterface()->getBoard()==BOARD_SKY9X) {
      item = RawSwitch(SWITCH_TYPE_REA,0);
      if (GetEepromInterface()->isAvailable(item, context)) {
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
      item = RawSwitch(SWITCH_TYPE_REA,1);
      if (GetEepromInterface()->isAvailable(item, context)) {
        b->addItem(item.toString(), item.toValue());
        if (item == value) b->setCurrentIndex(b->count()-1);
      }
    }
  }

  b->setMaxVisibleItems(10);
}

void populateGVCB(QComboBox *b, int value)
{
  int selected=0;
  int nullitem;
  b->clear();
  int pgvars=GetEepromInterface()->getCapability(GvarsOfsNum);
  for (int i=-pgvars; i<=-1; i++) {
    int16_t gval = (int16_t)(-10000+i);
    b->addItem(QObject::tr("-GV%1").arg(-i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected=1;
    }
  }
  b->addItem("---", 0);
  nullitem=b->count()-1;
  if (value == 0) {
    b->setCurrentIndex(b->count()-1);
    selected=1;
  }
  for (int i=1; i<=pgvars; i++) {
    int16_t gval = (int16_t)(10000+i);
    b->addItem(QObject::tr("GV%1").arg(i), gval);
    if (value == gval) {
      b->setCurrentIndex(b->count()-1);
      selected=1;
    }
  }
  if (selected==0)
    b->setCurrentIndex(nullitem);
}


void populateSourceCB(QComboBox *b, const RawSource &source, unsigned int flags)
{
  RawSource item;

  b->clear();

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_NONE);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);

    for (int i=0; i<4+GetEepromInterface()->getCapability(Pots); i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TRIMS) {
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  
  if (flags & POPULATE_SWITCHES) {
    for (int i=0; i<GetEepromInterface()->getCapability(Switches); i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetEepromInterface()->getCapability(CustomSwitches); i++) {
      item = RawSource(SOURCE_TYPE_CUSTOM_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_SOURCES) {
    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }

    for (int i=0; i<GetEepromInterface()->getCapability(Outputs)+GetEepromInterface()->getCapability(ExtraChannels); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_TELEMETRYEXT) {
    for (int i=0; i<TELEMETRY_SOURCE_ACC; i++) {
      item = RawSource(SOURCE_TYPE_TELEMETRY, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  } else if (flags & POPULATE_TELEMETRY) {
    for (int i=0; i<TELEMETRY_SOURCES_COUNT; i++) {
      item = RawSource(SOURCE_TYPE_TELEMETRY, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  if (flags & POPULATE_GVARS) {
    for (int i=0; i<GetEepromInterface()->getCapability(GvarsNum); i++) {
      item = RawSource(SOURCE_TYPE_GVAR, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
}

#define CSWITCH_STR  "----  a~x   a>x   a<x   |a|>x |a|<x AND   OR    XOR   a=b   a!=b  a>b   a<b   a>=b  a<=b  d>=x  |d|>=xTIM   "
#define CSW_NUM_FUNC 18 // TODO enum
#define CSW_LEN_FUNC 6
int csw_values[]={  
  CS_FN_OFF,
  CS_FN_VEQUAL, // added at the end to avoid everything renumbered
  CS_FN_VPOS,
  CS_FN_VNEG,
  CS_FN_APOS,
  CS_FN_ANEG,
  CS_FN_AND,
  CS_FN_OR,
  CS_FN_XOR,
  CS_FN_EQUAL,
  CS_FN_NEQUAL,
  CS_FN_GREATER,
  CS_FN_LESS,
  CS_FN_EGREATER,
  CS_FN_ELESS,
  CS_FN_DPOS,
  CS_FN_DAPOS,
  CS_FN_TIM
};

QString getCSWFunc(int val)
{
  return QString(CSWITCH_STR).mid(val*CSW_LEN_FUNC, CSW_LEN_FUNC);
}

float ValToTim(int value)
{
   return ((value < -109 ? 129+value : (value < 7 ? (113+value)*5 : (53+value)*10))/10.0);   
}

int TimToVal(float value)
{
  int temp;
  if (value>60) {
    temp=136+round((value-60));
  } else if (value>2) {
    temp=20+round((value-2.0)*2.0);
  } else {
    temp=round(value*10.0);
  }
  return (temp-129);
}  

void populateCSWCB(QComboBox *b, int value)
{
  b->clear();
  for (int i = 0; i < CSW_NUM_FUNC; i++) {
    b->addItem(getCSWFunc(i),csw_values[i]);
    if (i>GetEepromInterface()->getCapability(CSFunc)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
    }
    if (value == csw_values[i]) {
      b->setCurrentIndex(b->count()-1);
    }
  }
  b->setMaxVisibleItems(10);
}

QString getSignedStr(int value)
{
  return value > 0 ? QString("+%1").arg(value) : QString("%1").arg(value);
}

QString getCurveStr(int curve)
{
  QString crvStr = "!c16!c15!c14!c13!c12!c11!c10!c9 !c8 !c7 !c6 !c5 !c4 !c3 !c2 !c1 ----x>0 x<0 |x| f>0 f<0 |f| c1  c2  c3  c4  c5  c6  c7  c8  c9  c10 c11 c12 c13 c14 c15 c16 ";
  return crvStr.mid((curve+C9X_MAX_CURVES) * 4, 4).remove(' ').replace("c", QObject::tr("Curve") + " ");
}

QString getGVarString(int16_t val, bool sign)
{
  if (val >= -10000 && val <= 10000)
    if (sign)
      return QString("(%1%)").arg(getSignedStr(val));
    else
      return QString("(%1%)").arg(val);
  else
    if (val<0) {
      return QObject::tr("(-GV%1)").arg(-val-10000);
    } else {
      return QObject::tr("(GV%1)").arg(val-10000);
    }
}

QString image2qstring(QImage image)
{   
    if (image.isNull())
      return "";
    QBuffer buffer;
    image.save(&buffer, "PNG");
    QString ImageStr;
    int b=0;
    int size=buffer.data().size();
    for (int j = 0; j < size; j++) {
      b=buffer.data().at(j);
      ImageStr += QString("%1").arg(b&0xff, 2, 16, QChar('0'));
    }
    return ImageStr;
}

QImage qstring2image(QString imagestr)
{
  bool ok;
  bool failed=false;
  QImage Image;
  int len = imagestr.length();
  char b=0;
  QBuffer buffer;
  buffer.open(QIODevice::ReadWrite);
  buffer.seek(0);
  for (int i = 0; i < len/2; i++) {
    QString Byte;
    Byte = imagestr.mid((i * 2), 2);
    b = Byte.toUInt(&ok, 16);
    if (!ok) {
      failed = true;
    }
    buffer.putChar(b);
  }
  buffer.seek(0);
  if (!failed) {
    Image.load(&buffer,"PNG");
  }  
  return Image;
}

int findmult(float value, float base)
{
  int vvalue = value*10;
  int vbase = base*10;
  vvalue--;

  int mult = 0;
  for (int i=8; i>=0; i--) {
    if (vvalue/vbase >= (1<<i)) {
      mult = i+1;
      break;
    }
  }
  
  return mult;
}

bool checkbit(int value, int bit)
{
  return ((value & (1<<bit))==(1<<bit));
}


QString getFrSkyAlarmType(int alarm)
{
  switch (alarm) {
    case 1:
      return QObject::tr("Yellow");
    case 2:
      return QObject::tr("Orange");
    case 3:
      return QObject::tr("Red");
    default:
      return "----";
  }
}

QString getFrSkyBlades(int blades)
{
  switch (blades) {
    case 1:
      return "3";
    case 2:
      return "4";
    default:
      return "2";
  }
}


QString getFrSkyUnits(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("---");
    default:
      return "V";
  }
}

QString getFrSkyProtocol(int protocol)
{
  switch(protocol) {
    case 2:
      if ((GetEepromInterface()->getCapability(Telemetry) & TM_HASWSHH))
        return QObject::tr("Winged Shadow How High");
      else
        return QObject::tr("Winged Shadow How High (not supported)");
    case 1:
      return QObject::tr("FrSky Sensor Hub");
    default:
      return QObject::tr("None");
  }
}

QString getFrSkyMeasure(int units)
{
  switch(units) {
    case 1:
      return QObject::tr("Imperial");
    default:
      return QObject::tr("Metric");
  }
}

QString getFrSkySrc(int index)
{
  return RawSource(SOURCE_TYPE_TELEMETRY, index-1).toString();
}


/*
 1,2) Timer1/Timer2 0:765
 3,4) TX/RX
 5) A1 range
 6) A2 range
 7) ALT 0-1020
 8)RPM 0-12750
 9FUEL 0-100%
 10) T1 -30-225
 11) T2 -30-225
 12) spd 0-510
 13) dist 0-2040
 14)GAlt 0-1020
 15) cell 0-5.1
 16) Cels 0 25.5
 17) Vfas 0 25.5
 18) Curr 0 127.5
 19) Cnsp 0 5100
 20) Powr 0 1275
 21) AccX 0 2.55
 22) AccY 0 2.55
 23) AccZ 0 2.55
 24) Hdg 0 255
 25) VSpd 0 2.55
 26) A1- A1 range
 27) A2- A2 range
 28) Alt- 0 255
 29) Alt+ 0 255
 30) Rpm+ 0 255
 31) T1+ 0 255 (s????)
 32) T2+ 0 255 (e????)
 33) Spd+ 0 255 (ILG???)
 34) Dst+ 0 255 (v ????)
 35) Cur+ 0 25.5 (A)
 1.852
 */

float getBarValue(int barId, int value, FrSkyData *fd)
{
  switch (barId-1) {
    case TELEMETRY_SOURCE_TX_BATT:
      return value/10.0;
    case TELEMETRY_SOURCE_TIMER1:
    case TELEMETRY_SOURCE_TIMER2:
      return (3*value);
    case TELEMETRY_SOURCE_RSSI_TX:
    case TELEMETRY_SOURCE_RSSI_RX:
    case TELEMETRY_SOURCE_FUEL:
      return std::min(100, value);
    case TELEMETRY_SOURCE_A1:
    case TELEMETRY_SOURCE_A1_MIN:
      if (fd->channels[0].type==0)
        return ((fd->channels[0].ratio*value/255.0)+fd->channels[0].offset)/10;
      else
        return ((fd->channels[0].ratio*value/255.0)+fd->channels[0].offset);
    case TELEMETRY_SOURCE_A2:
    case TELEMETRY_SOURCE_A2_MIN:
      if (fd->channels[1].type==0)
        return ((fd->channels[1].ratio*value/255.0)+fd->channels[1].offset)/10;
      else
        return ((fd->channels[1].ratio*value/255.0)+fd->channels[1].offset);
    case TELEMETRY_SOURCE_ALT:
    case TELEMETRY_SOURCE_GPS_ALT:
    case TELEMETRY_SOURCE_ALT_MAX:
    case TELEMETRY_SOURCE_ALT_MIN:
      return (8*value)-500;
    case TELEMETRY_SOURCE_RPM:
    case TELEMETRY_SOURCE_RPM_MAX:
      return value * 50;
    case TELEMETRY_SOURCE_T1:
    case TELEMETRY_SOURCE_T2:
    case TELEMETRY_SOURCE_T1_MAX:
    case TELEMETRY_SOURCE_T2_MAX:
      return value - 30.0;
    case TELEMETRY_SOURCE_CELL:
      return value*2.0/100;
    case TELEMETRY_SOURCE_CELLS_SUM:
    case TELEMETRY_SOURCE_VFAS:
      return value/10.0;
    case TELEMETRY_SOURCE_HDG:
      return std::min(359, value*2);
    case TELEMETRY_SOURCE_DIST_MAX:
    case TELEMETRY_SOURCE_DIST:
      return value * 8;
    case TELEMETRY_SOURCE_CURRENT_MAX:
    case TELEMETRY_SOURCE_CURRENT:
      return value/2.0;
    case TELEMETRY_SOURCE_POWER:
      return value*5;
    case TELEMETRY_SOURCE_CONSUMPTION:
      return value * 20;
    case TELEMETRY_SOURCE_SPEED:
    case TELEMETRY_SOURCE_SPEED_MAX:
      if (fd->imperial==1)
        return value;
      else
        return value*1.852;
    default:
      return value;
  }
}

QString getTrimInc(ModelData * g_model)
{
    switch (g_model->trimInc) {
      case (1): return QObject::tr("Extra Fine");
      case (2): return QObject::tr("Fine");
      case (3): return QObject::tr("Medium");
      case (4): return QObject::tr("Coarse");
      default: return QObject::tr("Exponential");
    }
}

QString getTimerStr(TimerData & timer)
{
  QString str = ", " + (timer.dir ? QObject::tr("Count Up") : QObject::tr("Count Down"));
  return QObject::tr("%1:%2, ").arg(timer.val/60, 2, 10, QChar('0')).arg(timer.val%60, 2, 10, QChar('0')) + getTimerMode(timer.mode) + str;
}

QString getProtocol(ModelData * g_model)
{
  QString str = getProtocolStr(g_model->moduleData[0].protocol);

  if (g_model->moduleData[0].protocol == PPM)
    str.append(QObject::tr(": %1 Channels, %2usec Delay").arg(g_model->moduleData[0].channelsCount).arg(g_model->moduleData[0].ppmDelay));

  return str;
}

float c9xexpou(float point, float coeff)
{
  float x=point*1024.0/100.0;
  float k=coeff*256.0/100.0;
  return ((k*x*x*x/(1024*1024) + x*(256-k) + 128) / 256)/1024.0*100;
}

QString getCenterBeep(ModelData * g_model)
{
  //RETA123
  QStringList strl;
  if(g_model->beepANACenter & 0x01) strl << QObject::tr("Rudder");
  if(g_model->beepANACenter & 0x02) strl << QObject::tr("Elevator");
  if(g_model->beepANACenter & 0x04) strl << QObject::tr("Throttle");
  if(g_model->beepANACenter & 0x08) strl << QObject::tr("Aileron");
  if(g_model->beepANACenter & 0x10) strl << "P1";
  if(g_model->beepANACenter & 0x20) strl << "P2";
  if(g_model->beepANACenter & 0x40) strl << "P3";
  if(g_model->beepANACenter & 0x80) strl << "LS";
  return strl.join(", ");
}
