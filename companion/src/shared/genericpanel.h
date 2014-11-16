#ifndef GENERICPANEL_H_
#define GENERICPANEL_H_

#include <QWidget>

class ModelData;
class GeneralSettings;
class FirmwareInterface;
class QGridLayout;
class QString;

class GenericPanel : public QWidget
{
  Q_OBJECT

  friend class AutoComboBox;
  friend class AutoDoubleSpinBox;
  friend class AutoCheckBox;
  friend class AutoHexSpinBox;
  friend class AutoLineEdit;

  public:
    GenericPanel(QWidget *parent, ModelData * model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    virtual ~GenericPanel();

  signals:
    void modified();

  public slots:
    virtual void update();

  protected:
    ModelData * model;
    GeneralSettings & generalSettings;
    FirmwareInterface * firmware;
    bool lock;
    void addLabel(QGridLayout * gridLayout, const QString &text, int col, bool mimimize=false);
    void addEmptyLabel(QGridLayout * gridLayout, int col);
    void addHSpring(QGridLayout *, int col, int row);
    void addVSpring(QGridLayout *, int col, int row);
    void addDoubleSpring(QGridLayout *, int col, int row);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    void disableMouseScrolling();
};

#endif
