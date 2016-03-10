#ifndef BURNCONFIGDIALOG_H_
#define BURNCONFIGDIALOG_H_

#include <QtWidgets>

#define MEM_TYPE_EEPROM 1
#define MEM_TYPE_FLASH  2

#define OPR_TYPE_READ  1
#define OPR_TYPE_WRITE 2

namespace Ui {
    class burnConfigDialog;
}

class burnConfigDialog : public QDialog {
    Q_OBJECT
public:
    burnConfigDialog(QWidget *parent = 0);
    ~burnConfigDialog();

    QString getAVRDUDE() {return avrLoc;}
    QString getSAMBA() {return sambaLoc;}
    QString getDFU() {return dfuLoc;}
    QStringList getAvrdudeArgs() { QStringList args = avrArgs; if (!avrPort.isEmpty()) args << "-P" << avrPort; return args; }
    QStringList getDFUArgs() {return dfuArgs;}
    QString getProgrammer() {return avrProgrammer;}
    QString getMCU() {return avrMCU;}
    QString getArmMCU() {return armMCU;}
    QString getPort() {return avrPort;}
    QString getSambaPort() {return sambaPort;}

    void listAvrdudeProgrammers();

private:
    Ui::burnConfigDialog *ui;

    void populateProgrammers();

    QString avrLoc;
    QString sambaLoc;
    QString dfuLoc;
    QStringList avrArgs;
    QString avrProgrammer;
    QString avrMCU;
    QString armMCU;
    QString avrPort;
    QString sambaPort;
    QStringList dfuArgs;

private slots:
    void shrink();
    void on_avrArgs_editingFinished();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_avrdude_location_editingFinished();
    void on_avrdude_programmer_currentIndexChanged(QString );
    void on_avrdude_mcu_currentIndexChanged(QString );
    void on_avrdude_port_currentIndexChanged(QString );
    void on_samba_location_editingFinished();
    void on_samba_port_editingFinished();
    void on_arm_mcu_currentIndexChanged(QString );
    void on_sb_browse_clicked();
    void on_dfu_browse_clicked();
    void on_dfu_location_editingFinished();
    void on_dfuArgs_editingFinished();
    void on_advCtrChkB_toggled(bool checked);
    void getSettings();
    void putSettings();
};

#endif // BURNCONFIGDIALOG_H_
