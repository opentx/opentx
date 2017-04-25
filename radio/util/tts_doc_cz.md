The purpose of this document is to document the principle of TTS for CZ languages to non CZ speaker, with a strict focus on OpenTX TTS

## Large 'multiparts' numbers
This is for numbers requiring assembly of basic number blocks, like 110 in english : `ONE` `HUNDRED` `TEN`

## Plural units forms

4 forms of plural exist in otx cz tts, bellow is the exemple for volts

`(u"volt", u"volt0"), (u"volty", u"volt1"), (u"voltů", u"volt2"),(u"voltu", u"volt3")`

## Interger numbers with unit
X UNIT

If x = 1, form0 is used.
For x more than one but less than 5, form1 is used.
For x = 5 or more, form2 is used.

x = 1 "one" have 3 forms depending on the unit F/M/N
x = 2 "two" have 2 forms depending on the unit F/M/N

## decimal numbers with unit
X.Y UNIT
In this case unit is form3. X and Y is allways FEM because is not based on the unit but on the "tenth of unit" or "hundredth of unit" (both "tenth"/"hundredth" are FEM) 
"point" has three forms depending on X 1/2-4/5+ (u"celá", u"celé", u"celých")

Note : opentx audio only deals with PREC1 numbers, so Y can only be integer 1 to 9
