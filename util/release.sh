#!/usr/bin/env sh

# stock board

make clean
make PCB=STD BATT=UNSTABLE_BANDGAP BEEPER=BUZZER
cp ./open9x.hex ../open9x-stock.hex 

make clean
make PCB=STD HELI=YES TEMPLATES=YES BATT=UNSTABLE_BANDGAP BEEPER=BUZZER
cp ./open9x.hex ../open9x-stock-heli-templates.hex 

make clean
make PCB=STD HELI=YES TEMPLATES=YES BATT=UNSTABLE_BANDGAP BEEPER=SPEAKER
cp ./open9x.hex ../open9x-stock-heli-templates-speaker.hex 

make clean
make PCB=STD EXT=FRSKY BATT=UNSTABLE_BANDGAP BEEPER=BUZZER
cp ./open9x.hex ../open9x-stock-frsky.hex

make clean
make PCB=STD EXT=FRSKY BATT=UNSTABLE_BANDGAP BEEPER=SPEAKER
cp ./open9x.hex ../open9x-stock-frsky-speaker.hex

make clean
make PCB=STD EXT=FRSKY HELI=YES TEMPLATES=YES BATT=UNSTABLE_BANDGAP BEEPER=BUZZER
cp ./open9x.hex ../open9x-stock-frsky-heli-templates.hex

make clean
make PCB=STD EXT=FRSKY HELI=YES TEMPLATES=YES BATT=UNSTABLE_BANDGAP BEEPER=SPEAKER
cp ./open9x.hex ../open9x-stock-frsky-heli-templates-speaker.hex

# v4 board

make clean
make PCB=V4
cp ./open9x.hex ../open9x-v4-frsky.hex

make clean
make PCB=V4 HELI=YES TEMPLATES=YES
cp ./open9x.hex ../open9x-v4-frsky-heli-templates.hex

# stamp

make stamp

