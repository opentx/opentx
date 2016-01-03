#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release-20/

# pull the latest changes
cd opentx
git fetch
git checkout 2.0
git reset --hard origin/2.0
chmod -Rf g+w . || true

