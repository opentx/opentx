#!/bin/bash

# stops on first error
set -e
set -x

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
source ./version.sh

# pull the latest changes
cd opentx
git fetch
git checkout $branch
git reset --hard origin/$branch
chmod -Rf g+w . || true

