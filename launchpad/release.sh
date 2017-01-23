#!/bin/sh
set -e
set -x

# $1 is changelog user and email, example "projectkk2glider <projectkk2glider@gmail.com>"
# $2 is version, example "2.2.1"
# $3 is suffix (optional, used for nightly builds, example "N360")

email=${1}
version=${2}
suffix=${3}

cd `dirname ${0}`

for distro in trusty yakkety ; do
  echo ${distro}
  ./update-changelog.sh "${email}" ${distro} ${version} ${suffix}
  ./upload.sh ${distro}
done
