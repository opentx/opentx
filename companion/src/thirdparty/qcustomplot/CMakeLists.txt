set(qcustomplot_SRCS
  qcustomplot.cpp
)

set(qcustomplot_HDRS
  qcustomplot.h
)

qt5_wrap_cpp(qcustomplot_SRCS ${qcustomplot_HDRS})

add_library(qcustomplot ${qcustomplot_SRCS})
qt5_use_modules(qcustomplot Widgets PrintSupport)
