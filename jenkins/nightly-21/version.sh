#!/bin/bash

# make stamp header and extract OpenTX version (ie 2.1.2)
make -C opentx/radio/src stamp_header
rel=`grep "VERS_STR" opentx/radio/src/stamp-opentx.h | cut -d' ' -f3 | tr -d '"'`
release=${rel%$OPENTX_VERSION_SUFFIX}
echo ${release}

version=21
branch=master
winboxurl=http://winbox.open-tx.org/voice-builds

