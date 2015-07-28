#include "comparedialog.h"
#include "ui_comparedialog.h"
#include "helpers.h"
#include "helpers_html.h"
#include "eeprominterface.h"
#include <QtGui>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <algorithm>

#if !defined WIN32 && defined __GNUC__
#include <unistd.h>
#endif

#define ISIZE 200 // curve image size
class DragDropHeader {
public:
  DragDropHeader():
    general_settings(false),
    models_count(0)
  {
  }
  bool general_settings;
  uint8_t models_count;
  uint8_t models[C9X_MAX_MODELS];
};

CompareDialog::CompareDialog(QWidget * parent, Firmware * firmware):
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
  firmware(firmware),
  model1Valid(false),
  model2Valid(false),
  ui(new Ui::CompareDialog),
  modelPrinter1(firmware, &dummySettings, &g_model1),
  modelPrinter2(firmware, &dummySettings, &g_model2)
{
  ui->setupUi(this);

  this->setWindowIcon(CompanionIcon("compare.png"));
  te = ui->textEdit;
  this->setAcceptDrops(true);

  //setDragDropOverwriteMode(true);
  //setDropIndicatorShown(true);
  te->scrollToAnchor("1");
}

void CompareDialog::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasFormat("application/x-companion")) {   
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}

void CompareDialog::dragEnterEvent(QDragEnterEvent *event)
{
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("application/x-companion")) {   
    event->acceptProposedAction();
  }
  else {
    event->ignore();
  }
}

void CompareDialog::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void CompareDialog::printDiff()
{
  te->clear();
  curvefile1=generateProcessUniqueTempFileName("curve1.png");
  curvefile2=generateProcessUniqueTempFileName("curve2.png");
  printSetup();
  if (GetCurrentFirmware()->getCapability(FlightModes)) {
    printPhases();
  }
  printInputs();
  printMixers();
  printLimits();
  printCurves();
  printGvars();
  printLogicalSwitches();
  printCustomFunctions();
  printFrSky();
  te->scrollToAnchor("1");
}

bool CompareDialog::handleDroppedModel(const QMimeData * mimeData, ModelData * model, QLabel * label)
{
  if(mimeData->hasFormat("application/x-companion")) {
    QByteArray gmData = mimeData->data("application/x-companion");
    DragDropHeader *header = (DragDropHeader *)gmData.data();
    if (!header->general_settings) {
      char *gData = gmData.data()+sizeof(DragDropHeader);
      if(gData[0] == 'M') {
        ModelData * modeltemp = (ModelData *)(gData + 1);
        if (modeltemp->used) {
          memcpy(model, modeltemp, sizeof(ModelData));
          QString name(model->name);
          if (!name.isEmpty()) {
            label->setText(name);
          } else {
            label->setText(tr("No name"));
          }
          return true;
        }
      }
    }
  }
  return false;
}

void CompareDialog::dropEvent(QDropEvent *event)
{
  QLabel *child = qobject_cast<QLabel*>(childAt(event->pos()));
  if (!child) return;
  if (child->objectName().contains("label_1")) {   
    model1Valid = handleDroppedModel(event->mimeData(), &g_model1, ui->label_1);     
  }
  else if (child->objectName().contains("label_2")) {
    model2Valid = handleDroppedModel(event->mimeData(), &g_model2, ui->label_2);     
  }
  event->accept();
  if (model1Valid && model2Valid) printDiff();
}

void CompareDialog::closeEvent(QCloseEvent *event) 
{
}

CompareDialog::~CompareDialog()
{
  qunlink(curvefile1);
  qunlink(curvefile2);
  delete ui;
}

bool CompareDialog::ChannelHasExpo(ExpoData * expoArray, uint8_t destCh)
{
  for (int i=0; i< C9X_MAX_EXPOS; i++) {
    if ((expoArray[i].chn==destCh)&&(expoArray[i].mode!=0)) {
      return true;
    }
  }
  return false;
}

bool CompareDialog::ChannelHasMix(MixData * mixArray, uint8_t destCh)
{
  for (int i=0; i< C9X_MAX_MIXERS; i++) {
    if (mixArray[i].destCh==destCh) {
      return true;
    }
  }
  return false;
}

void CompareDialog::printSetup()
{
  QString color;
  QString str = "<a name=1></a><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("General Model Settings")+"</h2></td></tr>");
  str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor1(g_model1.name,g_model2.name);
  str.append(fv(tr("Name"), g_model1.name, color));
  color=getColor1(GetEepromInterface()->getSize(g_model1), GetEepromInterface()->getSize(g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(GetEepromInterface()->getSize(g_model1)).arg(color));
  color=getColor1(getTimerStr(g_model1.timers[0]), getTimerStr(g_model2.timers[0]));
  str.append(fv(tr("Timer1"), getTimerStr(g_model1.timers[0]), color));  //value, mode, count up/down
  color=getColor1(getTimerStr(g_model1.timers[1]), getTimerStr(g_model2.timers[1]));
  str.append(fv(tr("Timer2"), getTimerStr(g_model1.timers[1]), color));  //value, mode, count up/down
  color=getColor1(getProtocol(g_model1.moduleData[0]),getProtocol(g_model2.moduleData[0]));
  if (firmware->getCapability(NumModules)>1) {
    str.append("<b>"+(IS_TARANIS(firmware->getBoard()) ? tr("Internal Radio System") : tr("Radio System") )+"</b><br>&nbsp;&nbsp;"); //proto, numch, delay,
  }
  str.append(fv(tr("Protocol"), getProtocol(g_model1.moduleData[0]),color)); 
  if (firmware->getCapability(NumModules)>1) {
    color=getColor1(getProtocol(g_model1.moduleData[1]),getProtocol(g_model2.moduleData[1]));
    str.append("<b>"+(IS_TARANIS(firmware->getBoard()) ? tr("External Radio Module") : tr("Extra Radio System"))+"</b><br>&nbsp;&nbsp;"); //proto, numch, delay,
    str.append(fv(tr("Protocol"), getProtocol(g_model1.moduleData[1]),color));
  }
  if (IS_TARANIS(firmware->getBoard())){
    color=getColor1(getTrainerMode(g_model1.trainerMode, g_model1.moduleData[2]),getTrainerMode(g_model2.trainerMode, g_model2.moduleData[2]));
    str.append(fv(tr("Trainer port mode"), getTrainerMode(g_model1.trainerMode, g_model1.moduleData[2]),color)); 
  }
  color=getColor1(g_model1.thrTrim,g_model2.thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model1.thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor1(getTrimInc(&g_model1),getTrimInc(&g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(&g_model1),color));
  color = getColor1(getCenterBeepStr(&g_model1), getCenterBeepStr(&g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeepStr(&g_model1), color)); // specify which channels beep
  str.append("</table></td>");
  str.append("<td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor2(g_model1.name,g_model2.name);
  str.append(fv(tr("Name"), g_model2.name, color));
  color=getColor2(GetEepromInterface()->getSize(g_model1), GetEepromInterface()->getSize(g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(GetEepromInterface()->getSize(g_model2)).arg(color));
  color=getColor2(getTimerStr(g_model1.timers[0]), getTimerStr(g_model2.timers[0]));
  str.append(fv(tr("Timer1"), getTimerStr(g_model2.timers[0]),color));  //value, mode, count up/down
  color=getColor2(getTimerStr(g_model1.timers[1]), getTimerStr(g_model2.timers[1]));
  str.append(fv(tr("Timer2"), getTimerStr(g_model2.timers[1]),color));  //value, mode, count up/down
  color=getColor2(getProtocol(g_model1.moduleData[0]),getProtocol(g_model2.moduleData[0]));
  if (firmware->getCapability(NumModules)>1) {
    str.append("<b>"+(IS_TARANIS(firmware->getBoard()) ? tr("Internal Radio System") : tr("Radio System") )+"</b><br>&nbsp;&nbsp;"); //proto, numch, delay,
  }
  str.append(fv(tr("Protocol"), getProtocol(g_model2.moduleData[0]),color)); 
  if (firmware->getCapability(NumModules)>1) {
    color=getColor2(getProtocol(g_model1.moduleData[1]),getProtocol(g_model2.moduleData[1]));
    str.append("<b>"+(IS_TARANIS(firmware->getBoard()) ? tr("External Radio Module") : tr("Extra Radio System"))+"</b><br>&nbsp;&nbsp;"); //proto, numch, delay,
    str.append(fv(tr("Protocol"), getProtocol(g_model2.moduleData[1]),color));
  }
  if (IS_TARANIS(firmware->getBoard())){
    color=getColor2(getTrainerMode(g_model1.trainerMode, g_model1.moduleData[2]),getTrainerMode(g_model2.trainerMode, g_model2.moduleData[2]));
    str.append(fv(tr("Trainer port mode"), getTrainerMode(g_model2.trainerMode, g_model2.moduleData[2]),color)); 
  }
  color=getColor2(g_model1.thrTrim,g_model2.thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model2.thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor2(getTrimInc(&g_model1),getTrimInc(&g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(&g_model2),color));
  color = getColor2(getCenterBeepStr(&g_model1),getCenterBeepStr(&g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeepStr(&g_model2), color)); // specify which channels beep
  str.append("</td></tr></table></td></tr></table>");
  te->append(str);
}

void CompareDialog::printPhases()
{
  QString color;
  int i,k;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Flight modes Settings")+"</h2></td></tr>");
  str.append("<tr><td  width=\"50%\"><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  str.append("<tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>");
  str.append(tr("Fades")+"</b></td><td colspan=4 align=center><b>"+tr("Trims"));
  str.append("</b></td><td rowspan=2 align=\"center\" valign=\"bottom\"><b>"+tr("Switch")+"</b></td></tr><tr><td align=center width=\"80\"><b>"+tr("Flight mode name"));
  str.append("</b></td><td align=center width=\"30\"><b>"+tr("IN")+"</b></td><td align=center width=\"30\"><b>"+tr("OUT")+"</b></td>");
  for (i=0; i<4; i++) {
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(modelPrinter1.printInputName(i)));
  }
  str.append("</tr>");
  for (i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
    FlightModeData *pd1=&g_model1.flightModeData[i];
    FlightModeData *pd2=&g_model2.flightModeData[i];
    str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
    color=getColor1(pd1->name,pd2->name);
    str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->name).arg(color));
    color=getColor1(pd1->fadeIn,pd2->fadeIn);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->fadeIn).arg(color));
    color=getColor1(pd1->fadeOut,pd2->fadeOut);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->fadeOut).arg(color));
    for (k=0; k<4; k++) {
      if (pd1->trimRef[k]==-1) {
        color=getColor1(pd1->trim[k],pd2->trim[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->trim[k]).arg(color));
      } else {
        color=getColor1(pd1->trimRef[k],pd2->trimRef[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(pd1->trimRef[k]));
      }
    }
    color=getColor1(pd1->swtch,pd2->swtch);
    str.append(QString("<td align=center><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->swtch.toString()).arg(color));
    str.append("</tr>");
  }
  str.append("</table>");
  int gvars = GetCurrentFirmware()->getCapability(Gvars);
  if ((gvars && GetCurrentFirmware()->getCapability(GvarsFlightModes)) || GetCurrentFirmware()->getCapability(RotaryEncoders)) {
    str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    str.append("<tr><td style=\"border-style:none;\">&nbsp;</td>");
    if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(gvars)+tr("Gvars")+"</td>");
    }
    if (GetCurrentFirmware()->getCapability(RotaryEncoders)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(GetCurrentFirmware()->getCapability(RotaryEncoders))+tr("Rot. Enc.")+"</td>");
    }
    str.append("</tr><tr><td align=center><b>"+tr("Flight mode name")+"</b></td>");
    if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
      for (i=0; i<gvars; i++) {
        str.append(QString("<td width=\"40\" align=\"center\"><b>GV%1</b><br>%2</td>").arg(i+1).arg(g_model1.gvars_names[i]));
      }
    }
    for (i=0; i<GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
      str.append(QString("<td align=\"center\"><b>RE%1</b></td>").arg((i==0 ? 'A': 'B')));
    }
    str.append("</tr>");
    for (i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
      FlightModeData *pd1=&g_model1.flightModeData[i];
      FlightModeData *pd2=&g_model2.flightModeData[i];
      str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
      color=getColor1(pd1->name,pd2->name);
      str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->name).arg(color));
      if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
        for (k=0; k<gvars; k++) {
          color=getColor1(pd1->gvars[k],pd2->gvars[k]);
          if (pd1->gvars[k]<=1024) {
            str.append(QString("<td align=\"right\" width=\"40\"><font size=+1 face='Courier New' color=%2>%1").arg(pd1->gvars[k]).arg(color)+"</font></td>");
          }
          else {
            int num = pd1->gvars[k] - 1025;
            if (num>=i) num++;
            str.append(QString("<td align=\"right\" width=\"40\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(num));
          }
        }
      }
      for (k=0; k<GetCurrentFirmware()->getCapability(RotaryEncoders); k++) {
        color=getColor1(pd1->rotaryEncoders[k],pd2->rotaryEncoders[k]);
        if (pd1->rotaryEncoders[k]<=1024) {
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=%2>%1").arg(pd1->rotaryEncoders[k]).arg(color)+"</font></td>");
        }
        else {
          int num = pd1->rotaryEncoders[k] - 1025;
          if (num>=i) num++;
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(num));
        }
      }
      str.append("</tr>");
    }
    str.append("</table>");
  }
  str.append("</td>");

  str.append("<td  width=\"50%\"><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  str.append("<tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>");
  str.append(tr("Fades")+"</b></td><td colspan=4 align=center><b>"+tr("Trims"));
  str.append("</b></td><td rowspan=2 align=\"center\" valign=\"bottom\"><b>"+tr("Switch")+"</b></td></tr><tr><td align=center width=\"80\"><b>"+tr("Flight mode name"));
  str.append("</b></td><td align=center width=\"30\"><b>"+tr("IN")+"</b></td><td align=center width=\"30\"><b>"+tr("OUT")+"</b></td>");
  for (i=0; i<4; i++) {
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(modelPrinter2.printInputName(i)));
  }
  str.append("</tr>");
  for (i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
    FlightModeData *pd1=&g_model1.flightModeData[i];
    FlightModeData *pd2=&g_model2.flightModeData[i];
    str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
    color=getColor2(pd1->name,pd2->name);
    str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->name).arg(color));
    color=getColor2(pd1->fadeIn,pd2->fadeIn);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->fadeIn).arg(color));
    color=getColor2(pd1->fadeOut,pd2->fadeOut);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->fadeOut).arg(color));
    for (k=0; k<4; k++) {
      if (pd2->trimRef[k]==-1) {
        color=getColor2(pd1->trim[k],pd2->trim[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->trim[k]).arg(color));
      } else {
        color=getColor2(pd1->trimRef[k],pd2->trimRef[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(pd2->trimRef[k]));
      }
    }
    color=getColor2(pd1->swtch,pd2->swtch);
    str.append(QString("<td align=center><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->swtch.toString()).arg(color));
    str.append("</tr>");
  }
  str.append("</table>");
  
  if ((gvars && GetCurrentFirmware()->getCapability(GvarsFlightModes)) || GetCurrentFirmware()->getCapability(RotaryEncoders)) {
    str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    str.append("<tr><td style=\"border-style:none;\">&nbsp;</td>");
    if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(gvars)+tr("Gvars")+"</td>");
    }
    if (GetCurrentFirmware()->getCapability(RotaryEncoders)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(GetCurrentFirmware()->getCapability(RotaryEncoders))+tr("Rot. Enc.")+"</td>");
    }
    str.append("</tr><tr><td align=center ><b>"+tr("Flight mode name")+"</b></td>");
    if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
      for (i=0; i<gvars; i++) {
        str.append(QString("<td width=\"40\" align=\"center\"><b>GV%1</b><br>%2</td>").arg(i+1).arg(g_model2.gvars_names[i]));
      }
    }
    for (i=0; i<GetCurrentFirmware()->getCapability(RotaryEncoders); i++) {
      str.append(QString("<td align=\"center\"><b>RE%1</b></td>").arg((i==0 ? 'A': 'B')));
    }
    str.append("</tr>");
    for (i=0; i<GetCurrentFirmware()->getCapability(FlightModes); i++) {
      FlightModeData *pd1=&g_model1.flightModeData[i];
      FlightModeData *pd2=&g_model2.flightModeData[i];
      str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
      color=getColor1(pd1->name,pd2->name);
      str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->name).arg(color));
      if (GetCurrentFirmware()->getCapability(GvarsFlightModes)) {
        for (k=0; k<gvars; k++) {
          color=getColor1(pd1->gvars[k],pd2->gvars[k]);
          if (pd2->gvars[k]<=1024) {
            str.append(QString("<td align=\"right\" width=\"40\"><font size=+1 face='Courier New' color=%2>%1").arg(pd2->gvars[k]).arg(color)+"</font></td>");
          }
          else {
            int num = pd2->gvars[k] - 1025;
            if (num>=i) num++;
            str.append(QString("<td align=\"right\" width=\"40\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(num));
          }
        }
      }
      for (k=0; k<GetCurrentFirmware()->getCapability(RotaryEncoders); k++) {
        color=getColor1(pd1->rotaryEncoders[k],pd2->rotaryEncoders[k]);
        if (pd2->rotaryEncoders[k]<=1024) {
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=%2>%1").arg(pd2->rotaryEncoders[k]).arg(color)+"</font></td>");
        }
        else {
          int num = pd2->rotaryEncoders[k] - 1025;
          if (num>=i) num++;
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=%1>").arg(color)+tr("FM")+QString("%1</font></td>").arg(num));
        }
      }
      str.append("</tr>");
    }
    str.append("</table>");
  }  
  str.append("</td></tr></table>");
  te->append(str);
}

void CompareDialog::printLimits()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Limits")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  if (GetCurrentFirmware()->getCapability(HasChNames)) {
    str.append("<tr><td>"+tr("Name")+"</td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  } else {
    str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");    
  }
  for(int i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
    str.append("<tr>");
    if (GetCurrentFirmware()->getCapability(HasChNames)) {
      QString name1=g_model1.limitData[i].name;
      QString name2=g_model2.limitData[i].name;
      color=getColor1(name1,name2);
      if (name1.trimmed().isEmpty()) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),color,true));
      } else {
        str.append(doTC(name1,color,true));
      }
    } else {
      str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    }
    color=getColor1(g_model1.limitData[i].offset,g_model2.limitData[i].offset);
    str.append(doTR(g_model1.limitData[i].offsetToString(), color));
    color=getColor1(g_model1.limitData[i].min,g_model2.limitData[i].min);
    str.append(doTR(g_model1.limitData[i].minToString(), color));
    color=getColor1(g_model1.limitData[i].max,g_model2.limitData[i].max);
    str.append(doTR(g_model1.limitData[i].maxToString(), color));
    color=getColor1(g_model1.limitData[i].revert,g_model2.limitData[i].revert);
    str.append(doTR(QString(g_model1.limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td>");
  str.append("<td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  for(int i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
    str.append("<tr>");
    if (GetCurrentFirmware()->getCapability(HasChNames)) {
      QString name1=g_model1.limitData[i].name;
      QString name2=g_model2.limitData[i].name;
      color=getColor2(name1,name2);
      if (name2.trimmed().isEmpty()) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),color,true));
      } else {
        str.append(doTC(name2,color,true));
      }
    } else {
      str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    }
    color=getColor2(g_model1.limitData[i].offset,g_model2.limitData[i].offset);
    str.append(doTR(g_model2.limitData[i].offsetToString(), color));
    color=getColor2(g_model1.limitData[i].min,g_model2.limitData[i].min);
    str.append(doTR(g_model2.limitData[i].minToString(), color));
    color=getColor2(g_model1.limitData[i].max,g_model2.limitData[i].max);
    str.append(doTR(g_model2.limitData[i].maxToString(), color));
    color=getColor2(g_model1.limitData[i].revert,g_model2.limitData[i].revert);
    str.append(doTR(QString(g_model2.limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void CompareDialog::printGvars()
{
  QString color;
  int gvars = GetCurrentFirmware()->getCapability(Gvars);

  if (!GetCurrentFirmware()->getCapability(GvarsFlightModes) && gvars) {
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td colspan=2><h2>"+tr("Global Variables")+"</h2></td></tr>");
    str.append("<tr><td width=50%>");
    str.append("<table border=1 cellspacing=0 cellpadding=3 width=100>");
    FlightModeData *pd1=&g_model1.flightModeData[0];
    FlightModeData *pd2=&g_model2.flightModeData[0];
    int width = 100 / gvars;
    str.append("<tr>");
    for (int i=0; i<gvars; i++) {
      str.append(QString("<td width=\"%1%\" align=\"center\"><b>").arg(width)+tr("GV")+QString("%1</b></td>").arg(i+1));
    }
    str.append("</tr>");
    str.append("<tr>");
    for (int i=0; i<gvars; i++) {
      color=getColor1(pd1->gvars[i],pd2->gvars[i]);
      str.append(QString("<td width=\"%1%\" align=\"center\"><font color=%2>").arg(width).arg(color)+QString("%1</font></td>").arg(pd1->gvars[i]));
    }
    str.append("</tr>");
    str.append("</table></td><td width=50%>");
    str.append("<table border=1 cellspacing=0 cellpadding=3 width=100>");
    str.append("<tr>");
    for (int i=0; i<gvars; i++) {
      str.append(QString("<td width=\"%1%\" align=\"center\"><b>").arg(width)+tr("GV")+QString("%1</b></td>").arg(i+1));
    }
    str.append("</tr>");
    str.append("<tr>");
    for (int i=0; i<gvars; i++) {
      color=getColor2(pd1->gvars[i],pd2->gvars[i]);
      str.append(QString("<td width=\"%1%\" align=\"center\"><font color=%2>").arg(width).arg(color)+QString("%1</font></td>").arg(pd2->gvars[i]));
    }
    str.append("</tr>");
    str.append("</table></td>");
    str.append("</tr></table>");
    te->append(str);
  }
}

void CompareDialog::applyDiffFont(QString & v1, QString & v2, const QString & font, bool eqGrey)
{
  if (v1 != v2) {
    if (!v1.isEmpty()) v1 = addFont(v1, "green", "", font);
    if (!v2.isEmpty()) v2 = addFont(v2, "red"  , "", font);
  }
  else if (eqGrey) {
    if (!v1.isEmpty()) v1 = addFont(v1, "grey", "", font);
    if (!v2.isEmpty()) v2 = addFont(v2, "grey", "", font);
  }
}

void CompareDialog::diffAndAssembleTableLines(QStringList & i1, QStringList & i2, QString & o1, QString & o2)
{
  while(!i1.isEmpty() || !i2.isEmpty()) {
    QString l1, l2;
    if (!i1.isEmpty()) l1 = i1.takeFirst();
    if (!i2.isEmpty()) l2 = i2.takeFirst();
    applyDiffFont(l1, l2, "Courier New", true);
    if (!l1.isEmpty()) o1 += "<tr><td>" + l1 + "</td></tr>";
    if (!l2.isEmpty()) o2 += "<tr><td>" + l2 + "</td></tr>";
  }
}

void CompareDialog::printInputs()
{
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str += tr("Inputs");
  str += "</h2></td></tr><tr><td><table width=\"100%\" border=1 cellspacing=0 cellpadding=3>";
  for(uint8_t i=0; i<GetCurrentFirmware()->getCapability(Outputs); i++) {
    if (ChannelHasExpo(g_model1.expoData, i) || ChannelHasExpo(g_model2.expoData, i)) {

      QString name1, name2;
      name1 = modelPrinter1.printInputName(i);
      name2 = modelPrinter2.printInputName(i);
      applyDiffFont(name1, name2);

      QStringList lines1, lines2;
      for (int j=0; j<C9X_MAX_EXPOS; j++) {  
        ExpoData *ed1 = &g_model1.expoData[j];
        ExpoData *ed2 = &g_model2.expoData[j];
        if ((ed1->chn == i) && (ed1-> mode != 0)) lines1 << modelPrinter1.printInputLine(ed1);
        if ((ed2->chn == i) && (ed2-> mode != 0)) lines2 << modelPrinter2.printInputLine(ed2);
      }

      QString diffLines1, diffLines2;
      diffAndAssembleTableLines(lines1, lines2, diffLines1, diffLines2);
       
      str += "<tr>";
      str += "<td width=\"10%\" align=\"center\" valign=\"middle\" nowrap><b>" + name1 + "</b></td>";
      str += "<td width=\"40%\"><table border=0 cellspacing=0 cellpadding=0>" + diffLines1 + "</table></td>";
      str += "<td width=\"10%\" align=\"center\" valign=\"middle\" nowrap><b>" + name2 + "</b></td>";
      str += "<td width=\"40%\"><table border=0 cellspacing=0 cellpadding=0>" + diffLines2 + "</table></td>";
      str += "</tr>";
    }
  }
  str += "</table></td></tr></table>";
  te->append(str);
  // debugHtml(str);
}

void CompareDialog::printMixers()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str += tr("Mixers");
  str += "</h2></td></tr><tr><td><table border=1 cellspacing=0 cellpadding=3>";
  for(uint8_t i=1; i<=GetCurrentFirmware()->getCapability(Outputs); i++) {
    if (ChannelHasMix(g_model1.mixData, i) || ChannelHasMix(g_model2.mixData, i)) {

      QStringList lines1, lines2;
      for (int j=0; j<GetCurrentFirmware()->getCapability(Mixes); j++) {
        MixData *md1 = &g_model1.mixData[j];
        MixData *md2 = &g_model2.mixData[j];
        if (md1->destCh == i) lines1 << modelPrinter1.printMixerLine(md1);
        if (md2->destCh == i) lines2 << modelPrinter2.printMixerLine(md2);
      }

      QString diffLines1, diffLines2;
      diffAndAssembleTableLines(lines1, lines2, diffLines1, diffLines2);

      str += "<tr>";
      str += "<td width=\"47%\"><table border=0 cellspacing=0 cellpadding=0>"+ diffLines1 + "</table></td>";
      str += "<td width=\"6%\" align=\"center\" valign=\"middle\"><b>"+tr("CH")+QString("%1</b></td>").arg(i,2,10,QChar('0'));
      str += "<td width=\"47%\"><table border=0 cellspacing=0 cellpadding=0>"+ diffLines2 + "</table></td>";
      str += "</tr>";
    }
  }
  str += "</table></td></tr></table>";
  te->append(str);
  // debugHtml(str);
}

void CompareDialog::printCurves()
{
  int i,r,g,b,c,count1,count2,usedcurves=0;
  QString cm1y,cm1x, cm2y,cm2x;
  char buffer [16];
  QPen pen(Qt::black, 2, Qt::SolidLine);
  int numcurves=firmware->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }

  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Curves"));
  str.append("</h2></td></tr><tr><td>");
  str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">");
  QImage qi1(ISIZE+1,ISIZE+1,QImage::Format_RGB32);
  QPainter painter1(&qi1);
  QImage qi2(ISIZE+1,ISIZE+1,QImage::Format_RGB32);
  QPainter painter2(&qi2);
  painter1.setBrush(QBrush("#FFFFFF"));
  painter2.setBrush(QBrush("#FFFFFF"));
  painter1.setPen(QColor(0,0,0));
  painter2.setPen(QColor(0,0,0));
  painter1.drawRect(0,0,ISIZE,ISIZE);
  painter2.drawRect(0,0,ISIZE,ISIZE);
  
  for(i=0; i<numcurves; i++) {
    count1=0;
    for(int j=0; j<g_model1.curves[i].count; j++) {
      if (g_model1.curves[i].points[j].y!=0)
        count1++;
    }
    count2=0;
    for(int j=0; j<g_model2.curves[i].count; j++) {
      if (g_model2.curves[i].points[j].y!=0)
        count2++;
    }
    if ((count1>0) || (g_model1.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)|| (g_model1.curves[i].count !=5) ||
        (count2>0) || (g_model2.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)|| (g_model2.curves[i].count !=5)) {
      pen.setColor(colors[usedcurves]);
      painter1.setPen(pen);
      painter2.setPen(pen);

      colors[usedcurves].getRgb(&r,&g,&b);
      c=r;
      c*=256;
      c+=g;
      c*=256;
      c+=b;
      sprintf(buffer,"%06x",c);
      // curves are different in number of points or curve type so makes little sense to compare they are just different
      if ((g_model1.curves[i].count!=g_model2.curves[i].count) || (g_model1.curves[i].type!=g_model2.curves[i].type)) {        
        cm1y="[";
        cm1x="[";
        for(int j=0; j<g_model1.curves[i].count; j++) {
          cm1y.append(QString("%1").arg(g_model1.curves[i].points[j].y));
          cm1x.append(QString("%1").arg(g_model1.curves[i].points[j].x));
          if (j<(g_model1.curves[i].count-1)) {
            cm1y.append(",");
            cm1x.append(",");
          }
          if (g_model1.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)
            painter1.drawLine(ISIZE/2+(ISIZE*g_model1.curves[i].points[j-1].x)/200,ISIZE/2-(ISIZE*g_model1.curves[i].points[j-1].y)/200,ISIZE/2+(ISIZE*g_model1.curves[i].points[j].x)/200,ISIZE/2-(ISIZE*g_model1.curves[i].points[j].y)/200);
          else
            painter1.drawLine(ISIZE*(j-1)/(g_model1.curves[i].count-1),ISIZE/2-(ISIZE*g_model1.curves[i].points[j-1].y)/200,ISIZE*(j)/(g_model1.curves[i].count-1),ISIZE/2-(ISIZE*g_model1.curves[i].points[j].y)/200);
          
        }
        cm1y.append("]");
        cm1x.append("]");
        cm2y="[";
        cm2x="[";
        for(int j=0; j<g_model2.curves[i].count; j++) {
          cm2y.append(QString("%1").arg(g_model2.curves[i].points[j].y));
          cm2x.append(QString("%1").arg(g_model2.curves[i].points[j].x));
          if (j<(g_model2.curves[i].count-1)) {
            cm2y.append(",");
            cm2x.append(",");
          }
          if (g_model2.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)
            painter2.drawLine(ISIZE/2+(ISIZE*g_model2.curves[i].points[j-1].x)/200,ISIZE/2-(ISIZE*g_model2.curves[i].points[j-1].y)/200,ISIZE/2+(ISIZE*g_model2.curves[i].points[j].x)/200,ISIZE/2-(ISIZE*g_model2.curves[i].points[j].y)/200);
          else
            painter2.drawLine(ISIZE*(j-1)/(g_model2.curves[i].count-1),ISIZE/2-(ISIZE*g_model2.curves[i].points[j-1].y)/200,ISIZE*(j)/(g_model2.curves[i].count-1),ISIZE/2-(ISIZE*g_model2.curves[i].points[j].y)/200);
          
        }
        cm2y.append("]");
        cm2x.append("]");
        str.append("<tr><td nowrap width=\"45%\"><font color=green>");
        str.append(cm1y);
        if ((g_model1.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)) {
          str.append(QString("<br>")+cm1x+QString("</font></td>"));
        }      
        str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
        str.append("<td nowrap width=\"45%\"><font color=red>");
        str.append(cm2y);
        if ((g_model2.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)) {
          str.append(QString("<br>")+cm2x+QString("</font></td></tr>"));
        }      
      } else {
      // curves have the same number of points and the same type, we can compare them point by point
        cm1y="[";
        cm1x="[";
        cm2y="[";
        cm2x="[";
        for(int j=0; j<g_model1.curves[i].count; j++) {
          if (g_model1.curves[i].points[j].y!=g_model2.curves[i].points[j].y) {
            cm1y.append(QString("<font color=green>%1</font>").arg(g_model1.curves[i].points[j].y));
            cm2y.append(QString("<font color=red>%1</font>").arg(g_model2.curves[i].points[j].y));
          } else {
            cm1y.append(QString("<font color=grey>%1</font>").arg(g_model1.curves[i].points[j].y));
            cm2y.append(QString("<font color=grey>%1</font>").arg(g_model2.curves[i].points[j].y));            
          }
          if (g_model1.curves[i].points[j].x!=g_model2.curves[i].points[j].x) {
            cm1x.append(QString("<font color=green>%1</font>").arg(g_model1.curves[i].points[j].x));
            cm2x.append(QString("<font color=red>%1</font>").arg(g_model2.curves[i].points[j].x));
          } else {
            cm1x.append(QString("<font color=grey>%1</font>").arg(g_model1.curves[i].points[j].x));
            cm2x.append(QString("<font color=grey>%1</font>").arg(g_model2.curves[i].points[j].x));            
          }
          if (j<(g_model1.curves[i].count-1)) {
            cm1y.append(",");
            cm1x.append(",");
            cm2y.append(",");
            cm2x.append(",");
          }
          if (g_model1.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)
            painter1.drawLine(ISIZE/2+(ISIZE*g_model1.curves[i].points[j-1].x)/200,ISIZE/2-(ISIZE*g_model1.curves[i].points[j-1].y)/200,ISIZE/2+(ISIZE*g_model1.curves[i].points[j].x)/200,ISIZE/2-(ISIZE*g_model1.curves[i].points[j].y)/200);
          else
            painter1.drawLine(ISIZE*(j-1)/(g_model1.curves[i].count-1),ISIZE/2-(ISIZE*g_model1.curves[i].points[j-1].y)/200,ISIZE*(j)/(g_model1.curves[i].count-1),ISIZE/2-(ISIZE*g_model1.curves[i].points[j].y)/200);
          if (g_model2.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)
            painter2.drawLine(ISIZE/2+(ISIZE*g_model2.curves[i].points[j-1].x)/200,ISIZE/2-(ISIZE*g_model2.curves[i].points[j-1].y)/200,ISIZE/2+(ISIZE*g_model2.curves[i].points[j].x)/200,ISIZE/2-(ISIZE*g_model2.curves[i].points[j].y)/200);
          else
            painter2.drawLine(ISIZE*(j-1)/(g_model2.curves[i].count-1),ISIZE/2-(ISIZE*g_model2.curves[i].points[j-1].y)/200,ISIZE*(j)/(g_model2.curves[i].count-1),ISIZE/2-(ISIZE*g_model2.curves[i].points[j].y)/200);          
        }
        painter1.setPen(QColor(0,0,0));
        painter2.setPen(QColor(0,0,0));
        painter1.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
        painter2.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
        painter1.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
        painter2.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
        for(i=0; i<21; i++) {
          painter1.drawLine(ISIZE/2-5,(ISIZE*i)/(20),ISIZE/2+5,(ISIZE*i)/(20));
          painter2.drawLine(ISIZE/2-5,(ISIZE*i)/(20),ISIZE/2+5,(ISIZE*i)/(20));
          painter1.drawLine((ISIZE*i)/(20),ISIZE/2-5,(ISIZE*i)/(20),ISIZE/2+5);
          painter2.drawLine((ISIZE*i)/(20),ISIZE/2-5,(ISIZE*i)/(20),ISIZE/2+5);
        }

        cm1y.append("]");
        cm1x.append("]");
        cm2y.append("]");
        cm2x.append("]");
        str.append("<tr><td nowrap width=\"45%\">");
        str.append(cm1y);
        if ((g_model1.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)) {
          str.append(QString("<br>")+cm1x+QString("</td>"));
        }      
        str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
        str.append("<td nowrap width=\"45%\">");
        str.append(cm2y);
        if ((g_model2.curves[i].type == CurveData::CURVE_TYPE_CUSTOM)) {
          str.append(QString("<br>")+cm2x+QString("</td></tr>"));
        }      
      }
      usedcurves++;
    }
  }
  if (usedcurves>0) {
    str.append(QString("<tr><td width=45 align=center><img src=\"%1\" border=0></td><td>&nbsp;</td><td width=45 align=center><img src=\"%2\" border=0></td>").arg(curvefile1).arg(curvefile2));
    str.append("</table></td></tr></table>");
    qi1.save(curvefile1, "png",100); 
    qi2.save(curvefile2, "png",100); 
    te->append(str);
  }
}

void CompareDialog::printLogicalSwitches()
{
  bool haveOutput = false;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str += "<tr><td><h2>" + tr("Logical Switches") + "</h2></td></tr>";
  str += "<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">";
  for (int i=0; i<GetCurrentFirmware()->getCapability(LogicalSwitches); i++) {
    QString l1 = modelPrinter1.printLogicalSwitchLine(i);
    QString l2 = modelPrinter2.printLogicalSwitchLine(i);
    if (!(l1.isEmpty() && l2.isEmpty())) {
      applyDiffFont(l1, l2);
      str += "<tr>";
      str += "<td  width=\"45%\">"+ l1 + "</td>";
      str += "<td align=\"center\" width=\"10%\"><b>" + tr("L") + QString("%1</b></td>").arg(i+1);
      str += "<td  width=\"45%\">" + l2 + "</td>";
      str += "</tr>";
      haveOutput = true;
    }
  }
  str += "</table></td></tr></table>";
  if (haveOutput) te->append(str);
}

void CompareDialog::printCustomFunctions()
{
  bool haveOutput = false;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str += "<tr><td><h2>" + tr("Special Functions") + "</h2></td></tr>";
  str += "<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">";
  for (int i=0; i<GetCurrentFirmware()->getCapability(CustomFunctions); i++) {
    QString l1 = modelPrinter1.printCustomFunctionLine(i);
    QString l2 = modelPrinter2.printCustomFunctionLine(i);
    if (!(l1.isEmpty() && l2.isEmpty())) {
      applyDiffFont(l1, l2);
      str += "<tr>";
      str += "<td  width=\"45%\">"+ l1 + "</td>";
      str += "<td align=\"center\" width=\"10%\"><b>" + tr("SF") + QString("%1</b></td>").arg(i+1);
      str += "<td  width=\"45%\">" + l2 + "</td>";
      str += "</tr>";
      haveOutput = true;
    }
  }
  str += "</table></td></tr></table>";
  if (haveOutput) te->append(str);
}

void CompareDialog::printFrSky()
{
  QString color;
  float value1,value2;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Telemetry Settings")+"</h2></td></tr>");
  str.append("<tr><td width=\"50%\">");
  str.append("<table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  FrSkyData *fd1=&g_model1.frsky;
  FrSkyData *fd2=&g_model2.frsky;
  str.append("<tr><td align=\"center\" width=\"22%\"><b>"+tr("Analog")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Unit")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Scale")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Offset")+"</b></td></tr>");
  for (int i=0; i<2; i++) {
    str.append("<tr>");
    float ratio=(fd1->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    str.append("<td align=\"center\"><b>"+tr("A%1").arg(i+1)+"</b></td>");
    color=getColor1(fd1->channels[i].type,fd2->channels[i].type);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyUnits(fd1->channels[i].type)+"</font></td>");
    color=getColor1(fd1->channels[i].ratio,fd2->channels[i].ratio);
    str.append("<td align=\"center\"><font color="+color+">"+QString::number(ratio,10,(fd1->channels[i].type==0 ? 1:0))+"</font></td>");
    color=getColor1(fd1->channels[i].offset*fd1->channels[i].ratio,fd2->channels[i].offset*fd2->channels[i].ratio);
    str.append("<td align=\"center\"><font color="+color+">"+QString::number((fd1->channels[i].offset*ratio)/255,10,(fd1->channels[i].type==0 ? 1:0))+"</font></td>");
    str.append("</tr>");
  }
  str.append("</table><br>");
  str.append("<table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  str.append("<tr><td></td><td colspan=\"3\" align=\"center\"><b>"+tr("Alarm 1")+"</b></td><td colspan=\"3\" align=\"center\"><b>"+tr("Alarm 2")+"</b></td>");
  str.append("<tr><td width=\"22%\"></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Type")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Condition")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Type")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Condition")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Value")+"</b></td></tr>");
  for (int i=0; i<2; i++) {
    float ratio1=(fd1->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    float ratio2=(fd1->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    str.append("<tr>");
    str.append("<td align=\"center\"><b>"+tr("A%1").arg(i+1)+"</b></td>");
    color=getColor1(fd1->channels[i].alarms[0].level,fd2->channels[i].alarms[0].level);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd1->channels[i].alarms[0].level)+"</font></td>");
    color=getColor1(fd1->channels[i].alarms[0].greater,fd2->channels[i].alarms[0].greater);
    str.append("<td align=\"center\"><font color="+color+">");
    str.append((fd1->channels[i].alarms[0].greater==1) ? "&gt;" : "&lt;");
    value1=ratio1*(fd1->channels[i].alarms[0].value/255.0+fd1->channels[i].offset/255.0);
    value2=ratio2*(fd2->channels[i].alarms[0].value/255.0+fd2->channels[i].offset/255.0);
    color=getColor1(value1,value2);
    str.append("</font></td><td align=\"center\"><font color="+color+">"+QString::number(value1,10,(fd1->channels[i].type==0 ? 1:0))+"</font></td>");
    color=getColor1(fd1->channels[i].alarms[1].level,fd2->channels[i].alarms[1].level);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd1->channels[i].alarms[1].level)+"</font></td>");
    color=getColor1(fd1->channels[i].alarms[1].greater,fd2->channels[i].alarms[1].greater);
    str.append("<td align=\"center\"><font color="+color+">");
    str.append((fd1->channels[i].alarms[1].greater==1) ? "&gt;" : "&lt;");
    value1=ratio1*(fd1->channels[i].alarms[1].value/255.0+fd1->channels[i].offset/255.0);
    value2=ratio2*(fd2->channels[i].alarms[1].value/255.0+fd2->channels[i].offset/255.0);
    color=getColor1(value1,value2);
    str.append("</font></td><td align=\"center\"><font color="+color+">"+QString::number(value1,10,(fd1->channels[i].type==0 ? 1:0))+"</font></td></tr>");
  }
  str.append("<tr><td align=\"center\"><b>"+tr("RSSI Alarm")+"</b></td>");
  color=getColor1(fd1->rssiAlarms[0].level,fd2->rssiAlarms[0].level);
  str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd1->rssiAlarms[0].level)+"</td>");
  str.append("<td align=\"center\">&lt;</td>");
  color=getColor1(fd1->rssiAlarms[0].value,fd2->rssiAlarms[0].value);
  str.append("<td align=\"center\"><font color="+color+">"+QString::number(fd1->rssiAlarms[0].value,10)+"</td>");
  color=getColor1(fd1->rssiAlarms[1].level,fd2->rssiAlarms[1].level);
  str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd1->rssiAlarms[1].level)+"</td>");
  str.append("<td align=\"center\">&lt;</td>");
  color=getColor1(fd1->rssiAlarms[1].value,fd2->rssiAlarms[1].value);
  str.append("<td align=\"center\"><font color="+color+">"+QString::number(fd1->rssiAlarms[1].value,10)+"</td>");
  str.append("</table>");
#if 0
  if (GetCurrentFirmware()->getCapability(TelemetryBars) || GetCurrentFirmware()->getCapability(TelemetryCSFields)) {
    int cols=GetCurrentFirmware()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;
    for (int j=0; j<GetCurrentFirmware()->getCapability(TelemetryCSFields)/(4*cols); j++) {
      QString tcols;
      QString cwidth;
      QString swidth;
      if (cols==2) {
        tcols="3";
        cwidth="45";
        swidth="10";
      } else {
        tcols="5";
        cwidth="30";
        swidth="5";
      }
      color=getColor1(fd1->screens[j].type,fd2->screens[j].type);
      if (fd1->screens[j].type==0) {
        str.append("<br><table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan="+tcols+" align=\"Left\"><b><font color="+color+">"+tr("Custom Telemetry View")+"</font></b></td></tr><tr><td colspan=3>&nbsp;</td></tr>");
        for (int r=0; r<4; r++) {
          str.append("<tr>");
          for (int c=0; c<cols; c++) {
            if (fd1->screens[j].type==fd2->screens[j].type) 
              color=getColor1(fd1->screens[j].body.lines[r].source[c],fd2->screens[j].body.lines[r].source[c]);
            str.append("<td  align=\"Center\" width=\""+cwidth+"%\"><font color="+color+">"+getFrSkySrc(fd1->screens[j].body.lines[r].source[c])+"</font></td>");
            if (c<(cols-1)) {
              str.append("<td  align=\"Center\" width=\""+swidth+"%\"><b>&nbsp;</b></td>");
            }
          }
          str.append("</tr>");  
        }
        str.append("</table>");        
      } else {
        str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr><td colspan=4 align=\"Left\"><b><font color="+color+">"+tr("Telemetry Bars")+"</font></b></td></tr>");
        str.append("<tr><td  align=\"Center\"><b>"+tr("Bar Number")+"</b></td><td  align=\"Center\"><b>"+tr("Source")+"</b></td><td  align=\"Center\"><b>"+tr("Min")+"</b></td><td  align=\"Center\"><b>"+tr("Max")+"</b></td></tr>");
        for (int i=0; i<4; i++) {
          str.append("<tr><td  align=\"Center\"><b>"+QString::number(i+1,10)+"</b></td>");
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor1(fd1->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].source);
          str.append("<td  align=\"Center\"><font color="+color+">"+getFrSkySrc(fd1->screens[0].body.bars[i].source)+"</font></td>");
          // TODO value1 = getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMin,fd1);
          // TODO value2 = getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMin,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor1(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value1)+"</td>");
          // TODO value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMax,fd1);
          // TODO value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMax,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
           color=getColor1(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value1)+"</td></tr>");
        }
        str.append("</table>");
      }
    }
  }
#endif
  
  str.append("</td><td width=\"50%\">");
  str.append("<table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  str.append("<tr><td align=\"center\" width=\"22%\"><b>"+tr("Analog")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Unit")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Scale")+"</b></td><td align=\"center\" width=\"26%\"><b>"+tr("Offset")+"</b></td></tr>");
  for (int i=0; i<2; i++) {
    str.append("<tr>");
    float ratio=(fd2->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    str.append("<td align=\"center\"><b>"+tr("A%1").arg(i+1)+"</b></td>");
    color=getColor2(fd1->channels[i].type,fd2->channels[i].type);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyUnits(fd2->channels[i].type)+"</font></td>");
    color=getColor2(fd1->channels[i].ratio,fd2->channels[i].ratio);
    str.append("<td align=\"center\"><font color="+color+">"+QString::number(ratio,10,(fd2->channels[i].type==0 ? 1:0))+"</font></td>");
    color=getColor2(fd1->channels[i].offset*fd1->channels[i].ratio,fd2->channels[i].offset*fd2->channels[i].ratio);
    str.append("<td align=\"center\"><font color="+color+">"+QString::number((fd2->channels[i].offset*ratio)/255,10,(fd2->channels[i].type==0 ? 1:0))+"</font></td>");
    str.append("</tr>");
  }
  str.append("</table><br>");
  str.append("<table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  str.append("<tr><td></td><td colspan=\"3\" align=\"center\"><b>"+tr("Alarm 1")+"</b></td><td colspan=\"3\" align=\"center\"><b>"+tr("Alarm 2")+"</b></td>");
  str.append("<tr><td width=\"22%\"></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Type")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Condition")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Type")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Condition")+"</b></td>");
  str.append("<td width=\"13%\" align=\"center\"><b>"+tr("Value")+"</b></td></tr>");
  for (int i=0; i<2; i++) {
    float ratio1=(fd1->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    float ratio2=(fd1->channels[i].ratio/(fd1->channels[i].type==0 ?10.0:1));
    str.append("<tr>");
    str.append("<td align=\"center\"><b>"+tr("A%1").arg(i+1)+"</b></td>");
    color=getColor2(fd1->channels[i].alarms[0].level,fd2->channels[i].alarms[0].level);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd2->channels[i].alarms[0].level)+"</font></td>");
    color=getColor2(fd1->channels[i].alarms[0].greater,fd2->channels[i].alarms[0].greater);
    str.append("<td align=\"center\"><font color="+color+">");
    str.append((fd2->channels[i].alarms[0].greater==1) ? "&gt;" : "&lt;");
    value1=ratio1*(fd1->channels[i].alarms[0].value/255.0+fd1->channels[i].offset/255.0);
    value2=ratio2*(fd2->channels[i].alarms[0].value/255.0+fd2->channels[i].offset/255.0);
    color=getColor2(value1,value2);
    str.append("</font></td><td align=\"center\"><font color="+color+">"+QString::number(value2,10,(fd2->channels[i].type==0 ? 1:0))+"</font></td>");
    color=getColor2(fd1->channels[i].alarms[1].level,fd2->channels[i].alarms[1].level);
    str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd2->channels[i].alarms[1].level)+"</font></td>");
    color=getColor2(fd1->channels[i].alarms[1].greater,fd2->channels[i].alarms[1].greater);
    str.append("<td align=\"center\"><font color="+color+">");
    str.append((fd2->channels[i].alarms[1].greater==1) ? "&gt;" : "&lt;");
    value1=ratio1*(fd1->channels[i].alarms[1].value/255.0+fd1->channels[i].offset/255.0);
    value2=ratio2*(fd2->channels[i].alarms[1].value/255.0+fd2->channels[i].offset/255.0);
    color=getColor2(value1,value2);
    str.append("</font></td><td align=\"center\"><font color="+color+">"+QString::number(value2,10,(fd2->channels[i].type==0 ? 1:0))+"</font></td></tr>");
  }
  str.append("<tr><td align=\"Center\"><b>"+tr("RSSI Alarm")+"</b></td>");
  color=getColor2(fd1->rssiAlarms[0].level,fd2->rssiAlarms[0].level);
  str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd2->rssiAlarms[0].level)+"</td>");
  str.append("<td align=\"center\">&lt;</td>");
  color=getColor2(fd1->rssiAlarms[0].value,fd2->rssiAlarms[0].value);
  str.append("<td align=\"center\"><font color="+color+">"+QString::number(fd2->rssiAlarms[0].value,10)+"</td>");
  color=getColor2(fd1->rssiAlarms[1].level,fd2->rssiAlarms[1].level);
  str.append("<td align=\"center\"><font color="+color+">"+getFrSkyAlarmType(fd2->rssiAlarms[1].level)+"</td>");
  str.append("<td align=\"center\">&lt;</td>");
  color=getColor2(fd1->rssiAlarms[1].value,fd2->rssiAlarms[1].value);
  str.append("<td align=\"center\"><font color="+color+">"+QString::number(fd2->rssiAlarms[1].value,10)+"</td>");
  str.append("</table></br>");
#if 0
  if (GetCurrentFirmware()->getCapability(TelemetryBars) || GetCurrentFirmware()->getCapability(TelemetryCSFields)) {
    int cols=GetCurrentFirmware()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;
    for (int j=0; j<GetCurrentFirmware()->getCapability(TelemetryCSFields)/(4*cols); j++) {
      QString tcols;
      QString cwidth;
      QString swidth;
      if (cols==2) {
        tcols="3";
        cwidth="45";
        swidth="10";
      } else {
        tcols="5";
        cwidth="30";
        swidth="5";
      }
      color=getColor2(fd1->screens[j].type,fd2->screens[j].type);
      if (fd2->screens[j].type==0) {
        str.append("<br><table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan="+tcols+" align=\"Left\"><b><font color="+color+">"+tr("Custom Telemetry View")+"</font></b></td></tr><tr><td colspan=3>&nbsp;</td></tr>");
        for (int r=0; r<4; r++) {
          str.append("<tr>");
          for (int c=0; c<cols; c++) {
            if (fd1->screens[j].type==fd2->screens[j].type) 
              color=getColor2(fd1->screens[j].body.lines[r].source[c],fd2->screens[j].body.lines[r].source[c]);
            str.append("<td  align=\"Center\" width=\""+cwidth+"%\"><font color="+color+">"+getFrSkySrc(fd2->screens[j].body.lines[r].source[c])+"</font></td>");
            if (c<(cols-1)) {
              str.append("<td  align=\"Center\" width=\""+swidth+"%\"><b>&nbsp;</b></td>");
            }
          }
          str.append("</tr>");  
        }
        str.append("</table>");        
      } else {
        str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr><td colspan=4 align=\"Left\"><b><font color="+color+">"+tr("Telemetry Bars")+"</b></td></tr>");
        str.append("<tr><td  align=\"Center\"><b>"+tr("Bar Number")+"</b></td><td  align=\"Center\"><b>"+tr("Source")+"</b></td><td  align=\"Center\"><b>"+tr("Min")+"</b></td><td  align=\"Center\"><b>"+tr("Max")+"</b></td></tr>");
        for (int i=0; i<4; i++) {
          str.append("<tr><td  align=\"Center\"><b>"+QString::number(i+1,10)+"</b></td>");
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor2(fd1->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].source);
          str.append("<td  align=\"Center\"><font color="+color+">"+getFrSkySrc(fd2->screens[0].body.bars[i].source)+"</font></td>");
          // TODO value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMin,fd1);
          // TODO value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMin,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor2(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value2)+"</font></td>");
          // TODO value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMax,fd1);
          // TODO value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMax,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor2(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value2)+"</font></td></tr>");
        }
        str.append("</table>");
      }
    }
  }
#endif
  str.append("</td></tr></table>");
  te->append(str);
}

void CompareDialog::on_printButton_clicked()
{
    QPrinter printer;
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    te->print(&printer);
}

void CompareDialog::on_printFileButton_clicked()
{
    QPrinter printer;
    QString filename = QFileDialog::getSaveFileName(this,tr("Select PDF output file"),QString(),"Pdf File(*.pdf)"); 
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setColorMode(QPrinter::Color);
    if(!filename.isEmpty()) { 
        if(QFileInfo(filename).suffix().isEmpty()) 
            filename.append(".pdf"); 
        printer.setOutputFileName(filename);
        te->print(&printer);
    }
}
