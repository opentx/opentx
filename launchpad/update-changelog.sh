#!/bin/sh
set -e
set -x

# $1 is changelog user and email, example "projectkk2glider <projectkk2glider@gmail.com>"
# $2 is distribution "trusty" OR "yakkety"
# $3 is version, example "2.2.1"
# $4 is suffix (optional, used for nightly builds, example "N360")

email=${1}
distro=${2%/}
version=${3}
suffix=${4}

echo "Updating changelog for ${distro}"

cd `dirname ${0}`

[ -z ${distro} ] && echo "Missing distribution name" && exit 1;
[ ! -z ${suffix} ] && version=${version}~${suffix}

if [ ! -d ${distro} ] ; then
  echo "${distro} changelog not found. Most likely unsupported distribution. Aborting..."
  exit 1
fi

echo "opentx-companion22 (${version}~${distro}) ${distro}; urgency=low" > tmp-changelog
echo "" >> tmp-changelog
echo "  * New release ${version}" >> tmp-changelog
echo "" >> tmp-changelog
echo " -- ${email}  "`date -R` >> tmp-changelog
echo "" >> tmp-changelog

cat tmp-changelog ${distro}/changelog > new-changelog
mv new-changelog ${distro}/changelog
rm tmp-changelog

echo "suffix=${suffix}" > common/suffix

echo "Result:"
cat ${distro}/changelog | head -n 20