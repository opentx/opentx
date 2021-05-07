include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(shared_NAMES
  autocheckbox
)
set(shared_SRCS
  genericpanel.cpp
  hexspinbox.cpp
  verticalscrollarea.cpp
)

set(shared_HDRS
  autobitsetcheckbox.h
  autocombobox.h
  autodoublespinbox.h
  autohexspinbox.h
  autolineedit.h
  genericpanel.h
  hexspinbox.h
  autoprecisioncombobox.h
  autotimeedit.h
)

foreach(name ${shared_NAMES})
  set(shared_SRCS ${shared_SRCS} ${name}.cpp)
  set(shared_HDRS ${shared_HDRS} ${name}.h)
endforeach()

qt5_wrap_cpp(shared_SRCS ${shared_HDRS})

# add after moc generation to stop warnings
set(shared_SRCS ${shared_SRCS}
  autowidget.h
  autowidget.cpp
)

add_library(shared ${shared_SRCS})
target_link_libraries(shared PRIVATE Qt5::Widgets ${CPN_COMMON_LIB})
