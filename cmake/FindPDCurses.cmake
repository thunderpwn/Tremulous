# Locate PDCurses library
# This module defines
# PDCURSES_LIBRARIES, the name of the library to link against
# PDCURSES_FOUND, if false, do not try to link to PDCurses
# PDCURSES_INCLUDE_DIR, where to find curses.h

FIND_PATH(PDCURSES_INCLUDE_DIR curses.h
  HINTS
  $ENV{PDCURSESDIR}
  ${LIB_DIR}/pdcurses
  ${LIB_DIR}/pdcurses/include
  PATH_SUFFIXES include/pdcurses include)

FIND_LIBRARY(PDCURSES_LIBRARY
  NAMES pdcurses
  HINTS
  $ENV{PDCURSESDIR}
  ${LIB_DIR}/pdcurses/libs
  ${LIB_DIR}/pdcurses/libs/lib64
  PATH_SUFFIXES lib64 lib)

SET(PDCURSES_FOUND "NO")
IF(PDCURSES_INCLUDE_DIR AND PDCURSES_LIBRARY)
  #message(STATUS "Found PDCurses library: ${PDCURSES_LIBRARIES}")
  # Set the final string here so the GUI reflects the final state.
  SET(PDCURSES_LIBRARIES PDCURSES_LIBRARY CACHE STRING "Where the PDCurses Library can be found")

  SET(PDCURSES_FOUND "YES")
ENDIF(PDCURSES_INCLUDE_DIR AND PDCURSES_LIBRARY)

IF(PDCURSES_LIBRARY)
  SET(PDCURSES_LIBRARIES ${PDCURSES_LIBRARY})
ENDIF(PDCURSES_LIBRARY)

MESSAGE("PDCURSES_INCLUDE_DIR is ${PDCURSES_INCLUDE_DIR}")
MESSAGE("PDCURSES_LIBRARY is ${PDCURSES_LIBRARY}")