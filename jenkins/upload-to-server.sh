#!/bin/bash

set -e
set -x

# in this file define ssh options (like private key)
source ~/.opentx-jenkins-settings

function upload() {
  # $1 is release directory name
  # copy scripts to server
  scp ${SCP_OPT} ${1}/*.sh ${USERNAME}@jenkins.open-tx.org:/home/opentx/${1}/
}

upload release-20
upload nightly-21
upload release-21
upload nightly-22
