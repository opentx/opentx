set(TH9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/th9x)
add_custom_command(
  OUTPUT ${TH9X_CHECKOUT_DIRECTORY}/th9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${TH9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${TH9X_CHECKOUT_DIRECTORY} && svn checkout http://th9x.googlecode.com/svn/trunk/src@285 .
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/th9x/th9xsimulator.cpp ${TH9X_CHECKOUT_DIRECTORY}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/th9x/th9xsimulator.cpp
)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

add_library(th9x-simulator th9xsimulator.cpp)
