The purpose of this document is to document the principle of TTS for CZ languages to non CZ speaker, with a strict focus on OpenTX TTS

Plural units forms
4 forms of plural exist in otx cz tts, bellow is the exemple for volts
(u"volt", u"volt0"), (u"volty", u"volt1"), (u"voltů", u"volt2"),(u"voltu", u"volt3")

## Interger numbers with unit
X UNIT

If x = 1, form0 is used.
For x more than one but less than 5, form1 is used.
For x = 5 or more, form2 is used.


## decimal numbers with unit
X.Y UNIT

Note : opentx audio only deals with PREC1 numbers, so Y can only be integer 1 to 9
