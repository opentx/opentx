#include "simulateduiwidget.h"
#include "ui_simulateduiwidgetX7.h"

SimulatedUIWidgetX7::SimulatedUIWidgetX7(SimulatorInterface *simulator, QWidget * parent):
  SimulatedUIWidget(simulator, parent),
  ui(new Ui::SimulatedUIWidgetX7)
{
  RadioUiAction * act;
  QPolygon polygon;

  ui->setupUi(this);

  // add actions in order of appearance on the help menu

  QPoint ctr(70, 91);
  polygon << polyArc(ctr.x(), ctr.y(), 50, -90, 90) << polyArc(ctr.x(), ctr.y(), 22, -90, 90);
  act = addRadioUiAction(3, QList<int>() << Qt::Key_PageUp, tr("PG-UP"), tr("[ PAGE ]"));
  ui->leftbuttons->addArea(polygon, "X7/left_page.png", act);

  act = addRadioUiAction(0, QList<int>() << Qt::Key_PageDown, tr("PG-DN"), tr("[ MENU ]"));
  ui->leftbuttons->addArea(polyArc(ctr.x(), ctr.y(), 20), "X7/left_menu.png", act);

  polygon.clear();
  polygon << polyArc(ctr.x(), ctr.y(), 50, 90, 270) << polyArc(ctr.x(), ctr.y(), 22, 90, 270);
  act = addRadioUiAction(1, QList<int>() << Qt::Key_Delete << Qt::Key_Escape << Qt::Key_Backspace, tr("DEL/BKSP/ESC"), tr("[ EXIT ]"));
  ui->leftbuttons->addArea(polygon, "X7/left_exit.png", act);

  m_keymapHelp.append(keymapHelp_t(tr("WHEEL/PAD SCRL"),  tr("Rotary Selector")));
  m_scrollUpAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Minus << Qt::Key_Equal << Qt::Key_Up, tr("-/UP"), tr("Rotary UP"));
  m_scrollDnAction = addRadioUiAction(-1, QList<int>() << Qt::Key_Plus << Qt::Key_Down, tr("+/DN"), tr("Rotary DOWN"));
  connectScrollActions();

  act = addRadioUiAction(2, QList<int>() << Qt::Key_Enter << Qt::Key_Return, tr("ENTER/MOUSE-MID"), tr("Selector Press"));
  ui->rightbuttons->addArea(polyArc(88, 92, 33), "X7/right_ent.png", act);

  ui->leftbuttons->addArea(9, 154, 34, 177, "X7/left_scrnshot.png", m_screenshotAction);

  m_backlightColors << QColor(215, 243, 255);  // X7 Blue
  m_backlightColors << QColor(166,247,159);
  m_backlightColors << QColor(247,159,166);
  m_backlightColors << QColor(255,195,151);
  m_backlightColors << QColor(247,242,159);

  setLcd(ui->lcd);

  QString css = "#radioUiWidget {"
                  "background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1,"
                    "stop:0 rgba(255, 255, 255, 255),"
                    "stop:0.757062 rgba(241, 238, 238, 255),"
                    "stop:1 rgba(247, 245, 245, 255));"
                "}";

  QTimer * tim = new QTimer(this);
  tim->setSingleShot(true);
  connect(tim, &QTimer::timeout, [this, css]() {
    emit customStyleRequest(css);
  });
  tim->start(100);
}

SimulatedUIWidgetX7::~SimulatedUIWidgetX7()
{
  delete ui;
}

