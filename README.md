## OpenTX 2.2 Branch

[![Travis build Status](https://travis-ci.org/opentx/opentx.svg?branch=2.2)](https://travis-ci.org/opentx/opentx)
[![Quality Gate](https://sonarcloud.io/api/badges/gate?key=OpenTX:2.2)](https://sonarcloud.io/dashboard?id=OpenTX:2.2)
[![Join the chat at https://opentx.rocket.chat](https://camo.githubusercontent.com/3d659054abd6ce21c0e47cf3b83a51bda69ca282/68747470733a2f2f64656d6f2e726f636b65742e636861742f696d616765732f6a6f696e2d636861742e737667)](https://opentx.rocket.chat)
[![Donate using Paypal](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DJ9MASSKVW8WN)

The ongoing development on 2.2.x is done in this branch.

Refer to the [OpenTX wiki](https://github.com/opentx/opentx/wiki) for information about setting up the tool chains for building OpenTX and OpenTX Companion as well as other development related issues.

Useful links:
 * OpenTX Main Site http://www.open-tx.org/
 * OpenTx University http://open-txu.org/
 * OpenTX User Manual https://www.gitbook.com/book/opentx/opentx-taranis-manual/details
 * OpenTX Lua Reference Guide https://www.gitbook.com/book/opentx/opentx-lua-reference-guide/details

For Ubuntu:
 * apt-get install cmake git gdb gcc cmake gcc-avr avr-libc build-essential libfox-1.6-dev libsdl1.2-dev python-pyqt5 qt5-default qttools5-dev-tools qtmultimedia5-dev qttools5-dev libqt5svg5-dev python3-pyqt5.qtsvg python3-pyside.qtwebkit gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib libgtest-dev

For Fedora:
 * dnf install git gdb gcc qt qt-devel cmake patch xsd gcc-c++ SDL-devel phonon phonon-devel avr-gcc-c++ avr-gcc avr-binutils arm-none-eabi-binutils-cs arm-none-eabi-gcc-cs arm-none-eabi-gcc-cs-c++ arm-none-eabi-newlib.noarch avr-binutils avr-gcc avr-gcc-c++ avr-libc.noarch PyQt4 fox-devel qt5 qt5-qtmultimedia-devel qt5-devel.noarch qt5-qtsvg-devel qt5-qttools-devel dfu-util

git clone horus in workspace.

mkdir x10 && cd x10

For release:
 * cmake -DPCB=X10 -DGVARS=YES -DLUA=YES -DLUA_COMPILER=YES -DDEFAULT_MODE=2 -DMULTIMODULE=YES ../horus

For debug
 * cmake -DPCB=X10 -DGVARS=YES -DLUA=YES -DLUA_COMPILER=YES -DDEFAULT_MODE=2 -DMULTIMODULE=YES -DDEBUG=ON -DCMAKE_BUILD_TYPE=Debug ../horus


make help