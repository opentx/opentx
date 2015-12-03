#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# make sure we are in the good directory
cd ~opentx/release-$version/

# release all components
./release-companion.sh
./release-voices.sh
./release-lua.sh
./release-firmware.sh

