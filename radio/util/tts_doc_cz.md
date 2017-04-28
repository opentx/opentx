The purpose of this document is to document the principle of TTS for CZ languages to non CZ speaker, with a strict focus on OpenTX TTS

## Large 'multiparts' numbers
This is for numbers requiring assembly of basic number blocks, like 110 in english : `ONE` `HUNDRED` `TEN`

## Plural units forms

4 forms of plural exist in otx cz tts, bellow is the exemple for volts

`(u"volt", u"volt0"), (u"volty", u"volt1"), (u"voltů", u"volt2"),(u"voltu", u"volt3")`

In this document, we will use unit0 to unit3 to describe those 4 variations

## Interger numbers with unit

The number takes a from like X UNIT

#### "one ..."
The number "one" has 3 gender depending on the unit F/M/N
The forms are "jedna","jeden", "jedno"

See unit table bellow to find out wich gender of one should be used

unit0 is used with the 3 gender of one.

#### "two ..."
The number "two" has 3 gender depending on the unit
The forms are  "dva","dvě"

See unit table bellow to find out wich gender of two should be used

unit1 is used with the 2 gender of two.

### "tree to five"

unit1 is used

### "five and more"

unit2 is used


## decimal numbers with unit
X.Y UNIT
In this case unit is unit3. X and Y is allways FEM.

"point" has three forms depending on the value of X :  
0 and 1 : "celá"  
2, 3 , 4 : "celé"  
5 :"celých"  

Note : opentx audio only deals with PREC1 numbers, so Y can only be integer 1 to 9

# Unit table

| Index  | Unit            | Defined as             | One     | Two   | Gender
| --- | ---                |---                     |---      |---    |---
| 0   | Raw unit (no unit) | UNIT_RAW               | jedna   | dvě   | Female
| 1   | Volts              | UNIT_VOLTS             | jeden   | dva   | Male
| 2   | Amps               | UNIT_AMPS              | jeden   | dva   | Male
| 3   | Milliamps          | UNIT_MILLIAMPS         | jeden   | dva   | Male
| 4   | Knots              | UNIT_KTS               | jeden   | dva   | Male
| 5   | Meters per Second  | UNIT_METERS_PER_SECOND | jeden   | dva   | Male
| 6   | Feet per Second    | UNIT_FEET_PER_SECOND   | jedna   | dvě   | Female
| 7   | Kilometers per Hour| UNIT_KMH               | jeden   | dva   | Male
| 8   | Miles per Hour     | UNIT_MPH               | jedna   | dvě   | Female
| 9   | Meters             | UNIT_METERS            | jeden   | dva   | Male
| 10  | Feet               | UNIT_FEET              | jedna   | dvě   | Female
| 11  | Degrees Celsius    | UNIT_CELSIUS           | jeden   | dva   | Male
| 12  | Degrees Fahrenheit | UNIT_FAHRENHEIT        | jeden   | dva   | Male
| 13  | Percent            | UNIT_PERCENT           | jedno   | dvě   | Neutral
| 14  | Milliamp per Hour  | UNIT_MAH               | jedna   | dvě   | Female
| 15  | Watts              | UNIT_WATTS             | jeden   | dva   | Male
| 16  | Milliwatts         | UNIT_MILLIWATTS        | jeden   | dva   | Male
| 17  | dB                 | UNIT_DB                | jeden   | dva   | Male
| 18  | RPM                | UNIT_RPMS              | jedna   | dvě   | Female
| 19  | G                  | UNIT_G                 | jedno   | dvě   | Neutral
| 20  | Degrees            | UNIT_DEGREE            | jeden   | dva   | Male
| 21  | Radians            | UNIT_RADIANS           | jeden   | dva   | Male
| 22  | Milliliters        | UNIT_MILLILITERS       | jeden   | dva   | Male
| 23  | Fluid Ounces       | UNIT_FLOZ              | jedna   | dvě   | Female
| 24  | Hours              | UNIT_HOURS             | jedna   | dvě   | Female
| 25  | Minutes            | UNIT_MINUTES           | jedna   | dvě   | Female
| 26  | Seconds            | UNIT_SECONDS           | jedna   | dvě   | Female
