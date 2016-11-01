project(OpenTX)

set(VERSION_MAJOR "2")
set(VERSION_MINOR "2")
set(VERSION_REVISION "0")
set(VERSION_SUFFIX $ENV{OPENTX_VERSION_SUFFIX})
set(VERSION_FAMILY ${VERSION_MAJOR}.${VERSION_MINOR})
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}${VERSION_SUFFIX})
message(STATUS "OpenTX ${VERSION}")

set(SDCARD_REVISION "0003")
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

set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)

set(RADIO_DIRECTORY ${PROJECT_SOURCE_DIR}/radio)
set(RADIO_SRC_DIRECTORY ${RADIO_DIRECTORY}/src)
set(COMPANION_SRC_DIRECTORY ${PROJECT_SOURCE_DIR}/companion/src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Qt5Widgets)
find_package(Qt5Xml)
find_package(Qt5LinguistTools)
find_package(Qt5PrintSupport)
find_package(Qt5Multimedia)

message(STATUS "Looking for SDL ")
find_package(Sdl)
if(SDL_FOUND)
  message("SDL Include Path: " ${SDL_INCLUDE_DIR})
else()
  message("SDL not found! Joysticks support will be disabled. Simulator audio will not work.")
endif()

message(STATUS "Looking for FOX ")
find_package(Fox)
if ((NOT FOX_FOUND) AND (NOT WIN32))
  message("Libfox not found, simu target will not be available")
endif()

# Check for a file that is typically left from a OpenTX 2.1 build and abort if found
if (EXISTS ${RADIO_SRC_DIRECTORY}/stamp.h OR EXISTS ${RADIO_SRC_DIRECTORY}/translations/en.h)
   message(FATAL_ERROR "Source directory contains files leftover from a OpenTX 2.1 build. Please run `git clean -f` in source directory (Careful: Will remove any extra files) or do a new clean git checkout")
endif()

include(Macros)
today(DATE)
now(TIME)
git_id(GIT_STR)

add_subdirectory(${RADIO_SRC_DIRECTORY})

if(Qt5Widgets_FOUND)
  add_subdirectory(${COMPANION_SRC_DIRECTORY})
endif()
