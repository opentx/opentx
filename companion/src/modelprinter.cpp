#include "helpers.h"
#include "modelprinter.h"
#include <QPainter>

QString changeColor(const QString & input, const QString & to, const QString & from)
{
  QString result = input;
  return result.replace("color="+from, "color="+to);
}

ModelPrinter::ModelPrinter(Firmware * firmware, const GeneralSettings & generalSettings, const ModelData & model):
  firmware(firmware),
  generalSettings(generalSettings),
  model(model)
{
}

ModelPrinter::~ModelPrinter()
{
  foreach(QString filename, curvefiles) {
    qunlink(filename);
  }
}

void debugHtml(const QString & html)
{
  QFile file("foo.html");
  file.open(QIODevice::Truncate | QIODevice::WriteOnly);
  file.write(html.toUtf8());
  file.close();
}

QString addFont(const QString & input, const QString & color, const QString & size, const QString & face)
{
  QString colorStr;
  if (!color.isEmpty()) {
    colorStr = "color=" + color;
  }
  QString sizeStr;
  if (!size.isEmpty()) {
    sizeStr = "size=" + size;
  }
  QString faceStr;
  if (!face.isEmpty()) {
    faceStr = "face='" + face + "'";
  }
  return "<font " + sizeStr + " " + faceStr + " " + colorStr + ">" + input + "</font>";  
}

QString ModelPrinter::printEEpromSize()
{
  return tr("%1 bytes").arg(GetEepromInterface()->getSize(model));
}

QString ModelPrinter::printChannelName(int idx)
{
  return tr("CH%1").arg(idx+1, 2, 10, QChar('0'));
}

QString ModelPrinter::printOutputName(int idx)
{
  QString name = QString(model.limitData[idx].name).trimmed();
  if (firmware->getCapability(HasChNames) && !name.isEmpty()) {
    return name;
  }
  else {
    return "";
  }
}

QString ModelPrinter::printTrimIncrementMode()
{
  switch (model.trimInc) {
    case -2:
      return tr("Exponential");
    case -1:
      return tr("Extra Fine");
    case 0:
      return tr("Fine");
    case 1:
      return tr("Medium");
    case 2:
      return tr("Coarse");
    default:
      return tr("Unknown");
  }
}

QString ModelPrinter::printThrottleTrimMode()
{
  return model.thrTrim ? tr("Enabled") : tr("Disabled");
}

QString ModelPrinter::printModuleProtocol(unsigned int protocol)
{
  static const char *strings[] = { "OFF",
                                   "PPM",
                                   "Silverlit A", "Silverlit B", "Silverlit C",
                                   "CTP1009",
                                   "LP45", "DSM2", "DSMX",
                                   "PPM16", "PPMsim",
                                   "FrSky XJT (D16)", "FrSky XJT (D8)", "FrSky XJT (LR12)", "FrSky DJT",
  };

  return CHECK_IN_ARRAY(strings, protocol);
}

QString ModelPrinter::printModule(int idx)
{
  const ModuleData & module = model.moduleData[idx];
  if (module.protocol == OFF)
    return printModuleProtocol(module.protocol);
  else if (module.protocol == PPM)
    return tr("%1, Channels(%2-%3), PPM delay(%4usec), Pulse polarity(%5)").arg(printModuleProtocol(module.protocol)).arg(module.channelsStart+1).arg(module.channelsStart+module.channelsCount).arg(module.ppmDelay).arg(module.polarityToString());
  else {
    QString result = tr("%1, Channels(%2-%3)").arg(printModuleProtocol(module.protocol)).arg(module.channelsStart+1).arg(module.channelsStart+module.channelsCount);
    if (module.protocol != PXX_XJT_D8) {
      result += " " + tr("Receiver number(%1)").arg(module.modelId);
    }
    return result;
  }
}

QString ModelPrinter::printTrainerMode()
{
  QString result;
  switch (model.trainerMode) {
    case 1:
      result = tr("Slave/Jack"); // TODO + tr(": Channel start: %1, %2 Channels, %3usec Delay, Pulse polarity %4").arg(module.channelsStart+1).arg(module.channelsCount).arg(module.ppmDelay).arg(module.polarityToString());
      break;
    case 2:
      result = tr("Master/SBUS Module");
      break;
    case 3:
      result = tr("Master/CPPM Module");
      break;
    case 4:
      result = tr("Master/SBUS in battery compartment");
      break;
    default:
      result = tr("Master/Jack");
  }
  return result;
}

QString ModelPrinter::printCenterBeep()
{
  QStringList strl;
  if (model.beepANACenter & 0x01)
    strl << tr("Rudder");
  if (model.beepANACenter & 0x02)
    strl << tr("Elevator");
  if (model.beepANACenter & 0x04)
    strl << tr("Throttle");
  if (model.beepANACenter & 0x08)
    strl << tr("Aileron");
  if (IS_TARANIS(firmware->getBoard())) {
    if (model.beepANACenter & 0x10)
      strl << "S1";
    if (model.beepANACenter & 0x20)
      strl << "S2";
    if (model.beepANACenter & 0x40)
      strl << "S3";
    if (model.beepANACenter & 0x80)
      strl << "LS";
    if (model.beepANACenter & 0x100)
      strl << "RS";
  }
  else {
    if (model.beepANACenter & 0x10)
      strl << "P1";
    if (model.beepANACenter & 0x20)
      strl << "P2";
    if (model.beepANACenter & 0x40)
      strl << "P3";
  }
  return strl.join(", ");
}

QString ModelPrinter::printTimer(int idx)
{
  return printTimer(model.timers[idx]);
}

QString ModelPrinter::printTimer(const TimerData & timer)
{
  QStringList result;
  if (firmware->getCapability(TimersName) && timer.name[0]) 
    result += tr("Name(%1)").arg(timer.name);
  result += tr("%1:%2").arg(timer.val/60, 2, 10, QChar('0')).arg(timer.val%60, 2, 10, QChar('0'));
  result += timer.mode.toString();
  if (timer.persistent)
    result += tr("Persistent");
  if (timer.minuteBeep)
    result += tr("MinuteBeep");
  if (timer.countdownBeep == TimerData::COUNTDOWN_BEEPS)
    result += tr("CountDown(Beeps)");
  else if (timer.countdownBeep == TimerData::COUNTDOWN_VOICE)
    result += tr("CountDown(Voice)");
  else if (timer.countdownBeep == TimerData::COUNTDOWN_HAPTIC)
    result += tr("CountDown(Haptic)");
  return result.join(", ");
}

QString ModelPrinter::printTrim(int flightModeIndex, int stickIndex)
{
  const FlightModeData & fm = model.flightModeData[flightModeIndex];

  if (fm.trimMode[stickIndex] == -1) { 
    return tr("Off");
  } 
  else {
    if (fm.trimRef[stickIndex] == flightModeIndex) {
      return QString("%1").arg(fm.trim[stickIndex]);
    }
    else {
      if (fm.trimMode[stickIndex] == 0) {
        return tr("FM%1").arg(fm.trimRef[stickIndex]);
      }
      else {
        if (fm.trim[stickIndex] < 0) 
          return tr("FM%1%2").arg(fm.trimRef[stickIndex]).arg(fm.trim[stickIndex]);
        else
          return tr("FM%1+%2").arg(fm.trimRef[stickIndex]).arg(fm.trim[stickIndex]);
      }
    }
  }
}

QString ModelPrinter::printGlobalVar(int flightModeIndex, int gvarIndex)
{
  const FlightModeData & fm = model.flightModeData[flightModeIndex];

  if (fm.gvars[gvarIndex] <= 1024) {
    return QString("%1").arg(fm.gvars[gvarIndex]);
  }
  else {
    int num = fm.gvars[gvarIndex] - 1025;
    if (num >= flightModeIndex) num++;
    return tr("FM%1").arg(num);
  }
}

QString ModelPrinter::printRotaryEncoder(int flightModeIndex, int reIndex)
{
  const FlightModeData & fm = model.flightModeData[flightModeIndex];

  if (fm.rotaryEncoders[reIndex] <= 1024) {
    return QString("%1").arg(fm.rotaryEncoders[reIndex]);
  }
  else {
    int num = fm.rotaryEncoders[reIndex] - 1025;
    if (num >= flightModeIndex) num++;
    return tr("FM%1").arg(num);
  }
}

QString ModelPrinter::printInputName(int idx)
{
  QString result;
  if (firmware->getCapability(VirtualInputs)) {
    if (strlen(model.inputNames[idx]) > 0) {
      result = tr("[I%1]").arg(idx+1);
      result += QString(model.inputNames[idx]);
    }
    else {
      result = tr("Input%1").arg(idx+1, 2, 10, QChar('0'));
    }
  }
  else {
    result = RawSource(SOURCE_TYPE_STICK, idx).toString(&model);
  }
  return Qt::escape(result);
}

QString ModelPrinter::printInputLine(int idx)
{
  return printInputLine(model.expoData[idx]);
}

QString ModelPrinter::printInputLine(const ExpoData & input)
{
  QString str = "&nbsp;";

  switch (input.mode) {
    case (1): str += "&lt;-&nbsp;"; break;
    case (2): str += "-&gt;&nbsp;"; break;
    default:  str += "&nbsp;&nbsp;&nbsp;"; break;
  }

  if (firmware->getCapability(VirtualInputs)) {
    str += Qt::escape(input.srcRaw.toString(&model));
  }

  str += " " + Qt::escape(tr("Weight")) + QString("(%1)").arg(getGVarString(input.weight,true));
  if (input.curve.value) str += " " + Qt::escape(input.curve.toString());

  QString flightModesStr = printFlightModes(input.flightModes);
  if (!flightModesStr.isEmpty()) str += " " + Qt::escape(flightModesStr);

  if (input.swtch.type != SWITCH_TYPE_NONE) 
    str += " " + Qt::escape(tr("Switch")) + QString("(%1)").arg(input.swtch.toString());


  if (firmware->getCapability(VirtualInputs)) {
    if (input.carryTrim>0) str += " " + Qt::escape(tr("NoTrim"));
    else if (input.carryTrim<0) str += " " + Qt::escape(RawSource(SOURCE_TYPE_TRIM, (-(input.carryTrim)-1)).toString(&model));
  }

  if (firmware->getCapability(HasExpoNames) && input.name[0]) 
    str += Qt::escape(QString(" [%1]").arg(input.name));

  return str;
}

QString ModelPrinter::printMixerName(int curDest)
{
  QString str = printChannelName(curDest-1) + " ";
  if (firmware->getCapability(HasChNames)) {
    QString name = model.limitData[curDest-1].name;
    if (!name.isEmpty()) {
      name = QString("(") + name + QString(")");
    }
    name.append("        ");
    str += name.left(8);
  }
  return Qt::escape(str);
}

QString ModelPrinter::printMixerLine(int idx, int highlightedSource)
{
  return printMixerLine(model.mixData[idx], highlightedSource);
}

QString ModelPrinter::printMixerLine(const MixData & mix, int highlightedSource)
{
  QString str = "&nbsp;";

  switch(mix.mltpx) {
    case (1): str += "*"; break;
    case (2): str += "R"; break;
    default:  str += "&nbsp;"; break;
  };

  // highlight source if needed
  QString source = Qt::escape(mix.srcRaw.toString(&model));
  if ( (mix.srcRaw.type == SOURCE_TYPE_CH) && (mix.srcRaw.index+1 == (int)highlightedSource) ) {
    source = "<b>" + source + "</b>";
  }
  str += "&nbsp;" + source;

  str += " " + Qt::escape(tr("Weight")) + QString("(%1)").arg(getGVarString(mix.weight, true));

  QString flightModesStr = printFlightModes(mix.flightModes);
  if (!flightModesStr.isEmpty()) str += " " + Qt::escape(flightModesStr);

  if (mix.swtch.type != SWITCH_TYPE_NONE) {
    str += " " + Qt::escape(tr("Switch")) + QString("(%1)").arg(mix.swtch.toString());
  }

  if (mix.carryTrim>0)      str += " " + Qt::escape(tr("NoTrim"));
  else if (mix.carryTrim<0) str += " " + RawSource(SOURCE_TYPE_TRIM, (-(mix.carryTrim)-1)).toString(&model);

  if (firmware->getCapability(HasNoExpo) && mix.noExpo) str += " " + Qt::escape(tr("No DR/Expo"));
  if (mix.sOffset)     str += " " + Qt::escape(tr("Offset")
  ) + QString("(%1)").arg(getGVarString(mix.sOffset));
  if (mix.curve.value) str += " " + Qt::escape(mix.curve.toString());

  int scale = firmware->getCapability(SlowScale);
  if (scale == 0)
    scale = 1;
  if (mix.delayDown || mix.delayUp)
    str += " " + Qt::escape(tr("Delay")) + QString("(u%1:d%2)").arg((double)mix.delayUp/scale).arg((double)mix.delayDown/scale);
  if (mix.speedDown || mix.speedUp)
    str += " " + Qt::escape(tr("Slow")) + QString("(u%1:d%2)").arg((double)mix.speedUp/scale).arg((double)mix.speedDown/scale);
  if (mix.mixWarn)
    str += " " + Qt::escape(tr("Warn(%1)").arg(mix.mixWarn));
  if (firmware->getCapability(HasMixerNames) && mix.name[0]) 
    str += Qt::escape(QString(" [%1]").arg(mix.name));
  return str;
}

QString ModelPrinter::printFlightModeSwitch(int index)
{
  if (index == 0)
    return "---";
  else if (index > 0) {
    return printFlightModeName(index-1);
  }
  else {
    return "!" + printFlightModeName(-index-1);
  }
}

QString ModelPrinter::printFlightModeName(int index)
{
  const FlightModeData & fm = model.flightModeData[index];
  if (strlen(fm.name) > 0) {
    return QString("%1").arg(fm.name);
  }
  else {
    return tr("FM%1").arg(index);
  }
}

QString ModelPrinter::printFlightModes(unsigned int flightModes)
{
  int numFlightModes = firmware->getCapability(FlightModes);
  if (numFlightModes && flightModes) {
    if (flightModes == (unsigned int)(1<<numFlightModes) - 1) {
      return tr("Disabled in all flight modes");
    }
    else {
      QStringList list;
      for (int i=0; i<numFlightModes; i++) {
        if (!(flightModes & (1<<i))) {
          list << printFlightModeName(i);
        }
      }
      if (list.size() > 1)
        return tr("Flight modes(%1)").arg(list.join(", "));
      else
        return tr("Flight mode(%1)").arg(list.join(", "));
    }
  }
  else return "";
}

QString ModelPrinter::printLogicalSwitchLine(int idx)
{
  QString result = "";
  const LogicalSwitchData & ls = model.logicalSw[idx];

  if (!ls.func)
    return result;

  if (ls.andsw!=0) {
    result +="( ";
  }
  switch (ls.getFunctionFamily()) {
    case LS_FAMILY_EDGE:
      result += tr("Edge(%1, [%2:%3])").arg(RawSwitch(ls.val1).toString()).arg(ValToTim(ls.val2)).arg(ValToTim(ls.val2+ls.val3));
      break;
    case LS_FAMILY_STICKY:
      result += tr("Sticky(%1, %2)").arg(RawSwitch(ls.val1).toString()).arg(RawSwitch(ls.val2).toString());
      break;
    case LS_FAMILY_TIMER:
      result += tr("Timer(%1, %2)").arg(ValToTim(ls.val1)).arg(ValToTim(ls.val2));
      break;
    case LS_FAMILY_VOFS: {
      RawSource source = RawSource(ls.val1);
      RawSourceRange range = source.getRange(&model, generalSettings);
      QString res;
      if (ls.val1)
        res += source.toString(&model);
      else
        res += "0";
      res.remove(" ");
      if (ls.func == LS_FN_APOS || ls.func == LS_FN_ANEG)
        res = "|" + res + "|";
      else if (ls.func == LS_FN_DAPOS)
        res = "|d(" + res + ")|";
      else if (ls.func == LS_FN_DPOS)
        result = "d(" + res + ")";
      result += res;
      if (ls.func == LS_FN_VEQUAL)
        result += " = ";
      else if (ls.func == LS_FN_APOS || ls.func == LS_FN_VPOS || ls.func == LS_FN_DPOS || ls.func == LS_FN_DAPOS)
        result += " &gt; ";
      else if (ls.func == LS_FN_ANEG || ls.func == LS_FN_VNEG)
        result += " &lt; ";
      else if (ls.func == LS_FN_VALMOSTEQUAL)
        result += " ~ ";
      else
        result += " missing";
      result += QString::number(range.step * (ls.val2 /*TODO+ source.getRawOffset(model)*/) + range.offset);
      break;
    }
    case LS_FAMILY_VBOOL:
      result += RawSwitch(ls.val1).toString();
      switch (ls.func) {
        case LS_FN_AND:
          result += " AND ";
          break;
        case LS_FN_OR:
          result += " OR ";
          break;
        case LS_FN_XOR:
          result += " XOR ";
          break;
       default:
          result += " bar ";
          break;
      }
      result += RawSwitch(ls.val2).toString();
      break;

    case LS_FAMILY_VCOMP:
      if (ls.val1)
        result += RawSource(ls.val1).toString(&model);
      else
        result += "0";
      switch (ls.func) {
        case LS_FN_EQUAL:
        case LS_FN_VEQUAL:
          result += " = ";
          break;
        case LS_FN_NEQUAL:
          result += " != ";
          break;
        case LS_FN_GREATER:
          result += " &gt; ";
          break;
        case LS_FN_LESS:
          result += " &lt; ";
          break;
        case LS_FN_EGREATER:
          result += " &gt;= ";
          break;
        case LS_FN_ELESS:
          result += " &lt;= ";
          break;
        default:
          result += " foo ";
          break;
      }
      if (ls.val2)
        result += RawSource(ls.val2).toString(&model);
      else
        result += "0";
      break;
  }

  if (ls.andsw != 0) {
    result +=" ) AND ";
    result += RawSwitch(ls.andsw).toString();
  }

  if (firmware->getCapability(LogicalSwitchesExt)) {
    if (ls.duration)
      result += " " + tr("Duration(%1s)").arg(ls.duration/10.0);
    if (ls.delay)
      result += " " + tr("Delay(%1s)").arg(ls.delay/10.0);
  }

  return result;
}

QString ModelPrinter::printCustomFunctionLine(int idx)
{
  QString result;
  const CustomFunctionData & cf = model.customFn[idx];
  if (cf.swtch.type == SWITCH_TYPE_NONE) return result;

  result += cf.swtch.toString() + " - ";
  result += cf.funcToString() + "(";
  result += cf.paramToString(&model) + ")";
  if (!cf.repeatToString().isEmpty()) result += " " + cf.repeatToString();
  if (!cf.enabledToString().isEmpty()) result += " " + cf.enabledToString();
  return result;
}

QString ModelPrinter::printCurve(int idx)
{
  QString result;
  const CurveData & curve = model.curves[idx];
  result += (curve.type == CurveData::CURVE_TYPE_CUSTOM) ? tr("Custom") : tr("Standard");
  result += ", [";
  if (curve.type == CurveData::CURVE_TYPE_CUSTOM) {
    for (int j=0; j<curve.count; j++) {
      if (j != 0)
        result += ", ";
      result += QString("(%1, %2)").arg(curve.points[j].x).arg(curve.points[j].y);
    }
  }
  else {
    for (int j=0; j<curve.count; j++) {
      if (j != 0)
        result += ", ";
      result += QString("%1").arg(curve.points[j].y);
    }
  }
  result += "]";
  return result;
}

CurveImage::CurveImage():
  size(200),
  image(size+1, size+1, QImage::Format_RGB32),
  painter(&image)
{
  painter.setBrush(QBrush("#FFFFFF"));
  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, size, size);

  painter.setPen(QColor(0, 0, 0));
  painter.drawLine(0, size/2, size, size/2);
  painter.drawLine(size/2, 0, size/2, size);
  for (int i=0; i<21; i++) {
    painter.drawLine(size/2-5, (size*i)/(20), size/2+5, (size*i)/(20));
    painter.drawLine((size*i)/(20), size/2-5, (size*i)/(20), size/2+5);
  }
}

void CurveImage::drawCurve(const CurveData & curve, QColor color)
{
  painter.setPen(QPen(color, 2, Qt::SolidLine));
  for (int j=1; j<curve.count; j++) {
    if (curve.type == CurveData::CURVE_TYPE_CUSTOM)
      painter.drawLine(size/2+(size*curve.points[j-1].x)/200, size/2-(size*curve.points[j-1].y)/200, size/2+(size*curve.points[j].x)/200, size/2-(size*curve.points[j].y)/200);
    else
      painter.drawLine(size*(j-1)/(curve.count-1), size/2-(size*curve.points[j-1].y)/200, size*(j)/(curve.count-1), size/2-(size*curve.points[j].y)/200);
  }
}

void CurveImage::save(const QString & filename)
{
  image.save(filename, "png", 100);
}

QString ModelPrinter::createCurveImage(int idx)
{
  CurveImage image;
  image.drawCurve(model.curves[idx], colors[idx]);
  QString filename = generateProcessUniqueTempFileName(QString("curve-%1-%2.png").arg((uint64_t)this).arg(idx));
  image.save(filename);
  curvefiles << filename;
  return filename;
}

QString ModelPrinter::createCurvesImage()
{
  CurveImage image;
  for (int idx=0; idx<firmware->getCapability(NumCurves); idx++) {
    image.drawCurve(model.curves[idx], colors[idx]);
  }
  QString filename = generateProcessUniqueTempFileName(QString("curves-%1.png").arg((uint64_t)this));
  image.save(filename);
  curvefiles << filename;
  return filename;
}
