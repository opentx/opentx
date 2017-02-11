project(OpenTX)

set(VERSION_MAJOR "2")
set(VERSION_MINOR "2")
set(VERSION_REVISION "0")
set(VERSION_SUFFIX $ENV{OPENTX_VERSION_SUFFIX})
set(VERSION_FAMILY ${VERSION_MAJOR}.${VERSION_MINOR})
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}${VERSION_SUFFIX})
set(SDCARD_REVISION "0005")
set(SDCARD_VERSION  ${VERSION_MAJOR}.${VERSION_MINOR}V${SDCARD_REVISION})

cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0023 OLD)
if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)

set(RADIO_DIRECTORY ${PROJECT_SOURCE_DIR}/radio)
set(RADIO_SRC_DIRECTORY ${RADIO_DIRECTORY}/src)
set(COMPANION_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/companion/src)
set(SIMU_SRC_DIRECTORY ${COMPANION_SRC_DIRECTORY}/simulation)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(Macros)
today(DATE)
now(TIME)
git_id(GIT_STR)

message(STATUS "OpenTX ${VERSION} @${GIT_STR}")

# options shared by all targets
option(VERBOSE_CMAKELISTS "Show extra information while processing CMakeLists.txt files." OFF)
option(WARNINGS_AS_ERRORS "Treat any compiler warning as an error (adds -Werror flag)." OFF)
if(WIN32)
  set(WIN_EXTRA_LIBS_PATH "C:/Programs" CACHE PATH
    "Base path to extra libs/headers on Windows (SDL & pthreads folders should be in here).")
  list(APPEND CMAKE_PREFIX_PATH "${WIN_EXTRA_LIBS_PATH}" "${WIN_EXTRA_LIBS_PATH}/SDL")  # hints for FindSDL
endif()

find_package(Qt5Widgets)
find_package(Qt5Xml)
find_package(Qt5LinguistTools)
find_package(Qt5PrintSupport)
find_package(Qt5Multimedia)
find_package(Qt5Svg)

if(Qt5Widgets_FOUND)
  message(STATUS "Qt Version: ${Qt5Widgets_VERSION}")
else()
  message(WARNING "Qt not found! Companion and Simulator builds disabled.")
endif()

if(NOT WIN32)
  find_package(Fox QUIET)  # QUIET not working on WIN32?
  if (FOX_FOUND)
    message(STATUS "Foxlib found at ${FOX_LIBRARY}")
  else() # if(NOT WIN32)
    message("Libfox not found, simu target will not be available")
  endif()
endif()

if(Qt5Widgets_FOUND OR FOX_FOUND)
  set(SDL_BUILDING_LIBRARY YES)  # this prevents FindSDL from appending SDLmain lib to the results, which we don't want
  find_package("SDL")
  if(SDL_FOUND)
    message(STATUS "SDL Lib: ${SDL_LIBRARY}; Headers: ${SDL_INCLUDE_DIR}; Version: ${SDL_VERSION_STRING}")
  else()
    message(STATUS "SDL not found! Simulator audio, and joystick inputs, will not work.")
  endif()
endif()

# Check for a file that is typically left from a OpenTX 2.1 build and abort if found
if (EXISTS ${RADIO_SRC_DIRECTORY}/stamp.h OR EXISTS ${RADIO_SRC_DIRECTORY}/translations/en.h)
   message(FATAL_ERROR "Source directory contains files leftover from a OpenTX 2.1 build. Please run `git clean -f` in source directory (Careful: Will remove any extra files) or do a new clean git checkout")
endif()

# Windows-specific includes and libs shared by sub-projects
if(WIN32)
  list(APPEND WIN_INCLUDE_DIRS "${RADIO_SRC_DIRECTORY}/thirdparty/windows/dirent")
  if(MSVC)
    list(APPEND WIN_INCLUDE_DIRS "${RADIO_SRC_DIRECTORY}/thirdparty/windows/msinttypes")
    set(WIN_PTHREAD_BASE_PATH "${WIN_EXTRA_LIBS_PATH}/pthreads/Pre-built.2" CACHE PATH "Where to find pthreads include and lib folders.")
    list(APPEND WIN_INCLUDE_DIRS "${WIN_PTHREAD_BASE_PATH}/include")
    find_file(WIN_PTHREAD_LIB pthreadVC2.lib PATHS "${WIN_PTHREAD_BASE_PATH}" PATH_SUFFIXES lib/x86 lib NO_DEFAULT_PATH)
    find_file(WIN_PTHREAD_DLL pthreadVC2.dll PATHS "${WIN_PTHREAD_BASE_PATH}" PATH_SUFFIXES lib/x86 lib dll/x86 dll NO_DEFAULT_PATH)
    if(WIN_PTHREAD_LIB)
      list(APPEND WIN_LINK_LIBRARIES "${WIN_PTHREAD_LIB}")
    else()
      message(SEND_ERROR "pthreadVC2.lib not found!")
    endif()
  else()
    set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
    set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)
    set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
    set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)
  endif()
endif()

add_subdirectory(${RADIO_SRC_DIRECTORY})

if(Qt5Widgets_FOUND)
  add_subdirectory(${COMPANION_SRC_DIRECTORY})
endif()
