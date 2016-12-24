#ifndef MIXES_H
#define MIXES_H

#include "modeledit.h"
#include "mixerslist.h"
#include "mixerdialog.h"
#include "modelprinter.h"

class MixesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    MixesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~MixesPanel();

    virtual void update();

  private slots:
    // TODO all slots?
    void clearMixes();
    void mixersDelete(bool ask=true);
    void mixersCut();
    void mixersCopy();
    void mixersPaste();
    void mixersDuplicate();
    void mixerOpen();
    void mixerAdd();
    void moveMixUp();
    void moveMixDown();
    void mixerHighlight();


    void mixerlistWidget_customContextMenuRequested(QPoint pos);
    void mixerlistWidget_doubleClicked(QModelIndex index);
    void mixerlistWidget_KeyPress(QKeyEvent *event);

    void mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteMixerMimeData(const QMimeData * mimeData, int destIdx);

  private:
    MixersList *MixerlistWidget;
    bool mixInserted;
    unsigned int highlightedSource;
    bool firstLine;
    ModelPrinter modelPrinter;

    int getMixerIndex(unsigned int dch);
    bool gm_insertMix(int idx);
    void gm_deleteMix(int index);
    void gm_openMix(int index);
    int gm_moveMix(int idx, bool dir);
    void mixersDeleteList(QList<int> list);
    QList<int> createMixListFromSelected();
    void setSelectedByMixList(QList<int> list);
    bool AddMixerLine(int dest);
    QString getMixerText(int dest, bool * new_ch);
};

#endif // MIXES_H
