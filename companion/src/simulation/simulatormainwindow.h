#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include "simulator.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QPointer>

#define SIMULATOR_WINDOW_STATE_VERSION    1

class DebugOutput;
class RadioData;
class RadioOutputsWidget;
class SimulatorDialog;
class SimulatorInterface;
class TrainerSimulator;
class TelemetrySimulator;

class QKeySequence;

namespace Ui {
class SimulatorMainWindow;
}

using namespace Simulator;

class SimulatorMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit SimulatorMainWindow(QWidget * parent, SimulatorInterface * simulator, quint8 flags=0, Qt::WindowFlags wflags = Qt::WindowFlags());
    ~SimulatorMainWindow();

    bool setRadioData(RadioData * radioData);
    bool useTempDataPath(bool deleteOnClose = true, bool saveOnClose = false);
    bool setOptions(SimulatorOptions & options, bool withSave = true);
    QMenu * createPopupMenu();

  public slots:
    void start();
    void saveWindowState();
    void showRadioTitlebar(bool show);

  protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void changeEvent(QEvent *e);

  private slots:
    void luaReload(bool);
    void openJoystickDialog(bool);
    void showHelp(bool show);

  private:
    void createDockWidgets();
    void addTool(QDockWidget * widget, Qt::DockWidgetArea area, QIcon icon = QIcon(), QKeySequence shortcut = QKeySequence());

    SimulatorInterface  * m_simulator;

    Ui::SimulatorMainWindow * ui;
    SimulatorDialog * m_simulatorWidget;
    DebugOutput * m_consoleWidget;
    RadioOutputsWidget * m_outputsWidget;

    QDockWidget * m_simulatorDockWidget;
    QDockWidget * m_consoleDockWidget;
    QDockWidget * m_telemetryDockWidget;
    QDockWidget * m_trainerDockWidget;
    QDockWidget * m_outputsDockWidget;

    QVector<keymapHelp_t> m_keymapHelp;
    int m_radioProfileId;
    bool m_firstShow;
    bool m_showRadioTitlebar;

};

#endif // SIMULATORMAINWINDOW_H
