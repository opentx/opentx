Add Gear Ratio to RPM Telemetry Calculation and allow for Blades to equal 1

Jan 16 2014
- Switched to 'next' branch
- Need to solve issue with frsky_sport.cpp and calc compile error
- Need to translate "Spur" and "Gear" to other languages, using English as placeholder for now.


Jan 15 2014
•Fully working in Simulator now, new options for Spur and Pinion are in Telemetry setup screen for the model. Pinion is forced to stay equal to or smaller than spur (to prevent divide by zero).
•Tested on Taranis Rev A (eeprom is modified so backup everything before experimenting).


Jan 14 2014
•The current implementation of OpenTX has a few limitations with regards to RPM sensors on Nitro Helicopters and this patch is to resolve and improve that.

•Initial changes to make:
◦Change Minimum Blades to 1 (currently hardcoded to 2) to allow for Fan RPM Sensors (single magnet) and crank sensors.
◦Change Maximum Blades to 255 (currently limited to 5) to allow for any number of divider for both nitro or electric.
◦Add Spur Gear USRDATA option to assist with calculating head speed
◦Add Pinion Gear USRDATA option to assist with calculating head speed
◦Change RPM telemetry calculation to take into account gear ratio based upon Spur and Pinion size


•Future:
◦Add new telemtry calculated value "HDSP" for Headspeed to allow user to see engine RPM and calculated headspeed
◦Add more options for electric setups


Author - Tim Foley
