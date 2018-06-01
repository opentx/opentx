set(GTEST_ROOT /usr CACHE string "Base path to Google Test headers and source.")

find_path(GTEST_INCDIR gtest/gtest.h HINTS "${GTEST_ROOT}/include" DOC "Path to Google Test header files folder ('gtest/gtest.h').")
find_path(GTEST_SRCDIR src/gtest-all.cc HINTS "${GTEST_ROOT}" "${GTEST_ROOT}/src/gtest" DOC "Path of Google Test 'src' folder.")

if(GTEST_INCDIR AND GTEST_SRCDIR AND Qt5Widgets_FOUND)
  add_library(gtests-lib STATIC EXCLUDE_FROM_ALL ${GTEST_SRCDIR}/src/gtest-all.cc )
  target_include_directories(gtests-lib PUBLIC ${GTEST_INCDIR} ${GTEST_INCDIR}/gtest ${GTEST_SRCDIR})
  add_definitions(-DSIMU)
  add_definitions(-DGTESTS)
  set(TESTS_PATH ${RADIO_SRC_DIRECTORY})
  configure_file(${RADIO_SRC_DIRECTORY}/tests/location.h.in ${CMAKE_CURRENT_BINARY_DIR}/location.h @ONLY)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  if(WIN32)
    target_include_directories(gtests-lib PUBLIC ${WIN_INCLUDE_DIRS})
    target_link_libraries(gtests-lib PRIVATE ${WIN_LINK_LIBRARIES})
  endif(WIN32)

  if(SDL_FOUND AND SIMU_AUDIO)
    target_include_directories(gtests-lib PUBLIC ${SDL_INCLUDE_DIR})
    target_link_libraries(gtests-lib PRIVATE ${SDL_LIBRARY})
  endif()

  foreach(FILE ${SRC})
    set(RADIO_SRC ${RADIO_SRC} ../${FILE})
  endforeach()

  file(GLOB TEST_SRC_FILES ${RADIO_SRC_DIRECTORY}/tests/*.cpp)

  if(MINGW)
    # struct packing breaks on MinGW w/out -mno-ms-bitfields: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991 & http://stackoverflow.com/questions/24015852/struct-packing-and-alignment-with-mingw
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-ms-bitfields")
  endif()
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

  use_cxx11()  # ensure gnu++11 in CXX_FLAGS with CMake < 3.1

  add_executable(gtests EXCLUDE_FROM_ALL ${TEST_SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/location.h ${RADIO_SRC} ../targets/simu/simpgmspace.cpp ../targets/simu/simueeprom.cpp ../targets/simu/simufatfs.cpp)
  add_dependencies(gtests ${FIRMWARE_DEPENDENCIES} gtests-lib)
  target_link_libraries(gtests gtests-lib pthread Qt5::Core Qt5::Widgets)
  message(STATUS "Added optional gtests target")
else()
  message(WARNING "WARNING: gtests target will not be available (check that GTEST_INCDIR, GTEST_SRCDIR, and Qt5Widgets are configured).")
endif()
