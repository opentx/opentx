set(qcustomplot_SRCS
  qcustomplot.cpp
)

set(qcustomplot_HDRS
  qcustomplot.h
)

qt4_wrap_cpp(qcustomplot_SRCS ${qcustomplot_HDRS})

add_library(qcustomplot ${qcustomplot_SRCS})
