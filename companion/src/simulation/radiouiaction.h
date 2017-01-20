#ifndef RADIOUIACTION_H
#define RADIOUIACTION_H

#include <QtCore>
#include <QWidget>
#include <QKeyEvent>

/*
 * This class is somewhat like a QAction but specific for the radio UI.
 * Actions can have one or more keyboard shortcuts associated with them (currently single-key only, w/out modifiers).
 */
class RadioUiAction : public QObject
{
  Q_OBJECT

  public:
    /*
     * @param index    Typically this is the hardware array index corresponding to button on current radio model,
     *                   but it could be anything. Use -1 or any other negative value for non-hardware indices.
     * @param key      An optional Qt:Key code for shortcut.
     * @param parent   Parent widget, required for handling keyboard events.
     * @param text     Optional title for this action. The text and description are currently used in generated help text.
     * @param descript Optional longer description text for this action.
     */
    RadioUiAction(int index = -1, int key = 0, QWidget * parent = NULL, const QString &text = "", const QString &descript = ""):
      m_hwIndex(index),
      m_active(false),
      m_keys(QList<int>()),
      m_text(text),
      m_description(descript),
      m_parent(parent)
    {
      if (key)
        m_keys.append(key);

      init();
    }
    /*
     * @param keys QList of Qt:Key codes to use as shortcuts.
     *   [See above for other params.]
     */
    RadioUiAction(int index, QList<int> keys, QWidget * parent = NULL, const QString &text = "", const QString &descript = ""):
      m_hwIndex(index),
      m_active(false),
      m_keys(keys),
      m_text(text),
      m_description(descript),
      m_parent(parent)
    {
      init();
    }

    void init()
    {
      if (m_keys.size())
        addShortcut();
    }

    void addShortcut()
    {
      if (m_parent)
        m_parent->installEventFilter(this);
    }

    void setDescription(const QString & description)    { m_description = description; }
    void setText(QPair<QString, QString> name_descript) { setText(name_descript.first, name_descript.second); }

    void setText(const QString & text, const QString & description = "")
    {
      m_text = text;
      if (!description.isEmpty())
        setDescription(description);
    }

    int     getIndex()       const { return m_hwIndex; }
    bool    isActive()       const { return m_active; }
    QString getDescription() const { return m_description; }
    QString getText()        const { return m_text; }

    bool eventFilter(QObject * obj, QEvent * event)
    {
      if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((!keyEvent->modifiers() || keyEvent->modifiers() == Qt::KeypadModifier) && m_keys.contains(keyEvent->key())) {
          trigger(event->type() == QEvent::KeyPress);
          return true;
        }
      }
      return QObject::eventFilter(obj, event);
    }

  public slots:

    // "toggle" is the most basic way to set the action status and emits only one "toggled" signal
    bool toggle(bool active = true)
    {
      if (active != m_active) {
        m_active = active;
        emit toggled(m_hwIndex, active);
        return true;
      }
      return false;
    }

    // "trigger" indicates a user-initiated action and emits more signals than "toggle" does
    void trigger(bool active = true)
    {
      if (toggle(active)) {
        emit triggered(m_hwIndex, active);
        if (active)
          emit pushed(m_hwIndex);
      }
    }

  private:

    int m_hwIndex;
    bool m_active;
    QList<int> m_keys;
    QString m_text;
    QString m_description;
    QWidget * m_parent;


  signals:
    void toggled(int index, bool active);    // on programmatic or user interaction change
    void triggered(int index, bool active);  // on user interaction change only
    void pushed(int index);                  // only emitted on user interaction && when 'active' is true
};

#endif // RADIOUIACTION_H










