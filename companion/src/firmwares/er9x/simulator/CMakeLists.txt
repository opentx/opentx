set(ER9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/er9x)
add_custom_command(
  OUTPUT ${ER9X_CHECKOUT_DIRECTORY}/er9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ER9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${ER9X_CHECKOUT_DIRECTORY} && svn checkout http://er9x.googlecode.com/svn/trunk/src@790 .
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/er9x/er9xsimulator.cpp ${ER9X_CHECKOUT_DIRECTORY}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/er9x/er9xsimulator.cpp
)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

add_library(er9x-simulator er9xsimulator.cpp)
