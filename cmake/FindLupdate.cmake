# Locate lupdate binary from QT

# This module defines
# LUPDATE_EXECUTABLE, where is QT lupdate
# LUPDATE_FOUND, if false, don't try to use lupdate

FIND_PROGRAM( LUPDATE_EXECUTABLE
  NAMES
    lupdate lupdate-qt4
)

# if the program is found then we have it
IF( LUPDATE_EXECUTABLE )
  SET( LUPDATE_FOUND "YES" )
ENDIF( LUPDATE_EXECUTABLE )

MARK_AS_ADVANCED( LUPDATE_EXECUTABLE )
