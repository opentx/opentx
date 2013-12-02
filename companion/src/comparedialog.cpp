#include "comparedialog.h"
#include "ui_comparedialog.h"
#include "helpers.h"
#include "eeprominterface.h"
#include <QtGui>
#include <QImage>
#include <QColor>
#include <QPainter>

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
  uint8_t models[C9XMAX_MODELS];
};

compareDialog::compareDialog(QWidget *parent, GeneralSettings *gg) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::compareDialog)
{
  ui->setupUi(this);
  g_eeGeneral = gg;
  eepromInterface = GetEepromInterface();
  te = ui->textEdit;
  this->setAcceptDrops(true);
  model1=0;
  model2=0;
  g_model1=(ModelData *)malloc(sizeof(ModelData));
  g_model2=(ModelData *)malloc(sizeof(ModelData));
  modeltemp=(ModelData *)malloc(sizeof(ModelData));
    //setDragDropOverwriteMode(true);
    //setDropIndicatorShown(true);
/*
    printFrSky();
*/    
    te->scrollToAnchor("1");
}

void compareDialog::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasFormat("application/x-companion9x")) {   
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void compareDialog::dragEnterEvent(QDragEnterEvent *event)
{
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("application/x-companion9x")) {   
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void compareDialog::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void compareDialog::printDiff()
{
  te->clear();
  printSetup();
  if (GetEepromInterface()->getCapability(FlightPhases)) {
    printPhases();
  }
  printExpos();
  printMixers();
  printLimits();
  printCurves();
  printGvars();
  printSwitches();
  printSafetySwitches();
  printFSwitches();
  printFrSky();
  te->scrollToAnchor("1");
}

void compareDialog::dropEvent(QDropEvent *event)
{
  QLabel *child = qobject_cast<QLabel*>(childAt(event->pos()));
  const QMimeData  *mimeData = event->mimeData();
  if (child) {
    if (child->objectName().contains("label_1")) {        
      if(mimeData->hasFormat("application/x-companion9x")) {
        QByteArray gmData = mimeData->data("application/x-companion9x");
        DragDropHeader *header = (DragDropHeader *)gmData.data();
        if (!header->general_settings) {
          char *gData = gmData.data()+sizeof(DragDropHeader);//new char[gmData.size() + 1];
          char c = *gData;
          gData++;
          if(c=='M') {
            memcpy(modeltemp,(ModelData *)gData,sizeof(ModelData));
            if (modeltemp->used) {
              memcpy(g_model1,(ModelData *)gData,sizeof(ModelData));
              QString name;
              name.append(g_model1->name);
              if (!name.isEmpty()) {
                ui->label_1->setText(name);
              } else {
                ui->label_1->setText(tr("No name"));
              }
              model1=1;
            }
          }
        }
      }          
    }
    else if (child->objectName().contains("label_2")) {
      if(mimeData->hasFormat("application/x-companion9x")) {
        QByteArray gmData = mimeData->data("application/x-companion9x");
        DragDropHeader *header = (DragDropHeader *)gmData.data();
        if (!header->general_settings) {
          char *gData = gmData.data()+sizeof(DragDropHeader);//new char[gmData.size() + 1];
          char c = *gData;
          gData++;
          if(c=='M') {
            memcpy(modeltemp,(ModelData *)gData,sizeof(ModelData));
            if (modeltemp->used) {
              memcpy(g_model2,(ModelData *)gData,sizeof(ModelData));
              QString name;
              name.append(g_model2->name);
              if (!name.isEmpty()) {
                ui->label_2->setText(name);
              } else {
                ui->label_2->setText(tr("No name"));
              }
              model2=1;
            }
          }
        }
      }                  
    }
  }  else {
    return;
  }
  event->accept();
  if ((model1==1) & (model2==1)) {
    printDiff();
  }
}

void compareDialog::closeEvent(QCloseEvent *event) 
{
  QByteArray ba = curvefile5.toLatin1();
  char *name = ba.data(); 
  unlink(name);
  ba = curvefile9.toLatin1();
  name = ba.data(); 
  unlink(name);    
}

compareDialog::~compareDialog()
{
  delete ui;
}

QString compareDialog::doTC(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=center>" + str + "</td>";
}

QString compareDialog::doTR(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=right>" + str + "</td>";
}

QString compareDialog::doTL(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=left>" + str + "</td>";
}

QString compareDialog::fv(const QString name, const QString value, const QString color="green")
{
    return "<b>" + name + ": </b><font color=" +color + ">" + value + "</font><br>";
}



int compareDialog::ModelHasExpo(ExpoData * ExpoArray, ExpoData expo, bool * expoused)
{
  for (int i=0; i< C9X_MAX_EXPOS; i++) {
    if ((memcmp(&expo,&ExpoArray[i],sizeof(ExpoData))==0) && (expoused[i]==false)) {
      return i;
    }
  }
  return -1;
}

bool compareDialog::ChannelHasExpo(ExpoData * expoArray, uint8_t destCh)
{
  for (int i=0; i< C9X_MAX_EXPOS; i++) {
    if ((expoArray[i].chn==destCh)&&(expoArray[i].mode!=0)) {
      return true;
    }
  }
  return false;
}

int compareDialog::ModelHasMix(MixData * mixArray, MixData mix, bool * mixused)
{
  for (int i=0; i< C9X_MAX_MIXERS; i++) {
    if ((memcmp(&mix,&mixArray[i],sizeof(MixData))==0) && (mixused[i]==false)) {
      return i;
    }
  }
  return -1;
}

bool compareDialog::ChannelHasMix(MixData * mixArray, uint8_t destCh)
{
  for (int i=0; i< C9X_MAX_MIXERS; i++) {
    if (mixArray[i].destCh==destCh) {
      return true;
    }
  }
  return false;
}

void compareDialog::printSetup()
{
  QString color;
  QString str = "<a name=1></a><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("General Model Settings")+"</h2></td></tr>");
  str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor1(g_model1->name,g_model2->name);
  str.append(fv(tr("Name"), g_model1->name, color));
  color=getColor1(eepromInterface->getSize(*g_model1),eepromInterface->getSize(*g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(eepromInterface->getSize(*g_model1)).arg(color));
  color=getColor1(getTimerStr(g_model1->timers[0]), getTimerStr(g_model2->timers[0]));
  str.append(fv(tr("Timer1"), getTimerStr(g_model1->timers[0]), color));  //value, mode, count up/down
  color=getColor1(getTimerStr(g_model1->timers[1]), getTimerStr(g_model2->timers[1]));
  str.append(fv(tr("Timer2"), getTimerStr(g_model1->timers[1]), color));  //value, mode, count up/down
  color=getColor1(getProtocol(g_model1),getProtocol(g_model2));
  str.append(fv(tr("Protocol"), getProtocol(g_model1), color)); //proto, numch, delay,
  color=getColor1(g_model1->moduleData[0].ppmPulsePol,g_model2->moduleData[0].ppmPulsePol);
  str.append(fv(tr("Pulse Polarity"), g_model1->moduleData[0].ppmPulsePol ? "NEG" : "POS", color));
  color=getColor1(g_model1->thrTrim,g_model2->thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model1->thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor1(g_model1->thrExpo,g_model2->thrExpo);
  str.append(fv(tr("Throttle Expo"), g_model1->thrExpo ? tr("Enabled") : tr("Disabled"),color));
  // TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
  color=getColor1(getTrimInc(g_model1),getTrimInc(g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(g_model1),color));
  color=getColor1(getCenterBeep(g_model1),getCenterBeep(g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeep(g_model1),color)); // specify which channels beep
  str.append("</table></td>");
  str.append("<td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor2(g_model1->name,g_model2->name);
  str.append(fv(tr("Name"), g_model2->name, color));
  color=getColor2(eepromInterface->getSize(*g_model1),eepromInterface->getSize(*g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(eepromInterface->getSize(*g_model2)).arg(color));
  color=getColor2(getTimerStr(g_model1->timers[0]), getTimerStr(g_model2->timers[0]));
  str.append(fv(tr("Timer1"), getTimerStr(g_model2->timers[0]),color));  //value, mode, count up/down
  color=getColor2(getTimerStr(g_model1->timers[1]), getTimerStr(g_model2->timers[1]));
  str.append(fv(tr("Timer2"), getTimerStr(g_model2->timers[1]),color));  //value, mode, count up/down
  color=getColor2(getProtocol(g_model1),getProtocol(g_model2));
  str.append(fv(tr("Protocol"), getProtocol(g_model2), color)); //proto, numch, delay,
  color=getColor2(g_model1->moduleData[0].ppmPulsePol,g_model2->moduleData[0].ppmPulsePol);
  str.append(fv(tr("Pulse Polarity"), g_model2->moduleData[0].ppmPulsePol ? "NEG" : "POS", color));
  color=getColor2(g_model1->thrTrim,g_model2->thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model2->thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor2(g_model1->thrExpo,g_model2->thrExpo);
  str.append(fv(tr("Throttle Expo"), g_model2->thrExpo ? tr("Enabled") : tr("Disabled"),color));
  // TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
  color=getColor2(getTrimInc(g_model1),getTrimInc(g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(g_model2),color));
  color=getColor2(getCenterBeep(g_model1),getCenterBeep(g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeep(g_model2),color)); // specify which channels beep
  str.append("</td></tr></table></td></tr></table>");
  te->append(str);
}

void compareDialog::printPhases()
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
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(getStickStr(i)));
  }
  str.append("</tr>");
  for (i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
    PhaseData *pd1=&g_model1->phaseData[i];
    PhaseData *pd2=&g_model2->phaseData[i];
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
  int gvars=0;
  int gvarnum=0;
  if (GetEepromInterface()->getCapability(HasVariants)) {
    if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
      gvars=1;
    }
  } else {
    gvars=1;
  }
  if (gvars==1) {
    gvarnum=GetEepromInterface()->getCapability(GvarsNum);
  }
  if ((gvars==1 && GetEepromInterface()->getCapability(GvarsFlightPhases)) || GetEepromInterface()->getCapability(RotaryEncoders)) {
    str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    str.append("<tr><td style=\"border-style:none;\">&nbsp;</td>");
    if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
      
      str.append(QString("<td colspan=%1 align=center><b>").arg(gvarnum)+tr("Gvars")+"</td>");
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(GetEepromInterface()->getCapability(RotaryEncoders))+tr("Rot. Enc.")+"</td>");
    }
    str.append("</tr><tr><td align=center><b>"+tr("Flight mode name")+"</b></td>");
    if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
      for (i=0; i<gvarnum; i++) {
        str.append(QString("<td width=\"40\" align=\"center\"><b>GV%1</b><br>%2</td>").arg(i+1).arg(g_model1->gvars_names[i]));
      }
    }
    for (i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
      str.append(QString("<td align=\"center\"><b>RE%1</b></td>").arg((i==0 ? 'A': 'B')));
    }
    str.append("</tr>");
    for (i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
      PhaseData *pd1=&g_model1->phaseData[i];
      PhaseData *pd2=&g_model2->phaseData[i];
      str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
      color=getColor1(pd1->name,pd2->name);
      str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->name).arg(color));
      if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
        for (k=0; k<gvarnum; k++) {
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
      for (k=0; k<GetEepromInterface()->getCapability(RotaryEncoders); k++) {
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
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(getStickStr(i)));
  }
  str.append("</tr>");
  for (i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
    PhaseData *pd1=&g_model1->phaseData[i];
    PhaseData *pd2=&g_model2->phaseData[i];
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
  
  if ((gvars==1 && GetEepromInterface()->getCapability(GvarsFlightPhases)) || GetEepromInterface()->getCapability(RotaryEncoders)) {
    str.append("<br><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    str.append("<tr><td style=\"border-style:none;\">&nbsp;</td>");
    if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(gvarnum)+tr("Gvars")+"</td>");
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(GetEepromInterface()->getCapability(RotaryEncoders))+tr("Rot. Enc.")+"</td>");
    }
    str.append("</tr><tr><td align=center ><b>"+tr("Flight mode name")+"</b></td>");
    if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
      for (i=0; i<gvarnum; i++) {
        str.append(QString("<td width=\"40\" align=\"center\"><b>GV%1</b><br>%2</td>").arg(i+1).arg(g_model2->gvars_names[i]));
      }
    }
    for (i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
      str.append(QString("<td align=\"center\"><b>RE%1</b></td>").arg((i==0 ? 'A': 'B')));
    }
    str.append("</tr>");
    for (i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
      PhaseData *pd1=&g_model1->phaseData[i];
      PhaseData *pd2=&g_model2->phaseData[i];
      str.append("<tr><td><b>"+tr("FM")+QString("%1</b> ").arg(i));
      color=getColor1(pd1->name,pd2->name);
      str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->name).arg(color));
      if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
        for (k=0; k<gvarnum; k++) {
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
      for (k=0; k<GetEepromInterface()->getCapability(RotaryEncoders); k++) {
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

void compareDialog::printLimits()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Limits")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  if (GetEepromInterface()->getCapability(HasChNames)) {
    str.append("<tr><td>"+tr("Name")+"</td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  } else {
    str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");    
  }
  for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
    str.append("<tr>");
    if (GetEepromInterface()->getCapability(HasChNames)) {
      QString name1=g_model1->limitData[i].name;
      QString name2=g_model2->limitData[i].name;
      color=getColor1(name1,name2);
      if (name1.trimmed().isEmpty()) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),color,true));
      } else {
        str.append(doTC(name1,color,true));
      }
    } else {
      str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    }
    color=getColor1(g_model1->limitData[i].offset,g_model2->limitData[i].offset);
    str.append(doTR(QString::number((qreal)g_model1->limitData[i].offset/10, 'f', 1),color));
    color=getColor1(g_model1->limitData[i].min,g_model2->limitData[i].min);
    str.append(doTR(QString::number(g_model1->limitData[i].min),color));
    color=getColor1(g_model1->limitData[i].max,g_model2->limitData[i].max);
    str.append(doTR(QString::number(g_model1->limitData[i].max),color));
    color=getColor1(g_model1->limitData[i].revert,g_model2->limitData[i].revert);
    str.append(doTR(QString(g_model1->limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td>");
  str.append("<td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
    str.append("<tr>");
    if (GetEepromInterface()->getCapability(HasChNames)) {
      QString name1=g_model1->limitData[i].name;
      QString name2=g_model2->limitData[i].name;
      color=getColor2(name1,name2);
      if (name2.trimmed().isEmpty()) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),color,true));
      } else {
        str.append(doTC(name2,color,true));
      }
    } else {
      str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    }
    color=getColor2(g_model1->limitData[i].offset,g_model2->limitData[i].offset);
    str.append(doTR(QString::number((qreal)g_model2->limitData[i].offset/10, 'f', 1),color));
    color=getColor2(g_model1->limitData[i].min,g_model2->limitData[i].min);
    str.append(doTR(QString::number(g_model2->limitData[i].min),color));
    color=getColor2(g_model1->limitData[i].max,g_model2->limitData[i].max);
    str.append(doTR(QString::number(g_model2->limitData[i].max),color));
    color=getColor2(g_model1->limitData[i].revert,g_model2->limitData[i].revert);
    str.append(doTR(QString(g_model2->limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printGvars()
{
  QString color;
  int gvars=0;
  int gvarnum=0;
  if ((GetCurrentFirmwareVariant() & GVARS_VARIANT ) || (!GetEepromInterface()->getCapability(HasVariants) && GetEepromInterface()->getCapability(Gvars))) {
    gvars=1;
    gvarnum=GetEepromInterface()->getCapability(GvarsNum);
  }

  if (!GetEepromInterface()->getCapability(GvarsFlightPhases) && (gvars==1 && GetEepromInterface()->getCapability(Gvars))) {
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td colspan=2><h2>"+tr("Global Variables")+"</h2></td></tr>");
    str.append("<tr><td width=50%>");
    str.append("<table border=1 cellspacing=0 cellpadding=3 width=100>");
    PhaseData *pd1=&g_model1->phaseData[0];
    PhaseData *pd2=&g_model2->phaseData[0];
    int width=100/gvarnum;
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {        
      str.append(QString("<td width=\"%1%\" align=\"center\"><b>").arg(width)+tr("GV")+QString("%1</b></td>").arg(i+1));
    }
    str.append("</tr>");
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {
      color=getColor1(pd1->gvars[i],pd2->gvars[i]);
      str.append(QString("<td width=\"%1%\" align=\"center\"><font color=%2>").arg(width).arg(color)+QString("%1</font></td>").arg(pd1->gvars[i]));
    }
    str.append("</tr>");
    str.append("</table></td><td width=50%>");
    str.append("<table border=1 cellspacing=0 cellpadding=3 width=100>");
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {        
      str.append(QString("<td width=\"%1%\" align=\"center\"><b>").arg(width)+tr("GV")+QString("%1</b></td>").arg(i+1));
    }
    str.append("</tr>");
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {
      color=getColor2(pd1->gvars[i],pd2->gvars[i]);
      str.append(QString("<td width=\"%1%\" align=\"center\"><font color=%2>").arg(width).arg(color)+QString("%1</font></td>").arg(pd2->gvars[i]));
    }
    str.append("</tr>");
    str.append("</table></td>");
    str.append("</tr></table>");
    te->append(str);
  }
}

void compareDialog::printExpos()
{
  QString color;
  bool expoused[C9X_MAX_EXPOS]={false};
  bool expoused2[C9X_MAX_EXPOS]={false};

  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Expo/Dr Settings"));
  str.append("</h2></td></tr><tr><td><table border=1 cellspacing=0 cellpadding=3>");
  for(uint8_t i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
    if (ChannelHasExpo(g_model1->expoData, i) || ChannelHasExpo(g_model2->expoData, i)) {
      str.append("<tr>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<C9X_MAX_EXPOS; j++) {    
        if (g_model1->expoData[j].chn==i){
          int expo=ModelHasExpo(g_model2->expoData, g_model1->expoData[j],expoused);
          if (expo>-1) {
            if (expoused[expo]==false) {
              color="grey";
              expoused[expo]=true;
            } else {
              color="green";
            }    
          } else {
            color="green";
          }
          ExpoData *ed=&g_model1->expoData[j];
          if(ed->mode==0)
            continue;
          str.append("<tr><td><font face='Courier New' color=\""+color+"\">");
          switch(ed->mode) {
            case (1): 
              str += "&lt;-&nbsp;";
              break;
            case (2): 
              str += "-&gt;&nbsp;";
              break;
            default:
              str += "&nbsp;&nbsp;&nbsp;";
              break;
          };

          str += tr("Weight") + QString("%1").arg(getGVarString(ed->weight)).rightJustified(6, ' ');
          str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->expo)).rightJustified(7, ' ');
          if (GetEepromInterface()->getCapability(FlightPhases)) {
            if(ed->phases) {
              if (ed->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
                int mask=1;
                int first=0;
                for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                  if (!(ed->phases & mask)) {
                    first++;
                  }
                  mask <<=1;
                }
                if (first>1) {
                  str += " " + tr("Flight modes") + QString("(");
                } else {
                  str += " " + tr("Flight mode") + QString("(");
                }
                mask=1;
                first=1;
                for (int j=0; j<GetEepromInterface()->getCapability(FlightPhases);j++) {
                  if (!(ed->phases & mask)) {
                    PhaseData *pd = &g_model1->phaseData[j];
                    if (!first) {
                      str += QString(", ")+ QString("%1").arg(getPhaseName(j+1, pd->name));
                    } else {
                      str += QString("%1").arg(getPhaseName(j+1,pd->name));
                      first=0;
                    }
                  }
                  mask <<=1;
                }
                str += QString(")");
              } else {
                str += tr("DISABLED")+QString(" !!!");
              }
            }
          } 
          if (ed->swtch.type)
            str += " " + tr("Switch") + QString("(%1)").arg(ed->swtch.toString());
          if (ed->curveMode)
            if (ed->curveParam) 
                str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(ed->curveParam).replace("<", "&lt;").replace(">", "&gt;"));
          str += "</font></td></tr>";
        }
      }
      str.append("</table></td>");
      str.append("<td width=\"10%\" align=\"center\" valign=\"middle\"><b>"+getStickStr(i)+"</b></td>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<C9X_MAX_EXPOS; j++) {
        if (g_model2->expoData[j].chn==i){
          int expo=ModelHasExpo(g_model1->expoData, g_model2->expoData[j], expoused2);
          if (expo>-1) {
            if (expoused2[expo]==false) {
              color="grey";
              expoused2[expo]=true;
            } else {
              color="red";
            }    
          } else {
            color="red";
          }
          ExpoData *ed=&g_model2->expoData[j];
          if(ed->mode==0)
            continue;
          str.append("<tr><td><font face='Courier New' color=\""+color+"\">");
          switch(ed->mode) {
            case (1): 
              str += "&lt;-&nbsp;";
              break;
            case (2): 
              str += "-&gt;&nbsp;";
              break;
            default:
              str += "&nbsp;&nbsp;&nbsp;";
              break;
          }

          str += tr("Weight") + QString("%1").arg(getGVarString(ed->weight)).rightJustified(6, ' ');
          str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->expo)).rightJustified(7, ' ');
          if (GetEepromInterface()->getCapability(FlightPhases)) {
            if(ed->phases) {
              if (ed->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
                int mask=1;
                int first=0;
                for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                  if (!(ed->phases & mask)) {
                    first++;
                  }
                  mask <<=1;
                }
                if (first>1) {
                  str += " " + tr("Flight modes") + QString("(");
                } else {
                  str += " " + tr("Flight mode") + QString("(");
                }
                mask=1;
                first=1;
                for (int j=0; j<GetEepromInterface()->getCapability(FlightPhases);j++) {
                  if (!(ed->phases & mask)) {
                    PhaseData *pd = &g_model2->phaseData[j];
                    if (!first) {
                      str += QString(", ")+ QString("%1").arg(getPhaseName(j+1, pd->name));
                    } else {
                      str += QString("%1").arg(getPhaseName(j+1,pd->name));
                      first=0;
                    }
                  }
                  mask <<=1;
                }
                str += QString(")");
              } else {
                str += tr("DISABLED")+QString(" !!!");
              }
            }
          } 
          if (ed->swtch.type)
            str += " " + tr("Switch") + QString("(%1)").arg(ed->swtch.toString());
          if (ed->curveMode)
            if (ed->curveParam) 
                str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(ed->curveParam).replace("<", "&lt;").replace(">", "&gt;"));
          str += "</font></td></tr>";
        }
      }
      str.append("</table></td></tr>");
    }
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printMixers()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Mixers"));
  str.append("</h2></td></tr><tr><td><table border=1 cellspacing=0 cellpadding=3>");
  float scale=GetEepromInterface()->getCapability(SlowScale);
  bool mixused[64]={false};
  bool mixused2[64]={false};
  for(uint8_t i=1; i<=GetEepromInterface()->getCapability(Outputs); i++) {
    if (ChannelHasMix(g_model1->mixData, i) || ChannelHasMix(g_model2->mixData, i)) {
      str.append("<tr>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<GetEepromInterface()->getCapability(Mixes); j++) {
        if (g_model1->mixData[j].destCh==i) {
          int mix=ModelHasMix(g_model2->mixData, g_model1->mixData[j], mixused);
          if (mix>-1) {
            if (mixused[mix]==false) {
              color="grey";
              mixused[mix]=true;
            } else {
              color="green";
            }    
          } else {
            color="green";
          }
          MixData *md = &g_model1->mixData[j];
          str.append("<tr><td><font  face='Courier New' color=\""+color+"\">");
          switch(md->mltpx) {
            case (1):
              str += "&nbsp;*";
              break;
            case (2):
              str += "&nbsp;R";
              break;
            default:
              str += "&nbsp;&nbsp;";
              break;
          };
          str += QString(" %1").arg(getGVarString(md->weight)).rightJustified(6, ' ');
          str += md->srcRaw.toString();
          if (md->swtch.type) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
          if (md->carryTrim) str += " " + tr("noTrim");
          if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){ 
                  if (md->sOffset)  str += " "+ tr("FMTrim") + QString(" (%1%)").arg(md->sOffset);
          } else {
                  if (md->sOffset)  str += " "+ tr("Offset") + QString(" (%1%)").arg(getGVarString(md->sOffset));           
          }
          if (md->differential)  str += " "+ tr("Diff") + QString(" (%1%)").arg(getGVarString(md->differential));
          if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve).replace("<", "&lt;").replace(">", "&gt;"));
          if (md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp/scale).arg(md->delayDown/scale);
          if (md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp/scale).arg(md->speedDown/scale);
          if (md->mixWarn)  str += " "+tr("Warn")+QString("(%1)").arg(md->mixWarn);
          if (GetEepromInterface()->getCapability(FlightPhases)) {
            if(md->phases) {
              if (md->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
                int mask=1;
                int first=0;
                for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                  if (!(md->phases & mask)) {
                    first++;
                  }
                  mask <<=1;
                }
                if (first>1) {
                  str += " " + tr("Flight modes") + QString("(");
                } else {
                  str += " " + tr("Flight mode") + QString("(");
                }
                mask=1;
                first=1;
                for (int j=0; j<GetEepromInterface()->getCapability(FlightPhases);j++) {
                  if (!(md->phases & mask)) {
                    PhaseData *pd = &g_model1->phaseData[j];
                    if (!first) {
                      str += QString(", ")+ QString("%1").arg(getPhaseName(j+1, pd->name));
                    } else {
                      str += QString("%1").arg(getPhaseName(j+1,pd->name));
                      first=0;
                    }
                  }
                  mask <<=1;
                }
                str += QString(")");
              } else {
                str += tr("DISABLED")+QString(" !!!");
              }
            }
          }
          str.append("</font></td></tr>");
        }
      }
      str.append("</table></td>");
      str.append("<td width=\"10%\" align=\"center\" valign=\"middle\"><b>"+tr("CH")+QString("%1</b></td>").arg(i,2,10,QChar('0')));
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<GetEepromInterface()->getCapability(Mixes); j++) {
        if (g_model2->mixData[j].destCh==i) {
          int mix=ModelHasMix(g_model1->mixData, g_model2->mixData[j],mixused2);
          if (mix>-1) {
            if (mixused2[mix]==false) {
              color="grey";
              mixused2[mix]=true;
            } else {
              color="red";
            }    
          } else {
            color="red";
          }
          MixData *md = &g_model2->mixData[j];
          str.append("<tr><td><font  face='Courier New' color=\""+color+"\">");
          switch(md->mltpx) {
            case (1):
              str += "&nbsp;*";
              break;
            case (2):
              str += "&nbsp;R";
              break;
            default:
              str += "&nbsp;&nbsp;";
              break;
          };
          str += QString(" %1").arg(getGVarString(md->weight)).rightJustified(6, ' ');
          str += md->srcRaw.toString();
          if (md->swtch.type) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
          if (md->carryTrim) str += " " + tr("noTrim");
          if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){ 
                  if (md->sOffset)  str += " "+ tr("FMTrim") + QString(" (%1%)").arg(getGVarString(md->sOffset));
          } else {
                  if (md->sOffset)  str += " "+ tr("Offset") + QString(" (%1%)").arg(getGVarString(md->sOffset));
          }
          if (md->differential)  str += " "+ tr("Diff") + QString(" (%1%)").arg(getGVarString(md->differential));
          if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve).replace("<", "&lt;").replace(">", "&gt;"));
          if (md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp/scale).arg(md->delayDown/scale);
          if (md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp/scale).arg(md->speedDown/scale);
          if (md->mixWarn)  str += " "+tr("Warn")+QString("(%1)").arg(md->mixWarn);
          if (GetEepromInterface()->getCapability(FlightPhases)) {
            if(md->phases) {
              if (md->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
                int mask=1;
                int first=0;
                for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                  if (!(md->phases & mask)) {
                    first++;
                  }
                  mask <<=1;
                }
                if (first>1) {
                  str += " " + tr("Flight modes") + QString("(");
                } else {
                  str += " " + tr("Flight mode") + QString("(");
                }
                mask=1;
                first=1;
                for (int j=0; j<GetEepromInterface()->getCapability(FlightPhases);j++) {
                  if (!(md->phases & mask)) {
                    PhaseData *pd = &g_model2->phaseData[j];
                    if (!first) {
                      str += QString(", ")+ QString("%1").arg(getPhaseName(j+1, pd->name));
                    } else {
                      str += QString("%1").arg(getPhaseName(j+1,pd->name));
                      first=0;
                    }
                  }
                  mask <<=1;
                }
                str += QString(")");
              } else {
                str += tr("DISABLED")+QString(" !!!");
              }
            }
          }
          str.append("</font></td></tr>");
        }
      }
      str.append("</table></td>");
      str.append("</tr>");
    }
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printCurves()
{
#if 0
  int i,r,g,b,c;
  char buffer [16];
  QString color;
  QColor * qplot_color[8];
  qplot_color[0]=new QColor(0,0,127);
  qplot_color[1]=new QColor(0,127,0);
  qplot_color[2]=new QColor(127,0,0);
  qplot_color[3]=new QColor(127,127,0);
  qplot_color[4]=new QColor(0,0,255);
  qplot_color[5]=new QColor(0,255,0);
  qplot_color[6]=new QColor(255,0,0);
  qplot_color[7]=new QColor(255,255,0);
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Curves"));
  str.append("</h2></td></tr><tr><td>");
  str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=11><b>"+tr("5 Points Curves")+"</b></td></tr><tr>");
  for(i=0; i<5; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("<td></td>");
  for(i=0; i<5; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("</tr>");

  for(i=0; i<MAX_CURVE5; i++) {
    qplot_color[i]->getRgb(&r,&g,&b);
    c=r;
    c*=256;
    c+=g;
    c*=256;
    c+=b;
    sprintf(buffer,"%06x",c);
    str.append("<tr>");
    for(int j=0; j<5; j++) { 
      color=getColor1(g_model2->curves5[i][j],g_model2->curves5[i][j]);
      str.append(doTR(QString::number(g_model1->curves5[i][j]),color));
    }
    str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
    for(int j=0; j<5; j++) { 
      color=getColor1(g_model2->curves5[i][j],g_model2->curves5[i][j]);
      str.append(doTR(QString::number(g_model2->curves5[i][j]),color));
    }
    str.append("</tr>");
  }
  str.append("</table></td></tr><tr><td>");

  str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=19><b>"+tr("9 Points Curves")+"</b></td></tr><tr>");
  for(i=0; i<9; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("<td></td>");
  for(i=0; i<9; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("</tr>");
  for(i=0; i<MAX_CURVE9; i++) {
    qplot_color[i]->getRgb(&r,&g,&b);
    c=r;
    c*=256;
    c+=g;
    c*=256;
    c+=b;
    sprintf(buffer,"%06x",c);
    str.append("<tr>");
    for(int j=0; j<9; j++) { 
      color=getColor1(g_model2->curves9[i][j],g_model2->curves9[i][j]);
      str.append(doTR(QString::number(g_model1->curves5[i][j]),color));
    }
    str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1+MAX_CURVE5));
    for(int j=0; j<9; j++) { 
      color=getColor1(g_model2->curves9[i][j],g_model2->curves9[i][j]);
      str.append(doTR(QString::number(g_model2->curves9[i][j]),color));
    }
    str.append("</tr>");
  }
  str.append("</table></td></tr></table>");
  te->append(str);
#endif
}

void compareDialog::printSwitches()
{
    int sc=0;
    QString color;
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Custom Switches")+"</h2></td></tr>");
    str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    for (int i=0; i<GetEepromInterface()->getCapability(CustomSwitches); i++) {
      QString sw1 = getCustomSwitchStr(&g_model1->customSw[i], *g_model1);
      QString sw2 = getCustomSwitchStr(&g_model2->customSw[i], *g_model2);
      if (!(sw1.isEmpty() && sw2.isEmpty())) {
        str.append("<tr>");
        color=getColor1(sw1,sw2);
        str.append(QString("<td  width=\"45%\"><font color=%1>").arg(color)+sw1+"</font></td>");
        if (i<9) {
          str.append("<td align=\"center\" width=\"10%\"><b>"+tr("CS")+QString("%1</b></td>").arg(i+1));
        } else {
          str.append("<td align=\"center\" width=\"10%\"><b>"+tr("CS")+('A'+(i-9))+"</b></td>");
        }
        color=getColor2(sw1,sw2);
        str.append(QString("<td  width=\"45%\"><font color=%1>").arg(color)+sw2+"</font></td>");
        str.append("</tr>");
        sc++;
      }
    }
    str.append("</table></td></tr></table>");
    if (sc>0)
        te->append(str);
}

void compareDialog::printFSwitches()
{
  QString color1;
  QString color2;
  int sc=0;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-before:always;\" width=\"100%\">";
  str.append("<tr><td><h2>"+tr("Custom Functions")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"12%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("<td width=\"12%\" align=\"center\"><b>"+tr("Param")+"</b></td>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Repeat")+"</b></td>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Enable")+"</b></td>");
  str.append("<td width=\"10%\">&nbsp;</td>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"12%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("<td width=\"12%\" align=\"center\"><b>"+tr("Param")+"</b></td>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Repeat")+"</b></td>");
  str.append("<td width=\"7%\" align=\"center\"><b>"+tr("Enable")+"</b></td>");
  str.append("</tr>");
  for(int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++)
  {
    if (g_model1->funcSw[i].swtch.type || g_model2->funcSw[i].swtch.type) {
      if ((g_model1->funcSw[i].swtch != g_model2->funcSw[i].swtch) || (g_model1->funcSw[i].func!=g_model2->funcSw[i].func) || (g_model1->funcSw[i].adjustMode!=g_model2->funcSw[i].adjustMode) || (g_model1->funcSw[i].param!=g_model2->funcSw[i].param)) {
        color1="green";
        color2="red";
      } else {
        color1="grey";
        color2="grey";
      }
      str.append("<tr>");
      if (g_model1->funcSw[i].swtch.type) {
        str.append(doTC(g_model1->funcSw[i].swtch.toString(),color1));
        str.append(doTC(getFuncName(g_model1->funcSw[i].func),color1));
        str.append(doTC(FuncParam(g_model1->funcSw[i].func,g_model1->funcSw[i].param,g_model1->funcSw[i].paramarm, g_model1->funcSw[i].adjustMode),color1));        
        int index=g_model1->funcSw[i].func;
        if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncPlayValue || index==FuncPlayPrompt || index==FuncPlayBoth || index==FuncBackgroundMusic) {
          str.append(doTC(QString("%1").arg(g_model1->funcSw[i].repeatParam),color1));
        } else {
          str.append(doTC( "---",color1));
        }
        if ((index<=FuncInstantTrim) || (index>FuncBackgroundMusicPause)) {
          str.append(doTC((g_model1->funcSw[i].enabled ? "ON" : "OFF"),color1));
        } else {
          str.append(doTC( "---",color1));
        }
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append(doTC(tr("CF")+QString("%1").arg(i+1),"",true));
      if (g_model2->funcSw[i].swtch.type) {
        str.append(doTC(g_model2->funcSw[i].swtch.toString(),color2));
        str.append(doTC(getFuncName(g_model2->funcSw[i].func),color2));
        str.append(doTC(FuncParam(g_model2->funcSw[i].func,g_model2->funcSw[i].param,g_model2->funcSw[i].paramarm, g_model2->funcSw[i].adjustMode),color2));        
        int index=g_model2->funcSw[i].func;
        if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncPlayValue || index==FuncPlayPrompt || index==FuncPlayBoth || index==FuncBackgroundMusic) {
          str.append(doTC(QString("%1").arg(g_model2->funcSw[i].repeatParam),color2));
        } else {
          str.append(doTC( "---",color2));
        }
        if ((index<=FuncInstantTrim) || (index>FuncBackgroundMusicPause)) {
          str.append(doTC((g_model2->funcSw[i].enabled ? "ON" : "OFF"),color2));
        } else {
          str.append(doTC( "---",color2));
        }
      }
      else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append("</tr>");
      sc++;
    }
}
  str.append("</table></td></tr></table>");
  str.append("<br>");
  if (sc!=0)
      te->append(str);
}

void compareDialog::printSafetySwitches()
{
  QString color1;
  QString color2;
  int sc=0;
  QString str = "<table border=1 cellspacing=0 cellpadding=3  style=\"page-break-before:always;\" width=\"100%\">";
  str.append("<tr><td><h2>"+tr("Safety Switches")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("<td width=\"10%\">&nbsp;</td>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("</tr>");
  for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++)
  {
    if ((g_model1->safetySw[i].swtch.type!=SWITCH_TYPE_NONE)||(g_model2->safetySw[i].swtch.type!=SWITCH_TYPE_NONE)) {
      if ((g_model1->safetySw[i].swtch!=g_model2->safetySw[i].swtch)||(g_model1->safetySw[i].val!=g_model2->safetySw[i].val)) {
        color1="green";
        color2="red";
      } else {
        color1="grey";
        color2="grey";
      }
      str.append("<tr>");
      if (g_model1->safetySw[i].swtch.type) {
        str.append(doTC(g_model1->safetySw[i].swtch.toString(),color1));
        str.append(doTC(QString::number(g_model1->safetySw[i].val),color1));
      }
      else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append(doTC(tr("CH")+QString("%1").arg(i+1),"",true));
      if (g_model2->safetySw[i].swtch.type) {
        str.append(doTC(g_model2->safetySw[i].swtch.toString(),color2));
        str.append(doTC(QString::number(g_model2->safetySw[i].val),color2));
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append("</tr>");
      sc++;
    }
}
  str.append("</table></td></tr></table>");
  str.append("<br>");
  if (sc!=0)
      te->append(str);
}

void compareDialog::printFrSky()
{
  QString color;
  float value1,value2;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Telemetry Settings")+"</h2></td></tr>");
  str.append("<tr><td width=\"50%\">");
  str.append("<table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
  FrSkyData *fd1=&g_model1->frsky;
  FrSkyData *fd2=&g_model2->frsky;
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
  if (GetEepromInterface()->getCapability(TelemetryBars) || GetEepromInterface()->getCapability(TelemetryCSFields)) {
    int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;
    for (int j=0; j<GetEepromInterface()->getCapability(TelemetryCSFields)/(4*cols); j++) {
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
          value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMin,fd1);
          value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMin,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor1(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value1)+"</td>");
          value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMax,fd1);
          value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMax,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
           color=getColor1(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value1)+"</td></tr>");
        }
        str.append("</table>");
      }
    }
  }
  
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

  if (GetEepromInterface()->getCapability(TelemetryBars) || GetEepromInterface()->getCapability(TelemetryCSFields)) {
    int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;
    for (int j=0; j<GetEepromInterface()->getCapability(TelemetryCSFields)/(4*cols); j++) {
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
          value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMin,fd1);
          value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMin,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor2(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value2)+"</font></td>");
          value1=getBarValue(fd1->screens[0].body.bars[i].source,fd1->screens[0].body.bars[i].barMax,fd1);
          value2=getBarValue(fd2->screens[0].body.bars[i].source,fd2->screens[0].body.bars[i].barMax,fd2);
          if (fd1->screens[0].type==fd2->screens[0].type)
            color=getColor2(value1,value2);
          str.append("<td  align=\"Right\"><font color="+color+">"+QString::number(value2)+"</font></td></tr>");
        }
        str.append("</table>");
      }
    }
  }
  str.append("</td></tr></table>");
  te->append(str);
}

void compareDialog::on_printButton_clicked()
{
    QPrinter printer;
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    te->print(&printer);
}

void compareDialog::on_printFileButton_clicked()
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
