set(GTEST_ROOT /usr CACHE STRING "Base path to Google Test headers and source.")

find_path(GTEST_INCDIR gtest/gtest.h HINTS "${GTEST_ROOT}/include" DOC "Path to Google Test header files folder ('gtest/gtest.h').")
find_path(GTEST_SRCDIR src/gtest-all.cc HINTS "${GTEST_ROOT}" "${GTEST_ROOT}/src/gtest" DOC "Path of Google Test 'src' folder.")

if(GTEST_INCDIR AND GTEST_SRCDIR AND Qt5Widgets_FOUND)
  add_library(gtests-radio-lib STATIC EXCLUDE_FROM_ALL ${GTEST_SRCDIR}/src/gtest-all.cc )
  target_include_directories(gtests-radio-lib PUBLIC ${GTEST_INCDIR} ${GTEST_INCDIR}/gtest ${GTEST_SRCDIR})
  add_definitions(-DSIMU)
  add_definitions(-DGTESTS)
  remove_definitions(-DCLI)
  set(TESTS_PATH ${RADIO_SRC_DIRECTORY}/tests)
  configure_file(${RADIO_SRC_DIRECTORY}/tests/location.h.in ${CMAKE_CURRENT_BINARY_DIR}/location.h @ONLY)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  if(PCB STREQUAL X12S OR PCB STREQUAL X10)
    add_custom_command(
      MAIN_DEPENDENCY ${TESTS_PATH}/model_22_x10.otx
      OUTPUT ${TESTS_PATH}/model_22_x10/RADIO/radio.bin ${TESTS_PATH}/model_22_x10/RADIO/models.txt ${TESTS_PATH}/model_22_x10/MODELS/model1.bin
      COMMAND mkdir -p model_22_x10 && cd model_22_x10 && unzip -o -DD ../model_22_x10.otx
      WORKING_DIRECTORY ${TESTS_PATH}
    )
    add_custom_target(x10-model-test-files
      DEPENDS ${TESTS_PATH}/model_22_x10/RADIO/radio.bin ${TESTS_PATH}/model_22_x10/RADIO/models.txt ${TESTS_PATH}/model_22_x10/MODELS/model1.bin
    )
  endif()
  
  if(WIN32)
    target_include_directories(gtests-radio-lib PUBLIC ${WIN_INCLUDE_DIRS})
    target_link_libraries(gtests-radio-lib PRIVATE ${WIN_LINK_LIBRARIES})
  endif(WIN32)

  if(SDL_FOUND AND SIMU_AUDIO)
    target_include_directories(gtests-radio-lib PUBLIC ${SDL_INCLUDE_DIR})
    target_link_libraries(gtests-radio-lib PRIVATE ${SDL_LIBRARY})
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
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ${WARNING_FLAGS}")

  use_cxx11()  # ensure gnu++11 in CXX_FLAGS with CMake < 3.1

  add_executable(gtests-radio EXCLUDE_FROM_ALL ${TEST_SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/location.h ${RADIO_SRC} ../targets/simu/simpgmspace.cpp ../targets/simu/simueeprom.cpp ../targets/simu/simufatfs.cpp)
  add_dependencies(gtests-radio ${FIRMWARE_DEPENDENCIES} gtests-radio-lib)
  if(PCB STREQUAL X12S OR PCB STREQUAL X10)
    add_dependencies(gtests-radio x10-model-test-files)
  endif()
  target_link_libraries(gtests-radio gtests-radio-lib pthread Qt5::Core Qt5::Widgets)
  message(STATUS "Added optional gtests target")
else()
  message(WARNING "WARNING: gtests target will not be available (check that GTEST_INCDIR, GTEST_SRCDIR, and Qt5Widgets are configured).")
endif()
