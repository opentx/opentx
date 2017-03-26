Use this two scripts to check eeprom upgrade on lua capale radio.
Using create.lua on source version will create a predefined set of input, mixer, switche, timer and logical swictch
Perform the eeprom upgrade
On the target version, run check.lua, the debug output will give something like :
OK 	source	1	1
OK 	switch	3	3
OK 	offset	10	10
OK 	weight	50	50
OK 	name	In1	In1
OK 	weight	50	50
OK 	speedDown	5	5
OK 	curveValue	50	50
OK 	speedUp	2	2
OK 	delayDown	5	5
OK 	carryTrim	true	true
OK 	mixWarn	1	1
OK 	flightModes	3	3
OK 	offset	10	10
OK 	switch	3	3
OK 	source	1	1
OK 	curveType	1	1
OK 	delayUp	5	5
OK 	name	Mix1	Mix1
OK 	multiplex	1	1
OK 	min	-800	-800
OK 	ppmCenter	10	10
OK 	symetrical	1	1
OK 	offset	10	10
OK 	revert	1	1
OK 	max	800	800
OK 	name	Out1	Out1
OK 	v1	2	2
OK 	delay	2	2
OK 	func	2	2
OK 	v3	2	2
OK 	and	2	2
OK 	v2	2	2
OK 	duration	3	3
OK 	minuteBeep	true	true
ISSUE 	value	43	10
OK 	persistent	1	1
OK 	start	0	0
OK 	countdownBeep	2	2
OK 	mode	4	4

The issue on 'value' for timer is normal and should be expected, but it should be the only one if eeprom upgrade was ok
