# An Debian image for compiling OpenTX 2.2

FROM debian:stretch

RUN DEBIAN_FRONTEND=noninteractive apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential cmake gcc git lib32ncurses5 lib32z1 libfox-1.6-dev libsdl1.2-dev \
      qt5-default qtmultimedia5-dev qttools5-dev qttools5-dev-tools libqt5svg5-dev \
      software-properties-common wget zip python-pip python-pil libgtest-dev

RUN python -m pip install filelock

RUN wget -q https://launchpad.net/gcc-arm-embedded/4.7/4.7-2013-q3-update/+download/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 && \
    tar xjf gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 && \
    mv gcc-arm-none-eabi-4_7-2013q3 /opt/gcc-arm-none-eabi

RUN wget -q http://ftp.de.debian.org/debian/pool/main/g/gcc-avr/gcc-avr_4.7.2-2_amd64.deb && \
    wget -q http://ftp.de.debian.org/debian/pool/main/a/avr-libc/avr-libc_1.8.0-2_all.deb && \
    wget -q http://ftp.de.debian.org/debian/pool/main/b/binutils-avr/binutils-avr_2.24+Atmel3.4.4-1_amd64.deb && \
    wget -q http://ftp.de.debian.org/debian/pool/main/m/mpclib/libmpc2_0.9-4_amd64.deb && \
    dpkg -i gcc-avr_4.7.2-2_amd64.deb libmpc2_0.9-4_amd64.deb avr-libc_1.8.0-2_all.deb binutils-avr_2.24+Atmel3.4.4-1_amd64.deb

VOLUME ["/opentx"]

ENV PATH $PATH:/opt/gcc-arm-none-eabi/bin:/opentx/code/radio/util

ARG OPENTX_VERSION_SUFFIX=
ENV OPENTX_VERSION_SUFFIX ${OPENTX_VERSION_SUFFIX}
