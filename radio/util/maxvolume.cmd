echo off

 
echo Getting max amplitude..
sox %1.wav -n stat -v 2> %1_apl.txt
Set /P apl_value= < %1_apl.txt
 
echo Normalizing audio.. gain: %apl_value%
sox --show-progress -v %apl_value% %1.wav %1_norm.wav

copy %1_norm.wav %1.wav
 
echo Cleaning..
del %1_norm.wav
del %1_apl.txt

