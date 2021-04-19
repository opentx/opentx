
set(datamodels_SRCS
  compounditemmodels.cpp
  filtereditemmodels.cpp
)

set(datamodels_HDRS
  compounditemmodels.h
  filtereditemmodels.h
)

qt5_wrap_cpp(datamodels_SRCS ${datamodels_HDRS})

add_library(datamodels ${datamodels_SRCS} ${datamodels_HDRS})
target_link_libraries(datamodels PRIVATE ${CPN_COMMON_LIB})

target_include_directories(datamodels
  PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${COMPANION_SRC_DIRECTORY}"
  PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
)
