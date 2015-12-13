#!/bin/bash

for f in `find ../src -regex '.*\.\(c\|cpp\|h\)$' -print`
do
  if [[ $f != *"thirdparty"* ]]
  then
    dos2unix $f $f
    uncrustify -c ./uncrustify.cfg --no-backup $f
    ./copyright.py ./copyright-header.txt $f
    ./include-guard.py $f
  fi
done

