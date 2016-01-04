#!/bin/bash

# stops on first error
set -e
set -x

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd ${DIR}
source ./version.sh

# release all components
./release-companion.sh
./release-firmware.sh

