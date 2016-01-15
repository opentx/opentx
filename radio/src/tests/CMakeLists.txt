set(GTEST_INCDIR /usr/include/gtest)
set(GTEST_SRCDIR /usr/src/gtest)

if(EXISTS "${GTEST_SRCDIR}/src/gtest-all.cc")
  add_library(gtests-lib STATIC EXCLUDE_FROM_ALL ${GTEST_SRCDIR}/src/gtest-all.cc )
  target_include_directories(gtests-lib PUBLIC ${GTEST_INCDIR} PUBLIC ${GTEST_SRCDIR})

  find_package(Qt4 4.8.1 REQUIRED)
  include(${QT_USE_FILE})
  add_definitions(-DSIMU)
  set(TESTS_PATH ${RADIO_SRC_DIRECTORY})
  configure_file(${RADIO_SRC_DIRECTORY}/tests/location.h.in ${CMAKE_CURRENT_SOURCE_DIR}/location.h @ONLY)

  foreach(FILE ${SRC})
    set(RADIO_SRC ${RADIO_SRC} ../${FILE})
  endforeach()

  file(GLOB TEST_SRC_FILES ${RADIO_SRC_DIRECTORY}/tests/*.cpp)

  add_executable(gtests EXCLUDE_FROM_ALL ${TEST_SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/location.h ${RADIO_SRC} ../targets/simu/simpgmspace.cpp)
  add_dependencies(gtests ${FIRMWARE_DEPENDENCIES} gtests-lib)
  target_link_libraries(gtests gtests-lib ${QT_LIBRARIES} pthread)
  message("Added optional gtests target")
else()
  message("WARNING: can't find gtests source, gtest target will not be available!")
endif()