set(qxtcommandoptions_SRCS
  qxtcommandoptions.cpp
)

set(qxtcommandoptions_HDRS
  qxtcommandoptions.h
)

qt5_wrap_cpp(qxtcommandoptions_SRCS ${qxtcommandoptions_HDRS})

add_library(qxtcommandoptions ${qxtcommandoptions_SRCS})
qt5_use_modules(qxtcommandoptions Widgets)

