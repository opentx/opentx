#include "printdialog.h"
#include "ui_printdialog.h"
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
#define ISIZEW 400 // curve image size

printDialog::printDialog(QWidget *parent, GeneralSettings *gg, ModelData *gm, QString filename) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::printDialog)
{
    ui->setupUi(this);
    g_model = gm;
    g_eeGeneral = gg;
    printfilename=filename;
    eepromInterface = GetEepromInterface();
    te = ui->textEdit;

    setWindowTitle(tr("Setup for: ") + g_model->name);
    ui->textEdit->clear();
    QString modelname=g_model->name;
    if (modelname.isEmpty()) {
      curvefile5=QString("%1/curve5.png").arg(qd->tempPath());
      curvefile9=QString("%1/curve9.png").arg(qd->tempPath());      
    } else {
      curvefile5=QString("%1/%2-curve5.png").arg(qd->tempPath()).arg(modelname);
      curvefile9=QString("%1/%2-curve9.png").arg(qd->tempPath()).arg(modelname);
    }
    printSetup();
    int gvars=0;
    if (GetEepromInterface()->getCapability(HasVariants)) {
      if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
        gvars=1;
      }
    } else {
      gvars=1;
    }
    
    if (gvars) {
      te->append(printPhases()+"<br>");
    }
    printExpo();
    printMixes();
    printLimits();
    printCurves();
    printGvars();
    printSwitches();
    printSafetySwitches();
    printFSwitches();
    printFrSky();
    
    te->scrollToAnchor("1");
    if (!printfilename.isEmpty()) {
      printToFile();
      QTimer::singleShot(0, this, SLOT(autoClose()));
    }
}

void printDialog::closeEvent(QCloseEvent *event) 
{
  if (printfilename.isEmpty()) {
    QByteArray ba = curvefile5.toLatin1();
    char *name = ba.data(); 
    unlink(name);
    ba = curvefile9.toLatin1();
    name = ba.data(); 
    unlink(name);
  }
}

printDialog::~printDialog()
{
    delete ui;
}

QString doTC(const QString s, const QString color="", bool bold=false)
{
    QString str = s;
    if(bold) str = "<b>" + str + "</b>";
    if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
    return "<td align=center>" + str + "</td>";
}

QString doTR(const QString s, const QString color="", bool bold=false)
{
    QString str = s;
    if(bold) str = "<b>" + str + "</b>";
    if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
    return "<td align=right>" + str + "</td>";
}

QString doTL(const QString s, const QString color="", bool bold=false)
{
    QString str = s;
    if(bold) str = "<b>" + str + "</b>";
    if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
    return "<td align=left>" + str + "</td>";
}

QString printDialog::fv(const QString name, const QString value)
{
    return "<b>" + name + ": </b><font color=green>" + value + "</font><br>";
}

void printDialog::printSetup()
{
    int gvars=0;
    if (GetEepromInterface()->getCapability(HasVariants)) {
      if ((GetCurrentFirmwareVariant() & GVARS_VARIANT)) {
        gvars=1;
      }
    } else {
      gvars=1;
    }
    QString str = "<a name=1></a><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append(QString("<tr><td colspan=%1 ><table border=0 width=\"100%\"><tr><td><h1>").arg((GetEepromInterface()->getCapability(FlightPhases) && gvars==0) ? 2 : 1));
    str.append(g_model->name);
    str.append("&nbsp;(");
    str.append(eepromInterface->getName());
    str.append(")</h1></td><td align=right valign=top NOWRAP><font size=-1>"+tr("printed on: %1").arg(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate))+"</font></td></tr></table></td></tr><tr><td><table border=0 cellspacing=0 cellpadding=3>");
    str.append("<tr><td><h2>"+tr("General Model Settings")+"</h2></td></tr>");
    str.append("<tr><td>");
    str.append(fv(tr("Name"), g_model->name));
    str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=green>%1</font><br>").arg(eepromInterface->getSize(*g_model)));
    str.append(fv(tr("Timer1"), getTimerStr(g_model->timers[0])));  //value, mode, count up/down
    str.append(fv(tr("Timer2"), getTimerStr(g_model->timers[1])));  //value, mode, count up/down
    str.append(fv(tr("Protocol"), getProtocol(g_model))); //proto, numch, delay,
    str.append(fv(tr("Pulse Polarity"), g_model->moduleData[0].ppmPulsePol ? "NEG" : "POS"));
    str.append(fv(tr("Throttle Trim"), g_model->thrTrim ? tr("Enabled") : tr("Disabled")));
    str.append(fv(tr("Throttle Expo"), g_model->thrExpo ? tr("Enabled") : tr("Disabled")));
    // TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
    str.append(fv(tr("Trim Increment"), getTrimInc(g_model)));
    str.append(fv(tr("Center Beep"), getCenterBeep(g_model))); // specify which channels beep
    str.append("</td></tr></table></td>");
    if (!gvars) {
      str.append("<td width=\"380\">");
      str.append(printPhases());
      str.append("</td>");
    }
    str.append("</tr></table><br>");
    te->append(str);
}

QString printDialog::printPhases()
{      
    int gvars=0;
    int gvarnum=0;
    if ((GetCurrentFirmwareVariant() & GVARS_VARIANT ) || (!GetEepromInterface()->getCapability(HasVariants) && GetEepromInterface()->getCapability(Gvars))) {
      if (GetEepromInterface()->getCapability(GvarsFlightPhases)) {
        gvars=1;
        gvarnum=GetEepromInterface()->getCapability(GvarsNum);
      }
    }

    QString str="";
    str.append(QString("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=%1><h2>").arg(gvars==0 ? 8+GetEepromInterface()->getCapability(RotaryEncoders) : 8+gvarnum+GetEepromInterface()->getCapability(RotaryEncoders)));
    str.append(tr("Flight modes Settings"));
    str.append("</h2></td></tr><tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>");
    str.append(tr("Fades")+"</b></td>");
    str.append("<td colspan=4 align=center><b>"+tr("Trims")+"</b></td>");
    if (gvars) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(gvarnum)+tr("Gvars")+"</b></td>");
    }
    if (GetEepromInterface()->getCapability(RotaryEncoders)) {
      str.append(QString("<td colspan=%1 align=center><b>").arg(GetEepromInterface()->getCapability(RotaryEncoders))+tr("Rot.Enc.")+"</b></td>");
    }
    str.append("<td rowspan=2 align=\"center\" valign=\"bottom\"><b>"+tr("Switch")+"</b></td></tr><tr><td align=center width=\"90\"><b>"+tr("Flight mode name"));
    str.append("</b></td><td align=center width=\"30\"><b>"+tr("IN")+"</b></td><td align=center width=\"30\"><b>"+tr("OUT")+"</b></td>");
    for (int i=0; i<4; i++) {
      str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(getStickStr(i)));
    }
    if (gvars==1) {
      for (int i=0; i<gvarnum; i++) {
        str.append(QString("<td width=\"40\" align=\"center\"><b>GV%1</b><br>%2</td>").arg(i+1).arg(g_model->gvars_names[i]));
      }      
    }
    for (int i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
      str.append(QString("<td align=\"center\"><b>RE%1</b></td>").arg((i==0 ? 'A': 'B')));
    }
    str.append("</tr>");
    for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
      PhaseData *pd=&g_model->phaseData[i];
      str.append("<tr><td><b>"+tr("FM")+QString("%1</b> <font size=+1 face='Courier New' color=green>%2</font></td><td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=green>%3</font></td><td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=green>%4</font></td>").arg(i).arg(pd->name).arg(pd->fadeIn).arg(pd->fadeOut));
      for (int k=0; k<4; k++) {
        if (pd->trimRef[k]==-1) {
          str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>%1</font></td>").arg(pd->trim[k]));
        } else {
          str.append("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>"+tr("FM")+QString("%1</font></td>").arg(pd->trimRef[k]));
        }
      }
      if (gvars==1) {
        for (int k=0; k<gvarnum; k++) {
          if (pd->gvars[k]<=1024) {
            str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>%1").arg(pd->gvars[k])+"</font></td>");
          }
          else {
            int num = pd->gvars[k] - 1025;
            if (num>=i) num++;
            str.append("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>"+tr("FM")+QString("%1</font></td>").arg(num));
          }
        }
      }
      for (int k=0; k<GetEepromInterface()->getCapability(RotaryEncoders); k++) {
        if (pd->rotaryEncoders[k]<=1024) {
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=green>%1").arg(pd->rotaryEncoders[k])+"</font></td>");
        }
        else {
          int num = pd->rotaryEncoders[k] - 1025;
          if (num>=i) num++;
          str.append(QString("<td align=\"right\"><font size=+1 face='Courier New' color=green>")+tr("FM")+QString("%1</font></td>").arg(num));
        }
      }      
      str.append(QString("<td align=center><font size=+1 face='Courier New' color=green>%1</font></td>").arg(pd->swtch.toString()));
      str.append("</tr>");
    }
    str.append("</table>");
    return(str);
}

void printDialog::printExpo()
{
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td><h2>";
    str.append(tr("Expo/Dr Settings"));
    str.append("</h2></td></tr><tr><td><table border=0 cellspacing=0 cellpadding=3>");
    int ec=0;
    unsigned int lastCHN = 255;
    for(int i=0; i<C9X_MAX_EXPOS; i++) {
      ExpoData *ed=&g_model->expoData[i];
      if(ed->mode==0)
        continue;
      ec++;
      str.append("<tr><td><font size=+1 face='Courier New'>");
      if(lastCHN!=ed->chn) {
        lastCHN=ed->chn;
        str.append("<b>"+getStickStr(ed->chn)+"</b>");
      }
      else
        str.append("<b>&nbsp;</b>");
      str.append("</font></td>");
      str.append("<td><font size=+1 face='Courier New' color=green>");

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

      str += tr("Weight") + QString("%1").arg(getGVarString(ed->weight,true)).rightJustified(6, ' ');
      if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
        if (ed->expo!=0) {
          if (ed->expo<126) {
            str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->expo)).rightJustified(7, ' ');
          } else {
            str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->expo)).rightJustified(7, ' ');
          }
        }
      } else {
        if (ed->curveMode==0 && ed->curveParam!=0) {
          if (ed->curveParam<126) {
            str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->curveParam)).rightJustified(7, ' ');
          } else {
            str += " " + tr("Expo") + QString("%1").arg(getGVarString(ed->curveParam)).rightJustified(7, ' ');
          }
        }
      }   
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
                PhaseData *pd = &g_model->phaseData[j];
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
      if (GetEepromInterface()->getCapability(HasExpoNames)) {
        QString ExpoName;
        ExpoName.append(ed->name);
        if (!ExpoName.isEmpty()) {
          str+=QString(" (%1)").arg(ExpoName);
        }
      }
      str += "</font></td></tr>";
    }
    str += "</table></td></tr></table><br>";
    if (ec>0)
      te->append(str);
}


void printDialog::printMixes()
{
    QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
    str.append(tr("Mixers"));
    str.append("</h2></td></tr><tr><td><table border=0 cellspacing=0 cellpadding=3>");

    unsigned int lastCHN = 255;
    for(int i=0; i<GetEepromInterface()->getCapability(Mixes); i++) {
      MixData *md = &g_model->mixData[i];
      if(!md->destCh || md->destCh>(unsigned int)GetEepromInterface()->getCapability(Outputs) ) break;
      str.append("<tr><td><font size=+1 face='Courier New'><b>");
      if(lastCHN!=md->destCh) {
        lastCHN=md->destCh;
        QString chname=tr("CH")+QString("%1  ").arg(lastCHN,2,10,QChar('0'));
        if (GetEepromInterface()->getCapability(HasChNames)) {
          QString name=g_model->limitData[md->destCh-1].name;
          if (!name.isEmpty()) {
            name.append("      ");
            chname=name.left(6);
          }
        }
        str.append(chname);
      } else {
        str.append("&nbsp;");
      }
      str.append("</b></font></td>");
      str.append("<td><font size=+1 face='Courier New' color=green>");
      switch(md->mltpx) {
        case (1): str += "&nbsp;*"; break;
        case (2): str += "&nbsp;R"; break;
        default:  str += "&nbsp;&nbsp;"; break;
      };
      str += QString(" %1").arg(getGVarString(md->weight, true)).rightJustified(6, ' ');
      str += md->srcRaw.toString();
      if (md->swtch.type) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
      if (md->carryTrim) str += " " + tr("noTrim");
      if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){ 
        if (md->sOffset)  str += " "+ tr("FMTrim") + QString(" %1").arg(getGVarString(md->sOffset));
      } else {
        if (md->sOffset)  str += " "+ tr("Offset") + QString(" %1").arg(getGVarString(md->sOffset));           
      }
      if (md->differential)  str += " "+ tr("Diff") + QString(" %1").arg(getGVarString(md->differential));
      if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve).replace("<", "&lt;").replace(">", "&gt;"));
      float scale=GetEepromInterface()->getCapability(SlowScale);
      if (md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp/scale).arg(md->delayDown/scale);
      if (md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp/scale).arg(md->speedDown/scale);
      if (md->mixWarn)  str += " "+tr("Warn")+QString("(%1)").arg(md->mixWarn);
      if (GetEepromInterface()->getCapability(FlightPhases)) {
        if(md->phases) {
          if (md->phases!=(unsigned int)(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
            int mask=1;
            int first=0;
            for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
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
                PhaseData *pd = &g_model->phaseData[j];
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
      if (GetEepromInterface()->getCapability(HasMixerNames)) {
        QString MixerName;
        MixerName.append(md->name);
        if (!MixerName.isEmpty()) {
          str+=QString(" (%1)").arg(MixerName);
        }
      }
      str.append("</font></td></tr>");
    }
    str.append("</table></td></tr></table><br>");
    te->append(str);
}

void printDialog::printLimits()
{
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    int numcol;
    numcol=(GetEepromInterface()->getCapability(Outputs)+1)>17 ? 17:GetEepromInterface()->getCapability(Outputs)+1;
    str.append(QString("<tr><td colspan=%1><h2>").arg(numcol)+tr("Limits")+"</h2></td></tr>");
    str.append("<tr><td>&nbsp;</td>");
    if (GetEepromInterface()->getCapability(Outputs)<17) {
      for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
      }
      str.append("</tr>");
      if (GetEepromInterface()->getCapability(HasChNames)) {
        str.append("<tr><td><b>"+tr("Name")+"</b></td>");
        for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
          str.append(doTR(g_model->limitData[i].name,"green"));
        }
      }
      str.append("<tr><td><b>"+tr("Offset")+"</b></td>");
      for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number((qreal)g_model->limitData[i].offset/10, 'f', 1),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Min")+"</b></td>");
      for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number(g_model->limitData[i].min),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Max")+"</b></td>");
      for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number(g_model->limitData[i].max),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Invert")+"</b></td>");
      for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString(g_model->limitData[i].revert ? tr("INV") : tr("NOR")),"green"));
      }
    } else {
      for(int i=0; i<16; i++) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
      }
      str.append("</tr>");
      if (GetEepromInterface()->getCapability(HasChNames)) {
        str.append("<tr><td><b>"+tr("Name")+"</b></td>");
        for(int i=0; i<16; i++) {
          str.append(doTR(g_model->limitData[i].name,"green"));
        }
      }
      str.append("<tr><td><b>"+tr("Offset")+"</b></td>");
      for(int i=0; i<16; i++) {
        str.append(doTR(QString::number((qreal)g_model->limitData[i].offset/10, 'f', 1),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Min")+"</b></td>");
      for(int i=0; i<16; i++) {
        str.append(doTR(QString::number(g_model->limitData[i].min),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Max")+"</b></td>");
      for(int i=0; i<16; i++) {
        str.append(doTR(QString::number(g_model->limitData[i].max),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Invert")+"</b></td>");
      for(int i=0; i<16; i++) {
        str.append(doTR(QString(g_model->limitData[i].revert ? tr("INV") : tr("NOR")),"green"));
      }
      str.append("</tr>");
      str.append(QString("<tr><td colspan=%1>&nbsp;").arg(numcol)+"</td></tr>");
      str.append("<tr><td>&nbsp;</td>");
      for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
      }
      str.append("</tr>");
      if (GetEepromInterface()->getCapability(HasChNames)) {
        str.append("<tr><td><b>"+tr("Name")+"</b></td>");
        for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
          str.append(doTR(g_model->limitData[i].name,"green"));
        }
      }
      str.append("<tr><td><b>"+tr("Offset")+"</b></td>");
      for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number((qreal)g_model->limitData[i].offset/10, 'f', 1),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Min")+"</b></td>");
      for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number(g_model->limitData[i].min),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Max")+"</b></td>");
      for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString::number(g_model->limitData[i].max),"green"));
      }
      str.append("</tr>");
      str.append("<tr><td><b>"+tr("Invert")+"</b></td>");
      for(int i=16; i<GetEepromInterface()->getCapability(Outputs); i++) {
        str.append(doTR(QString(g_model->limitData[i].revert ? tr("INV") : tr("NOR")),"green"));
      }
    }
    str.append("</tr>");
    str.append("</table>");
    str.append("<br>");
    te->append(str);
}

void printDialog::printCurves()
{
    int i,r,g,b,c,count;
    char buffer[16];
    QPen pen(Qt::black, 2, Qt::SolidLine);
    QColor * qplot_color[16];
    qplot_color[0]=new QColor(0,0,127);
    qplot_color[1]=new QColor(0,127,0);
    qplot_color[2]=new QColor(127,0,0);
    qplot_color[3]=new QColor(0,127,127);
    qplot_color[4]=new QColor(127,0,127);
    qplot_color[5]=new QColor(127,127,0);
    qplot_color[6]=new QColor(127,127,127);
    qplot_color[7]=new QColor(0,0,255);
    qplot_color[8]=new QColor(0,127,255);
    qplot_color[9]=new QColor(127,0,255);
    qplot_color[10]=new QColor(0,255,0);
    qplot_color[11]=new QColor(0,255,127);
    qplot_color[12]=new QColor(127,255,0);
    qplot_color[13]=new QColor(255,0,0);
    qplot_color[14]=new QColor(255,0,127);
    qplot_color[15]=new QColor(255,127,0);
    
    QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-before:auto;\" width=\"100%\"><tr><td><h2>";
    str.append(tr("Curves"));
    str.append("</h2></td></tr><tr><td>");
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    if (!GetEepromInterface()->getCapability(CustomCurves)) {
      QImage qi(ISIZE+1,ISIZE+1,QImage::Format_RGB32);
      QPainter painter(&qi);
      painter.setBrush(QBrush("#FFFFFF"));
      painter.setPen(QColor(0,0,0));
      painter.drawRect(0,0,ISIZE,ISIZE);
      str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=2><b>"+tr("5 Points Curves")+QString("</b></td></tr><tr><td width=\"200\"><img src=\"%1\" border=0></td><td><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">").arg(curvefile5));
      str.append("<tr>");
      str.append(doTC("&nbsp;"));
      for(i=0; i<5; i++) 
          str.append(doTC(tr("pt %1").arg(i+1), "", true));
      str.append("</tr>");
      for(i=0; i<8; i++) {
        pen.setColor(*qplot_color[i]);
        painter.setPen(pen);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append("<tr>");
        str.append(QString("<td width=\"70\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
        count=0;
        for(int j=0; j<5; j++) {
          if (g_model->curves[i].points[j].y!=0)
            count++;
        }
        for(int j=0; j<5; j++) {    
          str.append(doTR(QString::number(g_model->curves[i].points[j].y),"green"));
          if (j>0 && count!=0) {
            painter.drawLine(ISIZE*(j-1)/4,ISIZE/2-(ISIZE*g_model->curves[i].points[j-1].y)/200,ISIZE*(j)/4,ISIZE/2-(ISIZE*g_model->curves[i].points[j].y)/200);
          }
        }
        str.append("</tr>");
      }
      str.append("</table></td></tr></table></td></tr><tr><td>");
      painter.setPen(QColor(0,0,0));
      painter.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
      painter.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
      for(i=0; i<5; i++) {
        painter.drawLine(ISIZE/2-2,(ISIZE*i)/4,ISIZE/2+2,(ISIZE*i)/4);
        painter.drawLine((ISIZE*i)/4,ISIZE/2-2,(ISIZE*i)/4,ISIZE/2+2);
      }

      qi.save(curvefile5, "png",100); 
      str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=2><b>"+tr("9 Points Curves")+QString("</b></td></tr><tr><td width=\"200\"><img src=\"%1\" border=0></td><td><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">").arg(curvefile9));
      str.append("<tr><td width=\"70\">&nbsp;</td>");
      for(i=0; i<9; i++) str.append(doTC(tr("pt %1").arg(i+1), "", true));
      str.append("</tr>");

      painter.setBrush(QBrush("#FFFFFF"));
      painter.setPen(QColor(0,0,0));
      painter.drawRect(0,0,ISIZE,ISIZE);
      for(i=0; i<9; i++) {
        painter.drawLine(ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2,(ISIZE*i)/8);
        painter.drawLine((ISIZE*i)/8,ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2);
      }
      for(i=0; i<8; i++) {
        pen.setColor(*qplot_color[i]);
        painter.setPen(pen);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append("<tr>");
        str.append(QString("<td width=\"70\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+9));
        count=0;
        for(int j=0; j<9; j++) {
          if (g_model->curves[i].points[j].y!=0)
            count++;
        }
        for(int j=0; j<9; j++) {
          str.append(doTR(QString::number(g_model->curves[i].points[j].y),"green"));
          if (j>0 && count!=0) {
            painter.drawLine(ISIZE*(j-1)/8,ISIZE/2-(ISIZE*g_model->curves[i+8].points[j-1].y)/200,ISIZE*(j)/8,ISIZE/2-(ISIZE*g_model->curves[i+8].points[j].y)/200);
          }
        }
        str.append("</tr>");
      }
      str.append("</table></td></tr></table></td></tr></table>");
      str.append("<br>");
      painter.setPen(QColor(0,0,0));
      painter.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
      painter.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
      for(i=0; i<9; i++) {
        painter.drawLine(ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2,(ISIZE*i)/8);
        painter.drawLine((ISIZE*i)/8,ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2);
      }
      qi.save(curvefile9, "png",100);     
    } else {
      QImage qi(ISIZEW+1,ISIZEW+1,QImage::Format_RGB32);
      QPainter painter(&qi);
      painter.setBrush(QBrush("#FFFFFF"));
      painter.setPen(QColor(0,0,0));
      painter.drawRect(0,0,ISIZEW,ISIZEW);
      str.append("<table border=0 cellspacing=0 cellpadding=3 width=\"100%\">"+QString("<tr><td width=\"400\"><img src=\"%1\" border=0></td><td><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">").arg(curvefile5));
      for(i=0; i<numcurves; i++) {
        pen.setColor(*qplot_color[i]);
        painter.setPen(pen);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append(QString("<tr><td width=\"70\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td></tr>").arg(i+1));
      }
      str.append("</table></td></tr><tr><td colspan=2><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">");
      str.append("<tr>");
      str.append(doTC("&nbsp;"));
      str.append(doTC("&nbsp;"));
      int numpoint=0;
      for(i=0; i<numcurves; i++) {
        if (g_model->curves[i].count>numpoint)
          numpoint=g_model->curves[i].count;
      }
      for(i=0; i<numpoint; i++) 
          str.append(doTC(tr("pt %1").arg(i+1), "", true));
      str.append("</tr>");
      for(i=0; i<numcurves; i++) {
        pen.setColor(*qplot_color[i]);
        painter.setPen(pen);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append("<tr>");
        int curvepoints=g_model->curves[i].count;
        if (!g_model->curves[i].custom) {
          str.append(QString("<td width=\"70\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td><td width=5>Y</td>").arg(i+1));
        } else {
          str.append(QString("<td width=\"70\" rowspan=2 valign=middle><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td><td width=5>Y</td>").arg(i+1));
        }
        count=0;
        for(int j=0; j<curvepoints; j++) {
          if (g_model->curves[i].points[j].y!=0)
            count++;
        }
        for(int j=0; j<curvepoints; j++) {    
          str.append(doTR(QString::number(g_model->curves[i].points[j].y),"green"));
          if (j>0 && count!=0) {
            if (!g_model->curves[i].custom) {
              painter.drawLine(ISIZEW*(j-1)/(curvepoints-1),ISIZEW/2-(ISIZEW*g_model->curves[i].points[j-1].y)/200,ISIZEW*(j)/(curvepoints-1),ISIZEW/2-(ISIZEW*g_model->curves[i].points[j].y)/200);
            } else {
              painter.drawLine(ISIZEW/2+(ISIZEW*g_model->curves[i].points[j-1].x)/200,ISIZEW/2-(ISIZEW*g_model->curves[i].points[j-1].y)/200,ISIZEW/2+(ISIZEW*g_model->curves[i].points[j].x)/200,ISIZEW/2-(ISIZEW*g_model->curves[i].points[j].y)/200);
            }
          }
        }
        for(int j=curvepoints; j<numpoint; j++) {
          str.append(doTR("","green"));
        }     
        str.append("</tr>");
        if (g_model->curves[i].custom) {
          str.append("<tr><td width=5>X</td>");
          for(int j=0; j<curvepoints; j++) {    
            str.append(doTR(QString::number(g_model->curves[i].points[j].x),"green"));
          }
          for(int j=curvepoints; j<numpoint; j++) {
            str.append(doTR("","green"));
          }     
          str.append("</tr>");  
        }
      }
      str.append("</table></td></tr></table></td></tr></table>");
      str.append("<br>");
      painter.setPen(QColor(0,0,0));
      painter.drawLine(0,ISIZEW/2,ISIZEW,ISIZEW/2);
      painter.drawLine(ISIZEW/2,0,ISIZEW/2,ISIZEW);
      for(i=0; i<21; i++) {
        painter.drawLine(ISIZEW/2-5,(ISIZEW*i)/(20),ISIZEW/2+5,(ISIZEW*i)/(20));
        painter.drawLine((ISIZEW*i)/(20),ISIZEW/2-5,(ISIZEW*i)/(20),ISIZEW/2+5);
      }

      qi.save(curvefile5, "png",100); 
      
    }
    te->append(str);
}

void printDialog::printSwitches()
{
    int sc=0;
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Custom Switches")+"</h2></td></tr>");
    str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3>");

    for (int i=0; i<GetEepromInterface()->getCapability(CustomSwitches); i++) {
      if (g_model->customSw[i].func) {
        str.append("<tr>");
        if (i<9) {
          str.append("<td width=\"60\" align=\"center\"><b>"+tr("CS")+QString("%1</b></td>").arg(i+1));
        } else {
          str.append("<td width=\"60\" align=\"center\"><b>"+tr("CS")+('A'+(i-9))+"</b></td>");
        }
        QString tstr = getCustomSwitchStr(&g_model->customSw[i], *g_model);
        str.append(doTC(tstr,"green"));
        str.append("</tr>");
        sc++;
      }
    }
    str.append("</table></td></tr></table>");
    str.append("<br>");
    if (sc!=0)
      te->append(str);
}

void printDialog::printGvars()
{
  int gvars=0;
  int gvarnum=0;
  if ((GetCurrentFirmwareVariant() & GVARS_VARIANT ) || (!GetEepromInterface()->getCapability(HasVariants) && GetEepromInterface()->getCapability(Gvars))) {
    gvars=1;
    gvarnum=GetEepromInterface()->getCapability(GvarsNum);
  }
  if (!GetEepromInterface()->getCapability(GvarsFlightPhases) && (gvars==1 && GetEepromInterface()->getCapability(Gvars))) {
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Global Variables")+"</h2></td></tr>");
    str.append("<tr><td><table border=1 cellspacing=0 cellpadding=3 width=100>");
    PhaseData *pd=&g_model->phaseData[0];
    int width=100/gvarnum;
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {        
        str.append(QString("<td width=\"%1%\" align=\"center\"><b>").arg(width)+tr("GV")+QString("%1</b></td>").arg(i+1));
    }
    str.append("</tr>");
    str.append("<tr>");
    for(int i=0; i<gvarnum; i++) {        
        str.append(QString("<td width=\"%1%\" align=\"center\"><font color=green>").arg(width)+QString("%1</font></td>").arg(pd->gvars[i]));
    }
    str.append("</tr>");
    str.append("</table></td></tr></table>");
    str.append("<br>");
    te->append(str);
  }
}

void printDialog::printSafetySwitches()
{
    int sc=0;
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Safety Switches")+"</h2></td></tr>");
    str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3><tr>");
    str.append("<td width=\"60\">&nbsp;</td>");
    str.append(doTC(tr("Switch"), "", true));
    str.append(doTL(tr("Value"), "", true));
    str.append("</tr>");
    for(int i=0; i<GetEepromInterface()->getCapability(Outputs); i++)
    {
        if (g_model->safetySw[i].swtch.type) {
           str.append("<tr>");
           str.append(doTC(tr("CH")+QString("%1").arg(i+1),"",true));
           str.append(doTC(g_model->safetySw[i].swtch.toString(),"green"));
           str.append(doTC(QString::number(g_model->safetySw[i].val),"green"));
           str.append("</tr>");
           sc++;
        }
    }
    str.append("</table></td></tr></table>");
    str.append("<br>");
    if (sc!=0)
        te->append(str);
}

void printDialog::printFSwitches()
{
    int sc=0;
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Function Switches")+"</h2></td></tr>");
    str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3><tr>");
    str.append("<td width=\"60\">&nbsp;</td>");
    str.append(doTC(tr("Switch"), "", true));
    str.append(doTL(tr("Function"), "", true));
    str.append(doTL(tr("Parameter"), "", true));
    str.append(doTL(tr("Repeat"), "", true));
    str.append(doTL(tr("Enabled"), "", true));
    str.append("</tr>");
    for(int i=0; i<GetEepromInterface()->getCapability(CustomFunctions); i++) {
      if (g_model->funcSw[i].swtch.type!=SWITCH_TYPE_NONE) {
          str.append("<tr>");
          str.append(doTC(tr("CF")+QString("%1").arg(i+1),"",true));
          str.append(doTC(g_model->funcSw[i].swtch.toString(),"green"));
          str.append(doTC(getFuncName(g_model->funcSw[i].func),"green"));
          str.append(doTC(FuncParam(g_model->funcSw[i].func,g_model->funcSw[i].param,g_model->funcSw[i].paramarm, g_model->funcSw[i].adjustMode),"green"));
          int index=g_model->funcSw[i].func;
          if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncPlayValue || index==FuncPlayPrompt || index==FuncPlayBoth || index==FuncBackgroundMusic) {
            str.append(doTC(QString("%1").arg(g_model->funcSw[i].repeatParam),"green"));
          } else {
            str.append(doTC( "&nbsp;","green"));
          }
          if ((index<=FuncInstantTrim) || (index>FuncBackgroundMusicPause)) {
            str.append(doTC((g_model->funcSw[i].enabled ? "ON" : "OFF"),"green"));
          } else {
            str.append(doTC( "---","green"));
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

void printDialog::printFrSky()
{
  int tc=0;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=10><h2>"+tr("Telemetry Settings")+"</h2></td></tr>");
  str.append("<tr><td colspan=4 align=\"center\">&nbsp;</td><td colspan=3 align=\"center\"><b>"+tr("Alarm 1")+"</b></td><td colspan=3 align=\"center\"><b>"+tr("Alarm 2")+"</b></td></tr>");
  str.append("<tr><td align=\"center\"><b>"+tr("Analog")+"</b></td><td align=\"center\"><b>"+tr("Unit")+"</b></td><td align=\"center\"><b>"+tr("Scale")+"</b></td><td align=\"center\"><b>"+tr("Offset")+"</b></td>");
  str.append("<td width=\"40\" align=\"center\"><b>"+tr("Type")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Condition")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("<td width=\"40\" align=\"center\"><b>"+tr("Type")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Condition")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Value")+"</b></td></tr>");
  FrSkyData *fd=&g_model->frsky;
  for (int i=0; i<2; i++) {
    if (fd->channels[i].ratio!=0) {
      tc++;
      float ratio=(fd->channels[i].ratio/(fd->channels[i].type==0 ?10.0:1));
      str.append("<tr><td align=\"center\"><b>"+tr("A%1").arg(i+1)+"</b></td><td align=\"center\"><font color=green>"+getFrSkyUnits(fd->channels[i].type)+"</font></td><td align=\"center\"><font color=green>"+QString::number(ratio,10,(fd->channels[i].type==0 ? 1:0))+"</font></td><td align=\"center\"><font color=green>"+QString::number((fd->channels[i].offset*ratio)/255,10,(fd->channels[i].type==0 ? 1:0))+"</font></td>");
      str.append("<td width=\"40\" align=\"center\"><font color=green>"+getFrSkyAlarmType(fd->channels[i].alarms[0].level)+"</font></td>");
      str.append("<td width=\"40\" align=\"center\"><font color=green>");
      str.append((fd->channels[i].alarms[0].greater==1) ? "&gt;" : "&lt;");
      str.append("</font></td><td width=\"40\" align=\"center\"><font color=green>"+QString::number(ratio*(fd->channels[i].alarms[0].value/255.0+fd->channels[i].offset/255.0),10,(fd->channels[i].type==0 ? 1:0))+"</font></td>");
      str.append("<td width=\"40\" align=\"center\"><font color=green>"+getFrSkyAlarmType(fd->channels[i].alarms[1].level)+"</font></td>");
      str.append("<td width=\"40\" align=\"center\"><font color=green>");
      str.append((fd->channels[i].alarms[1].greater==1) ? "&gt;" : "&lt;");
      str.append("</font></td><td width=\"40\" align=\"center\"><font color=green>"+QString::number(ratio*(fd->channels[i].alarms[1].value/255.0+fd->channels[i].offset/255.0),10,(fd->channels[i].type==0 ? 1:0))+"</font></td></tr>");
    }
  }
  str.append("<tr><td colspan=10 align=\"Left\" height=\"4px\"></td></tr>");
  str.append("<tr><td colspan=4 align=\"center\" rowspan=2>&nbsp;</td><td colspan=3 align=\"center\"><b>"+tr("Alarm 1")+"</b></td><td colspan=3 align=\"center\"><b>"+tr("Alarm 2")+"</b></td></tr>");
  str.append("<tr><td width=\"40\" align=\"center\"><b>"+tr("Type")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Condition")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Value")+"</b></td>");
  str.append("<td width=\"40\" align=\"center\"><b>"+tr("Type")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Condition")+"</b></td><td width=\"40\" align=\"center\"><b>"+tr("Value")+"</b></td></tr>");
  str.append("<tr><td align=\"Left\" colspan=4><b>"+tr("RSSI Alarm")+"</b></td>");
  str.append("<td width=\"40\" align=\"center\"><b>"+getFrSkyAlarmType(fd->rssiAlarms[0].level)+"</b></td><td width=\"40\" align=\"center\"><b>&lt;</b></td><td width=\"40\" align=\"center\"><b>"+QString::number(fd->rssiAlarms[0].value,10)+"</b></td>");
  str.append("<td width=\"40\" align=\"center\"><b>"+getFrSkyAlarmType(fd->rssiAlarms[1].level)+"</b></td><td width=\"40\" align=\"center\"><b>&lt;</b></td><td width=\"40\" align=\"center\"><b>"+QString::number(fd->rssiAlarms[1].value,10)+"</b></td></tr>");
  str.append("<tr><td colspan=10 align=\"Left\" height=\"4px\"></td></tr>");
  str.append("<tr><td colspan=2 align=\"Left\"><b>"+tr("Frsky serial protocol")+"</b></td><td colspan=8 align=\"left\">"+getFrSkyProtocol(fd->usrProto)+"</td></tr>");
  str.append("<tr><td colspan=2 align=\"Left\"><b>"+tr("System of units")+"</b></td><td colspan=8 align=\"left\">"+getFrSkyMeasure(fd->imperial)+"</td></tr>");
  str.append("<tr><td colspan=2 align=\"Left\"><b>"+tr("Propeller blades")+"</b></td><td colspan=8 align=\"left\">"+getFrSkyBlades(fd->blades)+"</td></tr>");
  str.append("<tr><td colspan=10 align=\"Left\" height=\"4px\"></td></tr></table>");
  
  if (GetEepromInterface()->getCapability(TelemetryBars) || (GetEepromInterface()->getCapability(TelemetryCSFields))) {
    int cols=GetEepromInterface()->getCapability(TelemetryColsCSFields);
    if (cols==0) cols=2;
    for (int j=0; j<GetEepromInterface()->getCapability(TelemetryCSFields)/(4*cols); j++ ) {
      if (fd->screens[j].type==0) {
        if (cols==2) {
          str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=3 align=\"Left\"><b>"+tr("Custom Telemetry View")+"</b></td></tr>");
        } else {
          str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=5 align=\"Left\"><b>"+tr("Custom Telemetry View")+"</b></td></tr>");
        }
        for (int r=0; r<4; r++) {
          str.append("<tr>");
          for (int c=0; c<cols; c++) {
            if (fd->screens[j].body.lines[r].source[c]!=0)
              tc++;
            if (cols==2) {
              str.append("<td  align=\"Center\" width=\"45%\"><b>"+getFrSkySrc(fd->screens[j].body.lines[r].source[c])+"</b></td>");
            } else {
              str.append("<td  align=\"Center\" width=\"30%\"><b>"+getFrSkySrc(fd->screens[j].body.lines[r].source[c])+"</b></td>");
            }
            if (c<(cols-1)) {
              if (cols==2) {
                str.append("<td  align=\"Center\" width=\"10%\"><b>&nbsp;</b></td>");
              } else {
                str.append("<td  align=\"Center\" width=\"5%\"><b>&nbsp;</b></td>");
              }
            }
          }
          str.append("</tr>");
        }
        str.append("</table>");
      } else {
        str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=4 align=\"Left\"><b>"+tr("Telemetry Bars")+"</b></td></tr>");
        str.append("<tr><td  align=\"Center\"><b>"+tr("Bar Number")+"</b></td><td  align=\"Center\"><b>"+tr("Source")+"</b></td><td  align=\"Center\"><b>"+tr("Min")+"</b></td><td  align=\"Center\"><b>"+tr("Max")+"</b></td></tr>");
        for (int i=0; i<4; i++) {
          if (fd->screens[j].body.bars[i].source!=0) 
            tc++;
          str.append("<tr><td  align=\"Center\"><b>"+QString::number(i+1,10)+"</b></td><td  align=\"Center\"><b>"+getFrSkySrc(fd->screens[j].body.bars[i].source)+"</b></td><td  align=\"Right\"><b>"+(fd->screens[j].body.bars[i].source>0 ? QString::number(getBarValue(fd->screens[j].body.bars[i].source, fd->screens[j].body.bars[i].barMin,fd)):"----")+"</b></td><td  align=\"Right\"><b>"+(fd->screens[j].body.bars[i].source>0 ? QString::number(getBarValue(fd->screens[j].body.bars[i].source,(255-fd->screens[j].body.bars[i].barMax),fd)) :"----")+"</b></td></tr>");
        }
        str.append("</table>");
      }
    }
  }
  if (tc>0)
      te->append(str);    
}

void printDialog::on_printButton_clicked()
{
    QPrinter printer;
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
      return;
    te->print(&printer);
}

void printDialog::on_printFileButton_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this,tr("Select PDF output file"),QString(),tr("ODF files (*.odt);;PDF Files(*.pdf);;HTML-Files (*.htm *.html);;All Files (*)")); 
    if (fn.isEmpty())
      return;
    if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".pdf", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive)) )
      fn += ".pdf"; // default
    if (fn.endsWith(".pdf", Qt::CaseInsensitive)) {
      QPrinter printer;
      printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setColorMode(QPrinter::Color);
      printer.setOutputFileName(fn);
      te->print(&printer);
    } else {
      QTextDocumentWriter writer(fn);
       writer.write(te->document());      
    }
}

void printDialog::printToFile()
{
    if (printfilename.isEmpty())
      return;
    if (! (printfilename.endsWith(".odt", Qt::CaseInsensitive) || printfilename.endsWith(".pdf", Qt::CaseInsensitive) || printfilename.endsWith(".htm", Qt::CaseInsensitive) || printfilename.endsWith(".html", Qt::CaseInsensitive)) )
      printfilename += ".pdf"; // default
    if (printfilename.endsWith(".pdf", Qt::CaseInsensitive)) {
      QPrinter printer;
      printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setColorMode(QPrinter::Color);
      printer.setOutputFileName(printfilename);
      te->print(&printer);
    } else {
      QTextDocumentWriter writer(printfilename);
       writer.write(te->document());      
    }
}

void printDialog::autoClose()
{
  this->close();
}
