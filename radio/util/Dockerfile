# A Debian image for compiling OpenTX 2.3

FROM debian:buster

RUN DEBIAN_FRONTEND=noninteractive apt-get -y update
RUN DEBIAN_FRONTEND=noninteractive apt-get -y install build-essential cmake gcc git lib32ncurses6 lib32z1 libfox-1.6-dev libsdl1.2-dev \
      qt5-default qtmultimedia5-dev qttools5-dev qttools5-dev-tools libqt5svg5-dev \
      software-properties-common wget zip python3 python-pip python3-pip python-pil python3-pil libgtest-dev libclang-dev clang python-clang

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 99 && python -m pip install filelock clang

RUN wget -q https://launchpad.net/gcc-arm-embedded/4.7/4.7-2013-q3-update/+download/gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 && \
    tar xjf gcc-arm-none-eabi-4_7-2013q3-20130916-linux.tar.bz2 && \
    mv gcc-arm-none-eabi-4_7-2013q3 /opt/gcc-arm-none-eabi

VOLUME ["/opentx"]

ENV PATH $PATH:/opt/gcc-arm-none-eabi/bin:/opentx/code/radio/util

ARG OPENTX_VERSION_SUFFIX=
ENV OPENTX_VERSION_SUFFIX ${OPENTX_VERSION_SUFFIX}

WORKDIR /build
