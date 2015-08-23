#ifndef _MULTIMODELPRINTER_H
#define _MULTIMODELPRINTER_H

#include <QObject>
#include "eeprominterface.h"
#include "modelprinter.h"

class MultiModelPrinter: public QObject
{
  public:
    MultiModelPrinter(Firmware * firmware);
    virtual ~MultiModelPrinter();
    
    void setModel(int idx, const ModelData & model);
    QString print();

  protected:
    class MultiColumns {
      public:
        MultiColumns(int count);
        ~MultiColumns();
        bool isEmpty();
        QString print();
        void append(const QString & str);
        void appendTitle(const QString & name);
        void append(int idx, const QString & str);
        template <class T> void append(int idx, T val);
        void beginCompare();
        void endCompare(const QString & color = "grey");
      private:
        int count;
        QString * columns;
        QString * compareColumns;
    };

    Firmware * firmware;
    GeneralSettings defaultSettings;
    QVector<ModelData *> models; // TODO const
    QVector<ModelPrinter *> modelPrinters;

    QString printTitle(const QString & label);
    QString printSetup();
    QString printFlightModes();
    QString printLimits();
    QString printInputs();
    QString printMixers();
    QString printCurves();
    QString printGvars();
    QString printLogicalSwitches();
    QString printCustomFunctions();
    QString printTelemetry();
};

#endif // _MULTIMODELPRINTER_H
