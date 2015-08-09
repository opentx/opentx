#ifndef GENERALEDIT_H
#define GENERALEDIT_H

#include <QtWidgets>
#include "eeprominterface.h"
#include "genericpanel.h"

namespace Ui {
  class GeneralEdit;
}

class GeneralPanel : public GenericPanel
{
  public:
    GeneralPanel(QWidget *parent, GeneralSettings & generalSettings, Firmware * firmware):
      GenericPanel(parent, NULL, generalSettings, firmware)
    {
    }
};

class GeneralEdit : public QDialog
{
  Q_OBJECT

  public:
    GeneralEdit(QWidget * parent, RadioData & radioData, Firmware * firmware);
    ~GeneralEdit();

  private:
    Ui::GeneralEdit *ui;
    GeneralSettings & generalSettings;

    bool switchDefPosEditLock;
    bool voiceLangEditLock;
    bool mavbaudEditLock;
    void getGeneralSwitchDefPos(int i, bool val);
    void setSwitchDefPos();
    void updateVarioPitchRange();
    
  signals:
    void modified();

  private slots:
    void onTabModified();
    void on_tabWidget_currentChanged(int index);
    void on_calretrieve_PB_clicked();
    void on_calstore_PB_clicked();

  private:
    Firmware * firmware;
    QVector<GenericPanel *> panels;
    void addTab(GenericPanel *panel, QString text);
    void closeEvent(QCloseEvent *event);

};

#endif // GENERALEDIT_H
