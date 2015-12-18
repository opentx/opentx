#include "simulatorinterface.h"
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QLibraryInfo>
#include <QMap>
#include <QMessageBox>
#include "version.h"
#if defined WIN32 || !defined __GNUC__
  #include <windows.h>
#endif

QMap<QString, SimulatorFactory *> registered_simulators;

void registerSimulatorFactory(SimulatorFactory *factory)
{
  qDebug() << "registering" << factory->name() << "simulator";
  registered_simulators[factory->name()] = factory;
}

void registerSimulator(const QString &filename)
{
  QLibrary lib(filename);
  typedef SimulatorFactory * (*RegisterSimulator)();
  RegisterSimulator registerSimulator = (RegisterSimulator)lib.resolve("registerSimu");
  if (registerSimulator) {
    SimulatorFactory *factory = registerSimulator();
    registerSimulatorFactory(factory);
  }
  else {
    qWarning() << "Library error" << filename << lib.errorString();
  }
}

void registerSimulators()
{
  bool simulatorsFound = false;
  QDir dir(".");
  QStringList filters;
#if defined(__APPLE__)
  filters << "*-simulator.dylib";
#elif (!defined __GNUC__) || (defined __CYGWIN__)
  filters << "*-simulator.dll";
#else
  filters << "*-simulator.so";
#endif

  foreach(QString filename, dir.entryList(filters, QDir::Files)) {
    registerSimulator(filename.prepend("./"));
    simulatorsFound = true;
  }

  if (!simulatorsFound) {
#if defined(__APPLE__)
    dir = QLibraryInfo::location(QLibraryInfo::PrefixPath) + "/Resources";
#elif (!defined __GNUC__) 
    char name[MAX_PATH];
    GetModuleFileName(NULL, name, MAX_PATH);
	QString path(name);
	path.truncate(path.lastIndexOf('\\'));
	dir.setPath(path);
#else
    dir = SIMULATOR_LIB_SEARCH_PATH;
#endif
    foreach(QString filename, dir.entryList(filters, QDir::Files)) {
      registerSimulator(filename.prepend(dir.path() + "/"));
      simulatorsFound = true;
    }
  }
}

SimulatorFactory *getSimulatorFactory(const QString &name)
{
  QString simuName = name;
  while(1) {
    qDebug() << "searching" << simuName << "simulator";
    foreach (QString name, registered_simulators.keys()) {
      if (name.contains(simuName)) {
        simuName = name;
        qDebug() << "found" << simuName;
        return registered_simulators[simuName];
      }
    }
    int pos = simuName.lastIndexOf('-');
    if (pos <= 0)
      break;
    simuName = simuName.mid(0, pos);
  }
  return NULL;
}

void unregisterSimulators()
{
  foreach(SimulatorFactory *factory, registered_simulators) {
    delete factory;
  }
}
