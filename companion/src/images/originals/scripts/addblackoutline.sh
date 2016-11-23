#!/bin/bash  
for file in $(ls *png)
do
  convert $file \( +clone \
             -channel A -morphology EdgeOut Diamond +channel \
             +level-colors black \
           \) -compose DstOver -composite  $file
done


