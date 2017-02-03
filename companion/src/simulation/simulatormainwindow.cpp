#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include "appdata.h"
#include "debugoutput.h"
#include "radiooutputswidget.h"
#include "simulatordialog.h"
#include "simulatorinterface.h"
#include "telemetrysimu.h"
#include "trainersimu.h"
#ifdef JOYSTICKS
#include "joystickdialog.h"
#endif

#include <QDebug>

extern AppData g;  // ensure what "g" means

const quint16 SimulatorMainWindow::m_savedUiStateVersion = 1;

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent, SimulatorInterface * simulator, quint8 flags, Qt::WindowFlags wflags) :
  QMainWindow(parent, wflags),
  m_simulator(simulator),
  ui(new Ui::SimulatorMainWindow),
  m_simulatorWidget(NULL),
  m_consoleWidget(NULL),
  m_outputsWidget(NULL),
  m_simulatorDockWidget(NULL),
  m_consoleDockWidget(NULL),
  m_telemetryDockWidget(NULL),
  m_trainerDockWidget(NULL),
  m_outputsDockWidget(NULL),
  m_radioProfileId(g.sessionId()),
  m_firstShow(true),
  m_showRadioTitlebar(true),
  m_showMenubar(true)
{
  ui->setupUi(this);
  ui->centralwidget->hide();

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
//  setDockOptions(dockOptions() | QMainWindow::GroupedDragging);
//#endif

  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

  m_simulatorWidget = new SimulatorDialog(this, m_simulator, flags);

  m_simulatorDockWidget = new QDockWidget(m_simulatorWidget->windowTitle(), this);
  m_simulatorDockWidget->setObjectName("RADIO_SIMULATOR");
  m_simulatorDockWidget->setWidget(m_simulatorWidget);
  m_simulatorDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::BottomDockWidgetArea, m_simulatorDockWidget);
  // setCentralWidget(m_simulatorWidget);

  createDockWidgets();

  ui->actionReloadLua->setIcon(SimulatorIcon("reload_script"));
  ui->actionReloadRadioData->setIcon(SimulatorIcon("restart"));
  ui->actionJoystickSettings->setIcon(SimulatorIcon("joystick_settings"));
  ui->actionScreenshot->setIcon(SimulatorIcon("camera"));
  ui->actionShowKeymap->setIcon(SimulatorIcon("info"));
  ui->actionToggleRadioTitle->setIcon(ui->toolBar->toggleViewAction()->icon());
  ui->actionToggleMenuBar->setIcon(ui->toolBar->toggleViewAction()->icon());

  ui->toolBar->toggleViewAction()->setShortcut(tr("Alt+T"));
  ui->toolBar->setIconSize(SimulatorIcon::toolbarIconSize(g.iconSize()));
  ui->toolBar->insertSeparator(ui->actionReloadLua);

  // add these to this window directly to maintain shorcuts when menubar is hidden
  addAction(ui->toolBar->toggleViewAction());
  addAction(ui->actionToggleMenuBar);

  ui->menuView->addSeparator();
  ui->menuView->addAction(ui->toolBar->toggleViewAction());
  ui->menuView->addAction(ui->actionToggleMenuBar);
  ui->menuView->addAction(ui->actionToggleRadioTitle);

  // Hide some actions based on board capabilities.
  Firmware * firmware = getCurrentFirmware();
  if(!firmware->getCapability(Capability(LuaInputsPerScript)))
    ui->actionReloadLua->setDisabled(true);
  if (!firmware->getCapability(Capability(SportTelemetry)))
    m_telemetryDockWidget->toggleViewAction()->setDisabled(true);
#ifndef JOYSTICKS
  ui->actionJoystickSettings->setDisabled(true);
#endif

  // Add radio-specific help text from simulator widget
  foreach (keymapHelp_t item, *m_simulatorWidget->getKeymapHelp())
    m_keymapHelp.append(item);

  restoreUiState();

  setStyleSheet(SimulatorStyle::styleSheet());

  connect(ui->actionShowKeymap, &QAction::triggered, this, &SimulatorMainWindow::showHelp);
  connect(ui->actionJoystickSettings, &QAction::triggered, this, &SimulatorMainWindow::openJoystickDialog);
  connect(ui->actionReloadLua, &QAction::triggered, this, &SimulatorMainWindow::luaReload);
  connect(ui->actionToggleRadioTitle, &QAction::toggled, this, &SimulatorMainWindow::showRadioTitlebar);
  connect(ui->actionToggleMenuBar, &QAction::toggled, this, &SimulatorMainWindow::toggleMenuBar);
  if (m_simulatorWidget) {
    connect(ui->actionScreenshot, &QAction::triggered, m_simulatorWidget, &SimulatorDialog::captureScreenshot);
    connect(ui->actionReloadRadioData, &QAction::triggered, m_simulatorWidget, &SimulatorDialog::restart);
  }
  if (m_outputsWidget)
    connect(ui->actionReloadRadioData, &QAction::triggered, m_outputsWidget, &RadioOutputsWidget::restart);

  // this sets the radio widget to a fixed width while docked, freeform while floating
  connect(m_simulatorDockWidget, &QDockWidget::topLevelChanged, [this](bool top) {
    if (top)
      m_simulatorWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    else
      m_simulatorWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  });

}

SimulatorMainWindow::~SimulatorMainWindow()
{
  delete ui;
}

void SimulatorMainWindow::closeEvent(QCloseEvent *)
{
  saveUiState();

  if (m_consoleDockWidget)
    delete m_consoleDockWidget;
  if (m_telemetryDockWidget)
    delete m_telemetryDockWidget;
  if (m_trainerDockWidget)
    delete m_trainerDockWidget;
  if (m_outputsDockWidget)
    delete m_outputsDockWidget;
  if (m_simulatorDockWidget)
    delete m_simulatorDockWidget;
}

void SimulatorMainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

QMenu* SimulatorMainWindow::createPopupMenu(){
  QMenu * menu = QMainWindow::createPopupMenu();
  menu->addAction(ui->actionToggleMenuBar);
  menu->addAction(ui->actionToggleRadioTitle);
  return menu;
}

void SimulatorMainWindow::saveUiState()
{
  QByteArray state;
  QDataStream stream(&state, QIODevice::WriteOnly);
  stream << m_savedUiStateVersion << saveState() << m_showMenubar << m_showRadioTitlebar;

  SimulatorOptions opts = g.profile[m_radioProfileId].simulatorOptions();
  opts.windowState = state;
  opts.windowGeometry = saveGeometry();
  g.profile[m_radioProfileId].simulatorOptions(opts);
}

void SimulatorMainWindow::restoreUiState()
{
  quint16 ver = 0;
  QByteArray windowState;
  QByteArray state = g.profile[m_radioProfileId].simulatorOptions().windowState;
  QDataStream stream(state);

  stream >> ver;
  if (ver && ver <= m_savedUiStateVersion)
    stream >> windowState >> m_showMenubar >> m_showRadioTitlebar;

  restoreState(windowState);
  restoreGeometry(g.profile[m_radioProfileId].simulatorOptions().windowGeometry);
  showRadioTitlebar(m_showRadioTitlebar);
  toggleMenuBar(m_showMenubar);
}

bool SimulatorMainWindow::setRadioData(RadioData * radioData)
{
  return m_simulatorWidget->setRadioData(radioData);
}

bool SimulatorMainWindow::useTempDataPath(bool deleteOnClose, bool saveOnClose)
{
  return m_simulatorWidget->useTempDataPath(deleteOnClose, saveOnClose);
}

bool SimulatorMainWindow::setOptions(SimulatorOptions & options, bool withSave)
{
  return m_simulatorWidget->setOptions(options, withSave);
}

void SimulatorMainWindow::start()
{
  if (m_consoleWidget)
    m_consoleWidget->start();
  if (m_simulatorWidget)
    m_simulatorWidget->start();
  if (m_outputsWidget)
    m_outputsWidget->start();
}

void SimulatorMainWindow::createDockWidgets()
{
  if (!m_outputsDockWidget) {
    SimulatorIcon icon("radio_outputs");
    m_outputsDockWidget = new QDockWidget(tr("Radio Outputs"), this);
    m_outputsWidget = new RadioOutputsWidget(m_simulator, getCurrentFirmware(), this);
    m_outputsWidget->setWindowIcon(icon);
    m_outputsDockWidget->setWidget(m_outputsWidget);
    m_outputsDockWidget->setObjectName("OUTPUTS");
    addTool(m_outputsDockWidget, Qt::BottomDockWidgetArea, icon, QKeySequence(tr("F2")));
  }

  if (!m_telemetryDockWidget) {
    SimulatorIcon icon("telemetry");
    m_telemetryDockWidget = new QDockWidget(tr("Telemetry Simulator"), this);
    TelemetrySimulator * telem = new TelemetrySimulator(this, m_simulator);
    telem->setWindowIcon(icon);
    m_telemetryDockWidget->setWidget(telem);
    m_telemetryDockWidget->setObjectName("TELEMETRY_SIMULATOR");
    addTool(m_telemetryDockWidget, Qt::LeftDockWidgetArea, icon, QKeySequence(tr("F4")));
  }

  if (!m_trainerDockWidget) {
    SimulatorIcon icon("trainer");
    m_trainerDockWidget = new QDockWidget(tr("Trainer Simulator"), this);
    TrainerSimulator * trainer = new TrainerSimulator(this, m_simulator);
    trainer->setWindowIcon(icon);
    m_trainerDockWidget->setWidget(trainer);
    m_trainerDockWidget->setObjectName("TRAINER_SIMULATOR");
    addTool(m_trainerDockWidget, Qt::TopDockWidgetArea, icon, QKeySequence(tr("F5")));
  }

  if (!m_consoleDockWidget) {
    SimulatorIcon icon("console");
    m_consoleDockWidget = new QDockWidget(tr("Debug Output"), this);
    m_consoleWidget = new DebugOutput(this, m_simulator);
    m_consoleWidget->setWindowIcon(icon);
    m_consoleDockWidget->setWidget(m_consoleWidget);
    m_consoleDockWidget->setObjectName("CONSOLE");
    addTool(m_consoleDockWidget, Qt::RightDockWidgetArea, icon, QKeySequence(tr("F6")));
  }
}

void SimulatorMainWindow::addTool(QDockWidget * widget, Qt::DockWidgetArea area, QIcon icon, QKeySequence shortcut)
{
  QAction* tempAction = widget->toggleViewAction();
  tempAction->setIcon(icon);
  tempAction->setShortcut(shortcut);
  ui->menuView->addAction(tempAction);
  ui->toolBar->insertAction(ui->actionReloadLua, tempAction);
  widget->setAllowedAreas(Qt::AllDockWidgetAreas);
  widget->setWindowIcon(icon);
  addDockWidget(area, widget);
  widget->hide();
  widget->setFloating(true);
}

void SimulatorMainWindow::showRadioTitlebar(bool show)
{
  m_showRadioTitlebar = show;
  if (show) {
    QWidget * w = m_simulatorDockWidget->titleBarWidget();
    if (w)
      w->deleteLater();
    m_simulatorDockWidget->setTitleBarWidget(0);
  }
  else {
    m_simulatorDockWidget->setTitleBarWidget(new QWidget());
  }
  if (ui->actionToggleRadioTitle->isChecked() != show)
    ui->actionToggleRadioTitle->setChecked(show);

}

void SimulatorMainWindow::toggleMenuBar(bool show)
{
  m_showMenubar = show;
  ui->menubar->setVisible(show);
  if (ui->actionToggleMenuBar->isChecked() != show)
    ui->actionToggleMenuBar->setChecked(show);
}

void SimulatorMainWindow::luaReload(bool)
{
  // force a reload of the lua environment
  if (m_simulator)
    m_simulator->setLuaStateReloadPermanentScripts();
}

void SimulatorMainWindow::openJoystickDialog(bool)
{
#ifdef JOYSTICKS
  joystickDialog * jd = new joystickDialog(this);
  if (jd->exec() == QDialog::Accepted && m_simulatorWidget)
    m_simulatorWidget->setupJoysticks();
  jd->deleteLater();
#endif
}

void SimulatorMainWindow::showHelp(bool show)
{
  QString helpText = tr("Simulator Controls:");
  helpText += "<table cellspacing=4 cellpadding=0>";
  helpText += tr("<tr><th>Key/Mouse</td><th>Action</td></tr>");
  QString keyTemplate = "<tr><td align='center'><pre>%1</pre></td><td align='center'>%2</td></tr>";
  foreach (keymapHelp_t pair, m_keymapHelp)
    helpText += keyTemplate.arg(pair.first, pair.second);
  helpText += "</table>";

  QMessageBox * msgBox = new QMessageBox(this);
  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setWindowFlags(msgBox->windowFlags() | Qt::WindowStaysOnTopHint);
  msgBox->setStandardButtons( QMessageBox::Ok );
  msgBox->setWindowTitle(tr("Simulator Help"));
  msgBox->setText(helpText);
  msgBox->setModal(false);
  msgBox->show();
}
