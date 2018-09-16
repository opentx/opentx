set(WINDOWS_DIRECTORY gui/common/windows)
include_directories(${WINDOWS_DIRECTORY})

set(SRC
    ${SRC}
    ${WINDOWS_DIRECTORY}/mainwindow.cpp
    ${WINDOWS_DIRECTORY}/window.cpp
    ${WINDOWS_DIRECTORY}/button.cpp
    ${WINDOWS_DIRECTORY}/keyboard_text.cpp
    ${WINDOWS_DIRECTORY}/keyboard_number.cpp
    ${WINDOWS_DIRECTORY}/keyboard_curve.cpp
    ${WINDOWS_DIRECTORY}/checkbox.cpp
    ${WINDOWS_DIRECTORY}/textedit.cpp
    ${WINDOWS_DIRECTORY}/numberedit.cpp
    ${WINDOWS_DIRECTORY}/timeedit.cpp
    ${WINDOWS_DIRECTORY}/choice.cpp
    ${WINDOWS_DIRECTORY}/tabsgroup.cpp
    ${WINDOWS_DIRECTORY}/page.cpp
    ${WINDOWS_DIRECTORY}/sourcechoice.cpp
    ${WINDOWS_DIRECTORY}/switchchoice.cpp
    ${WINDOWS_DIRECTORY}/filechoice.cpp
    ${WINDOWS_DIRECTORY}/slider.cpp
    ${WINDOWS_DIRECTORY}/menu.cpp
    ${WINDOWS_DIRECTORY}/dialog.cpp
    ${WINDOWS_DIRECTORY}/curve.cpp
    ${WINDOWS_DIRECTORY}/curveedit.cpp
    ${WINDOWS_DIRECTORY}/coloredit.cpp
    )