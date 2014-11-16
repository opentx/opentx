include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(shared_SRCS
  foldersync.cpp
  genericpanel.cpp
  hexspinbox.cpp
  verticalscrollarea.cpp
)

set(shared_HDRS
  autocheckbox.h
  autocombobox.h
  autodoublespinbox.h
  autohexspinbox.h
  autolineedit.h
  genericpanel.h
  hexspinbox.h
  verticalscrollarea.h
)
 
qt4_wrap_cpp(shared_SRCS ${shared_HDRS})

add_library(shared ${shared_SRCS})