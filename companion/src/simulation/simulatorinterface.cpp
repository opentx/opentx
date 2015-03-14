#include "simulatorinterface.h"
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QMap>
#include <QMessageBox>

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
  QDir dir(".");
  QStringList filters;
#if !defined __GNUC__
  filters << "*-simulator.dll";
#else
  filters << "*-simulator.so";
#endif
  foreach(QString filename, dir.entryList(filters, QDir::Files)) {
    registerSimulator(filename.prepend("./"));
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

