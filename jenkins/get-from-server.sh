#!/bin/bash

set -e
set -x

# in this file define ssh options (like private key)
source ./settings.sh

function download() {
  # $1 is release directory name
  mkdir -p ${1}
  scp ${SCP_OPT} ${USERNAME}@jenkins.open-tx.org:/home/opentx/${1}/*.sh ${1}/
  #chmod 774 ${1}/*.sh
}

download release-20
download nightly-21
download release-21
