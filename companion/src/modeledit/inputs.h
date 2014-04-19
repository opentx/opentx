#ifndef INPUTS_H
#define INPUTS_H

#include "modelpanel.h"
#include "mixerslist.h"

class InputsPanel : public ModelPanel
{
    Q_OBJECT

  public:
    InputsPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    virtual ~InputsPanel();

    virtual void update();

  private slots:
    void clearExpos();
    void moveExpoUp();
    void moveExpoDown();
    void mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction action);
    void expolistWidget_customContextMenuRequested(QPoint pos);
    void expolistWidget_doubleClicked(QModelIndex index);
    void expolistWidget_KeyPress(QKeyEvent *event);
    void exposDelete(bool ask=true);
    void exposCut();
    void exposCopy();
    void exposPaste();
    void exposDuplicate();
    void expoOpen(QListWidgetItem *item = NULL);
    void expoAdd();

  private:
    bool expoInserted;
    MixersList *ExposlistWidget;

    int getExpoIndex(unsigned int dch);
    bool gm_insertExpo(int idx);
    void gm_deleteExpo(int index);
    void gm_openExpo(int index);
    int gm_moveExpo(int idx, bool dir);
    void exposDeleteList(QList<int> list);
    QList<int> createExpoListFromSelected();
    void setSelectedByExpoList(QList<int> list);
    void pasteExpoMimeData(const QMimeData * mimeData, int destIdx);

};

#endif // INPUTS_H
