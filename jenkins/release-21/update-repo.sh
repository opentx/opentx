#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# make sure we are in the good directory
cd ~opentx/release-$version/

# pull the latest changes
cd opentx
git fetch
git checkout $branch
git reset --hard origin/$branch
chmod -Rf g+w . || true

