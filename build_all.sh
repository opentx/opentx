#!/bin/bash
set -o nounset  # exit if trying to use an uninitialized var
#set -o errexit  # exit if any program fails
set -o pipefail # exit if any program in a pipeline fails, also
set -x          # debug mode

TARGETS=(
  9X
  GRUVIN9X
  HORUS
  I6
  MEGA2560
  SKY9X
  TARANIS
)
for target in "${TARGETS[@]}"
do
  echo building $target
  mkdir build-${target}
  pushd build-${target}
  cmake -DPCB=${target} -DGVARS=YES -DLUA=YES -DDEBUG=YES -DCMAKE_BUILD_TYPE=Debug ../
  make
  popd
done
