set(ERSKY9X_CHECKOUT_DIRECTORY ${PROJECT_BINARY_DIR}/firmwares/ersky9x)
add_custom_command(
  OUTPUT ${ERSKY9X_CHECKOUT_DIRECTORY}/ersky9xsimulator.cpp
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ERSKY9X_CHECKOUT_DIRECTORY}
  COMMAND cd ${ERSKY9X_CHECKOUT_DIRECTORY} && svn checkout http://ersky9x.googlecode.com/svn/trunk/src@170 .
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/ersky9x/ersky9xsimulator.cpp ${ERSKY9X_CHECKOUT_DIRECTORY}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/ersky9x/ersky9xsimulator.cpp
)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

add_library(ersky9x-simulator ersky9xsimulator.cpp)
