set(qxtcommandoptions_SRCS
  qxtcommandoptions.cpp
)

set(qxtcommandoptions_HDRS
  qxtcommandoptions.h
)

qt4_wrap_cpp(qxtcommandoptions_SRCS ${qxtcommandoptions_HDRS})

add_library(qxtcommandoptions ${qxtcommandoptions_SRCS})
