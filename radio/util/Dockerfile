# An Debian image for compiling OpenTX 2.2

FROM        debian:jessie
MAINTAINER  bsongis

RUN         apt-get update &&
            apt-get install -y software-properties-common wget lib32z1 lib32ncurses5 git cmake gcc avr-libc build-essential libfox-1.6-dev python-qt4
RUN         wget -q https://launchpad.net/gcc-arm-embedded/4.7/4.7-2013-q3-update/+download/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 &&
            tar xjf gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 &&
            mv gcc-arm-none-eabi-4_7-2013q3 /opt/gcc-arm-none-eabi
ENV         PATH $PATH:/opt/gcc-arm-none-eabi/bin

ADD         http://ftp.de.debian.org/debian/pool/main/g/gcc-avr/gcc-avr_4.7.2-2_amd64.deb gcc-avr_4.7.2-2_amd64.deb
ADD         http://ftp.de.debian.org/debian/pool/main/m/mpclib/libmpc2_0.9-4_amd64.deb libmpc2_0.9-4_amd64.deb
RUN         dpkg -i gcc-avr_4.7.2-2_amd64.deb libmpc2_0.9-4_amd64.deb

VOLUME      ["/opentx"]
ENTRYPOINT  ["python", "/opentx/code/radio/util/build.py"]
