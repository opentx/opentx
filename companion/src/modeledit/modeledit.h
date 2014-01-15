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
    explicit ModelEdit(RadioData & radioData, int modelId , bool openWizard =false, bool inNew =false, QWidget *parent = 0);
    ~ModelEdit();

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
    QVector<ModelPanel *> panels;

    void addTab(ModelPanel *panel, QString text);
    void launchSimulation();

};

#endif // MODELEDIT_H
