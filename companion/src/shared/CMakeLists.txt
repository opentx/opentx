include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(shared_SRCS
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
)

qt5_wrap_cpp(shared_SRCS ${shared_HDRS})

add_library(shared ${shared_SRCS})
target_link_libraries(shared PRIVATE Qt5::Widgets ${CPN_COMMON_LIB})
