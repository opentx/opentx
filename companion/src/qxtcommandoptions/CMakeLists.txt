set(qxtcommandoptions_SRCS
  qxtcommandoptions.cpp
)

qt5_wrap_cpp(qxtcommandoptions_SRCS)

add_library(qxtcommandoptions ${qxtcommandoptions_SRCS})
qt5_use_modules(qxtcommandoptions Widgets)

