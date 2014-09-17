include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(generaledit_NAMES
  generaledit
  setup
  trainer
  calibration
)

set(generaledit_SRCS
)

set(generaledit_HDRS
)

set(generaledit_UIS
)

foreach(name ${generaledit_NAMES})
  set(generaledit_SRCS ${generaledit_SRCS} ${name}.cpp)
  set(generaledit_HDRS ${generaledit_HDRS} ${name}.h)
  set(generaledit_UIS  ${generaledit_UIS}  ${name}.ui)
endforeach()
 
qt4_wrap_ui(generaledit_SRCS ${generaledit_UIS})
qt4_wrap_cpp(generaledit_SRCS ${generaledit_HDRS})

add_library(generaledit ${generaledit_SRCS})