#ifndef MODELEDIT_H
#define MODELEDIT_H

#include <QDialog>
#include "modelpanel.h"

namespace Ui {
    class ModelEdit;
}

class ModelEdit : public QDialog
{
    Q_OBJECT

  public:
    ModelEdit(QWidget * parent, RadioData & radioData, int modelId, FirmwareInterface * firmware);
    ~ModelEdit();
  
  protected:
    void closeEvent(QCloseEvent *event);

  signals:
    void modified();

  private slots:
    void onTabModified();
    void on_pushButton_clicked();
    void on_tabWidget_currentChanged(int index);

  private:
    Ui::ModelEdit *ui;
    int modelId;
    ModelData & model;
    GeneralSettings & generalSettings;
    FirmwareInterface * firmware;
    QVector<ModelPanel *> panels;

    void addTab(ModelPanel *panel, QString text);
    void launchSimulation();

};

#endif // MODELEDIT_H
