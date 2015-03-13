# Locate Xsd from code synthesis include paths and binary
# Xsd can be found at http://codesynthesis.com/products/xsd/
# Written by Frederic Heem, frederic.heem _at_ telsey.it

# This module defines
# XSD_INCLUDE_DIR, where to find elements.hxx, etc.
# XSD_EXECUTABLE, where is the xsd compiler
# XSD_FOUND, If false, don't try to use xsd

FIND_PATH( XSD_INCLUDE_DIR xsd/cxx/parser/elements.hxx
  "/opt/local/xsd-3.3.0-i686-macosx/libxsd"
  "C:/Programs/xsd-3.3.0/libxsd"
  "C:/Program Files/CodeSynthesis XSD 3.2/include"
  "C:/mingw/xsd-3.3.0-i686-windows/libxsd"
  $ENV{XSDDIR}/include
  $ENV{CODESYNTH}/include
  /usr/local/include /usr/include
  $ENV{XSDDIR}/libxsd
)

IF( WIN32 )
  SET( XSDCXX_FILENAME1 xsd-cxx.exe )
ELSE( )
  SET( XSDCXX_FILENAME1 xsdcxx )
  SET( XSDCXX_FILENAME2 xsd )
ENDIF( )

FIND_PROGRAM( XSDCXX_EXECUTABLE
  NAMES
    ${XSDCXX_FILENAME1} ${XSDCXX_FILENAME2}
  PATHS
    "/opt/local/xsd-3.3.0-i686-macosx/bin"
    "C:/Programs/xsd-3.3.0/bin"
    "C:/mingw/xsd-3.3.0-i686-windows/bin"
    "C:/Program Files/CodeSynthesis XSD 3.2/bin"
    $ENV{XSDDIR}/bin
    /usr/local/bin
    /usr/bin
    $ENV{XSDDIR}/xsd
)

MESSAGE(STATUS ${XSDCXX_EXECUTABLE})

# if the include and the program are found then we have it
IF( XSD_INCLUDE_DIR )
  IF( XSDCXX_EXECUTABLE )
    SET( XSD_FOUND "YES" )
  ENDIF( XSDCXX_EXECUTABLE )
ENDIF( XSD_INCLUDE_DIR )

MARK_AS_ADVANCED(
  XSD_INCLUDE_DIR
  XSDCXX_EXECUTABLE
)
