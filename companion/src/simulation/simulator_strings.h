#ifndef SIMULATOR_STRINGS_H
#define SIMULATOR_STRINGS_H

#include <QtGlobal>

#define SIMU_STR_HLP_KEY_ENTER         QT_TRANSLATE_NOOP("SimulatedUIWidget", "ENTER")
#define SIMU_STR_HLP_KEY_PGUP          QT_TRANSLATE_NOOP("SimulatedUIWidget", "PG-UP")
#define SIMU_STR_HLP_KEY_PGDN          QT_TRANSLATE_NOOP("SimulatedUIWidget", "PG-DN")
#define SIMU_STR_HLP_KEY_DEL           QT_TRANSLATE_NOOP("SimulatedUIWidget", "DEL")
#define SIMU_STR_HLP_KEY_BKSP          QT_TRANSLATE_NOOP("SimulatedUIWidget", "BKSP")
#define SIMU_STR_HLP_KEY_ESC           QT_TRANSLATE_NOOP("SimulatedUIWidget", "ESC")
#define SIMU_STR_HLP_KEY_INS           QT_TRANSLATE_NOOP("SimulatedUIWidget", "INS")
#define SIMU_STR_HLP_KEY_PLS           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>+</font>")
#define SIMU_STR_HLP_KEY_MIN           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>-</font>")

#define SIMU_STR_HLP_KEY_LFT           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&larr;</font>")  // ←
#define SIMU_STR_HLP_KEY_RGT           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&rarr;</font>")  // →
#define SIMU_STR_HLP_KEY_UP            QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&uarr;</font>")  // ↑
#define SIMU_STR_HLP_KEY_DN            QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&darr;</font>")  // ↓

#define SIMU_STR_HLP_ROTENC            QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x2686;</font>")  // ⚆
#define SIMU_STR_HLP_ROTENC_LFT        QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21b6;</font>")  // ↶
#define SIMU_STR_HLP_ROTENC_RGT        QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21b7;</font>")  // ↷
#define SIMU_STR_HLP_ROTENC_LR         QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21c6;</font>")  // ⇆
#define SIMU_STR_HLP_ROTENC_DN         QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21d3;</font>")  // ⇓

#define SIMU_STR_HLP_SCRL_UP           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21d1;</font>")  // ⇑
#define SIMU_STR_HLP_SCRL_DN           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21d3;</font>")  // ⇓
#define SIMU_STR_HLP_SCRL_UD           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x21d5;</font>")  // ⇕

#define SIMU_STR_HLP_MOUSE             QT_TRANSLATE_NOOP("SimulatedUIWidget", "<img src='qrc:/images/simulator/icons/svg/mouse.svg' width=20 height=18 />")
#define SIMU_STR_HLP_CLICK             QT_TRANSLATE_NOOP("SimulatedUIWidget", "<img src='qrc:/images/simulator/icons/svg/arrow_click.svg' width=18 height=18 />")
#define SIMU_STR_HLP_MOUSE_MID         SIMU_STR_HLP_MOUSE SIMU_STR_HLP_CLICK
#define SIMU_STR_HLP_MOUSE_UP          SIMU_STR_HLP_MOUSE SIMU_STR_HLP_SCRL_UP
#define SIMU_STR_HLP_MOUSE_DN          SIMU_STR_HLP_MOUSE SIMU_STR_HLP_SCRL_DN
#define SIMU_STR_HLP_MOUSE_SCRL        SIMU_STR_HLP_MOUSE SIMU_STR_HLP_SCRL_UD

#define SIMU_STR_HLP_ACT_MENU          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ MENU ]</pre>")
#define SIMU_STR_HLP_ACT_PAGE          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ PAGE ]</pre>")
#define SIMU_STR_HLP_ACT_EXIT          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ EXIT ]</pre>")
#define SIMU_STR_HLP_ACT_ENT           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ ENT ]</pre>")
#define SIMU_STR_HLP_ACT_UP            QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ UP ]</pre>")
#define SIMU_STR_HLP_ACT_DN            QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ DN ]</pre>")
#define SIMU_STR_HLP_ACT_PLS           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ <font size=+2>+</font> ]</pre>")
#define SIMU_STR_HLP_ACT_MIN           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ <font size=+2>-</font> ]</pre>")
#define SIMU_STR_HLP_ACT_PGUP          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ PgUp ]</pre>")
#define SIMU_STR_HLP_ACT_PGDN          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ PgDn ]</pre>")
#define SIMU_STR_HLP_ACT_MDL           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ MDL ]</pre>")
#define SIMU_STR_HLP_ACT_RTN           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ RTN ]</pre>")
#define SIMU_STR_HLP_ACT_SYS           QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ SYS ]</pre>")
#define SIMU_STR_HLP_ACT_TELE          QT_TRANSLATE_NOOP("SimulatedUIWidget", "<pre>[ TELE ]</pre>")
#define SIMU_STR_HLP_ACT_MENU_ICN      QT_TRANSLATE_NOOP("SimulatedUIWidget", "<font size=+3>&#x2261;</font>")  // ≡


#define SIMU_STR_HLP_KEYS_EXIT         SIMU_STR_HLP_KEY_DEL "|" SIMU_STR_HLP_KEY_BKSP "|" SIMU_STR_HLP_KEY_ESC
#define SIMU_STR_HLP_KEYS_ACTIVATE     SIMU_STR_HLP_KEY_ENTER "|" SIMU_STR_HLP_MOUSE_MID
#define SIMU_STR_HLP_KEYS_GO_UP        SIMU_STR_HLP_KEY_UP "|" SIMU_STR_HLP_KEY_PGUP
#define SIMU_STR_HLP_KEYS_GO_DN        SIMU_STR_HLP_KEY_DN "|" SIMU_STR_HLP_KEY_PGDN
#define SIMU_STR_HLP_KEYS_GO_LFT       SIMU_STR_HLP_KEY_LFT "|" SIMU_STR_HLP_KEY_MIN "|" SIMU_STR_HLP_MOUSE_UP
#define SIMU_STR_HLP_KEYS_GO_RGT       SIMU_STR_HLP_KEY_RGT "|" SIMU_STR_HLP_KEY_PLS "|" SIMU_STR_HLP_MOUSE_DN

#define SIMU_STR_HLP_ACT_ROT_LFT       SIMU_STR_HLP_ROTENC " " SIMU_STR_HLP_ROTENC_LFT
#define SIMU_STR_HLP_ACT_ROT_RGT       SIMU_STR_HLP_ROTENC " " SIMU_STR_HLP_ROTENC_RGT
#define SIMU_STR_HLP_ACT_ROT_DN        SIMU_STR_HLP_ROTENC " " SIMU_STR_HLP_ROTENC_DN

#endif // SIMULATOR_STRINGS_H
