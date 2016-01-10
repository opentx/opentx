find_package(Qt4 4.8.1 REQUIRED)
include(${QT_USE_FILE})

set(GTEST_INCDIR /usr/include/gtest)
set(GTEST_SRCDIR /usr/src/gtest)

add_library(gtests-lib STATIC EXCLUDE_FROM_ALL ${GTEST_SRCDIR}/src/gtest-all.cc )
target_include_directories(gtests-lib PUBLIC ${GTEST_INCDIR} PUBLIC ${GTEST_SRCDIR})


add_definitions(-DSIMU)

set(TESTS_PATH ${RADIO_SRC_DIRECTORY})
configure_file(${RADIO_SRC_DIRECTORY}/tests/location.h.in ${RADIO_SRC_DIRECTORY}/tests/location.h @ONLY)

foreach(FILE ${SRC})
  set(RADIO_SRC ${RADIO_SRC} ../${FILE})
endforeach()

file(GLOB TEST_SRC_FILES ${RADIO_SRC_DIRECTORY}/tests/*.cpp)

add_executable(gtests EXCLUDE_FROM_ALL ${TEST_SRC_FILES} ${RADIO_SRC} ../targets/simu/simpgmspace.cpp)
add_dependencies(gtests ${FIRMWARE_DEPENDENCIES} gtests-lib)
target_link_libraries(gtests gtests-lib ${QT_LIBRARIES} pthread)
