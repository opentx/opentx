#ifndef PROGRESS_WIDGET_H_
#define PROGRESS_WIDGET_H_

#include <QWidget>

namespace Ui {
  class ProgressWidget;
}

class ProgressWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit ProgressWidget(QWidget *parent);
    ~ProgressWidget();
    void lock(bool lock);
    void addText(const QString &text);
    QString getText();
    void setInfo(const QString &text);
    void setMaximum(int value);
    int maximum();
    void setValue(int value);
    void setProgressColor(const QColor &color);
    void addSeparator();
    void forceOpen();
    void stop();

  signals:
    void detailsToggled();
    void locked(bool);
    void stopped();

  protected slots:
    void on_checkBox_toggled(bool checked);
    void shrink();

  private:
    Ui::ProgressWidget *ui;
};

#endif // PROGRESS_WIDGET_H_
