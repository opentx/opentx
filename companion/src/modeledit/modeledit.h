#ifndef MODELEDIT_H
#define MODELEDIT_H

#include <QtWidgets>
#include "genericpanel.h"

class RadioData;

namespace Ui {
  class ModelEdit;
}

class ModelPanel : public GenericPanel
{
  public:
    ModelPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
      GenericPanel(parent, &model, generalSettings, firmware)
    {
    }
};

class ModelEdit : public QDialog
{
    Q_OBJECT

  public:
    ModelEdit(QWidget * parent, RadioData & radioData, int modelId, Firmware * firmware);
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
    Firmware * firmware;
    QVector<GenericPanel *> panels;

    void addTab(GenericPanel *panel, QString text);
    void launchSimulation();

};

#endif // MODELEDIT_H
