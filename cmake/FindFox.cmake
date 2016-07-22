# Imported from the DeskVox project (LPGL 2.1)

include(FindPackageHandleStandardArgs)

set(hints
  $ENV{LIB_BASE_PATH}/fox
)

set(paths
  /usr
  /usr/local
)

find_path(FOX_INCLUDE_DIR
  NAMES
    fx.h
  HINTS
    ${hints}
  PATHS
    ${paths}
  PATH_SUFFIXES
    include
    include/fox
    include/fox-1.6
)

find_library(FOX_LIBRARY
  NAMES
    FOX-1.6
    fox-1.6
  HINTS
    ${hints}
  PATHS
    ${paths}
  PATH_SUFFIXES
    lib64
    lib
)

find_library(FOX_LIBRARY_DEBUG
  NAMES
    FOXD-1.6
    foxd-1.6
  HINTS
    ${hints}
  PATHS
    ${paths}
  PATH_SUFFIXES
    lib64
    lib
)

if(FOX_LIBRARY_DEBUG)
  set(FOX_LIBRARIES optimized ${FOX_LIBRARY} debug ${FOX_LIBRARY_DEBUG})
else()
  set(FOX_LIBRARIES ${FOX_LIBRARY})
endif()

find_package_handle_standard_args(FOX
  DEFAULT_MSG
  FOX_INCLUDE_DIR
  FOX_LIBRARY
)