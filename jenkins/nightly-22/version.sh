#!/bin/bash

# make stamp header and extract OpenTX version (ie 2.1.2)
make -C opentx/radio/src stamp.h
rel=`grep "VERSION" opentx/radio/src/stamp.h | cut -d' ' -f3 | tr -d '"'`
release=${rel%$OPENTX_VERSION_SUFFIX}
echo ${release}

version=22
branch=next
winboxurl=http://winbox.open-tx.org/voice-builds

