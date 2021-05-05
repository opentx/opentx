# OpenTX 2.4 Branch

[![Travis build Status](https://travis-ci.org/opentx/opentx.svg?branch=2.4)](https://travis-ci.org/opentx/opentx)
[![Join the Discord chat](https://img.shields.io/badge/discord-join_chat-yellow.svg)](https://discord.gg/CZCwVx2)
[![Donate using Paypal](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DJ9MASSKVW8WN)

## Introduction
OpenTX is open source firmware for RC radio transmitters. The firmware is highly configurable and brings much more features than found in traditional radios. The daily feedback from the thousands of users ensures the continued stability and quality of the firmware. For the list of supported radios refer to <https://www.open-tx.org/radios>.

OpenTX Companion is the open source software used for many different tasks such as downloading and loading the latest OpenTX firmware onto the radio, backing up model settings, editing settings and running radio simulators.
OpenTX Companion is available for Windows, Apple OSX and Linux.

Radio manufacturers seeking to incorporate OpenTX into their products should refer to <https://www.open-tx.org/newradios.html> *(Note: this web page will requie updating to GPLv3)*

## Quicklinks:
 * OpenTX Main Site https://www.open-tx.org/
 * OpenTx University http://open-txu.org/
 * OpenTX Lua Reference Guide https://www.gitbook.com/book/opentx/opentx-lua-reference-guide/details
 * OpenTX Taranis User Manual https://www.gitbook.com/book/opentx/opentx-taranis-manual/details

## Acknowledgements

OpenTX is based on a number of projects code named:
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x

The ongoing support and development of the project has and continues to rely on the efforts of a small group of volunteers who contribute their own time free of charge.

## Toolchain and other developer information

Refer to the [OpenTX Wiki](https://github.com/opentx/opentx/wiki) for information about setting up the tool chains for building OpenTX Firmware and OpenTX Companion as well as other development related issues.

## OpenTX Licensing

OpenTX Firmware and OpenTX Companion are licensed under GNU General Public License version 3 (GPLv3) or any later version. For full terms and conditions refer to LICENSE file.

*OpenTX Sound is licensed under ???????  (Cannot find source code. Is this even supported or should all references be removed?)*

OpenTX documentation is licensed under GNU Free Documentation License version 1.3 (FDLv1.3) or any later version. For full details refer <https://www.gnu.org/licenses/fdl-1.3.html>

## Licenses used in OpenTX

OpenTX contains some code that is not provided under the GPLv3, but rather under specific licenses from the original authors or projects.

OpenTX gratefully acknowledges these and other contributions to OpenTX. We recommend that programs that use OpenTX also acknowledge these contributions, and quote these license statements in an appendix to the documentation.

Note: You only need to comply with (and acknowledge) the licenses of the third-party components that you are using with your application. For more details, refer to the specific licensing requirments contained within each module.

### Third-party Licenses

The following table lists parts (modules) of OpenTX that incorporate code licensed under third-party open-source licenses:

**OpenTX Firmware**

| Product | License | Source | Notes |
| --- | --- | --- | --- |
| CoOS | BSD<sup>1</sup> | unknown | *Question to OpenTX devs: is this still used?* |
| dirent | MIT | <https://github.com/tronkko/dirent> | |
| FatFs | BSD<sup>1</sup> | <https://github.com/abbrev/fatfs> | |
| GCS_MAVLink | MIT | <https://mavlink.io> | *Question to OpenTX devs: not in 2.4 branch, was it stopped being used in 2.2?* |
| libACCESS | libACCESS | <https://www.frsky-rc.com/> | |
| libopenui | GPLv3 | <https://github.com/opentx/libopenui> | |
| lua | MIT | <http://www.lua.org> | |
| msinttypes | BSD<sup>1</sup> | unknown  | |
| STM32 USB | MCD-ST | <http://www.st.com> | |
| STM32 F2xx | MCD-ST | <http://www.st.com> | |
| STM32 F4xx | MCD-ST | <http://www.st.com> | |
| TWI | TWI | <http://www.atmel.com/design-support> | |
|   |   |   |   |

Note 1 - BSD style license
<br></br>
**OpenTX Companion**

| Product | License | Source | Notes |
| --- | --- | --- | --- |
| maxLibQt | GPLv3 | <https://github.com/mpaperno/maxLibQt>  | |
| miniz | GPLv3 | <https://github.com/paulharris/miniz>  | |
| QCustomPlot | GPLv3 | <http://www.qcustomplot.com>  | |
| Qt<sup>TM</sup> | GPLv3 | <https://www.qt.io> | refer to Qt web site for sub-component licensing |
|   |   |   |   |

Qt is a trademark of The Qt Company with which this project has no affiliation.
