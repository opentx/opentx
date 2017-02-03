#.rst:
# FindSDL
# -------
#
# Locate SDL library
#
# This module defines
#
# ::
#
#   SDL_FOUND, if false, do not try to link to SDL
#   SDL_LIBRARY, the name(s) of the library(ies) to link against
#   SDL_LIBSDL, the full path and name of libSDL (or SDL.[lib|dll].
#   SDL_LIBSDLMAIN, full path and name of libSDLmain (or SDLmain.lib).
#   SDL_LIBRARY_PATH, the full path to the found libSDL
#   SDL_INCLUDE_DIR, where to find SDL.h
#   SDL_VERSION_STRING, human-readable string containing the version of SDL
#
#
#
# This module responds to the flag:
#
# ::
#
#   SDL_BUILDING_LIBRARY
#     If this is defined, then no SDL_main will be linked in because
#     only applications need main().
#     Otherwise, it is assumed you are building an application and this
#     module will attempt to locate and set the proper link flags
#     as part of the returned SDL_LIBRARY variable.
#
#
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the
# OS X framework based version.  (Other versions link to -lSDLmain which
# this module will try to find on your behalf.) Also for OS X, this
# module will automatically add the -framework Cocoa on your behalf.
#
#
#
# Additional Note: If you see an empty SDL_LIBSDL in your
# configuration and no SDL_LIBRARY, it means CMake did not find your SDL
# library (SDL.dll, libsdl.so, SDL.framework, etc).  Set
# SDL_LIBSDL to point to your SDL library, and configure again.
# Similarly, if you see an empty SDL_LIBSDLMAIN, you should set this
# value as appropriate.  These values are used to generate the final
# SDL_LIBRARY variable, but when these values are unset, SDL_LIBRARY
# does not get created.
#
#
#
# $SDLDIR is an environment variable that would correspond to the
# ./configure --prefix=$SDLDIR used in building SDL.  l.e.galup 9-20-02
#
# Modified by Eric Wing.  Added code to assist with automated building
# by using environmental variables and providing a more
# controlled/consistent search behavior.  Added new modifications to
# recognize OS X frameworks and additional Unix paths (FreeBSD, etc).
# Also corrected the header search path to follow "proper" SDL
# guidelines.  Added a search for SDLmain which is needed by some
# platforms.  Added a search for threads which is needed by some
# platforms.  Added needed compile switches for MinGW.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of SDL_LIBRARY to
# override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from SDL/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention is #include
# "SDL.h", not <SDL/SDL.h>.  This is done for portability reasons
# because not all systems place things in SDL/ (see FreeBSD).

#
# Modified for OpenTx by Maxim Paperno 19-Dec-2016
# Added SDL_LIBSDL, SDL_LIBSDLMAIN & SDL_LIBRARY_PATH, removed SDLMAIN_LIBRARY,
#  SDL_LIBRARY_TEMP, and fixed bug with recursive appending of supporting 
#  libraries to cached SDL_LIBRARY_TEMP variable.
#

#=============================================================================
#CMake - Cross Platform Makefile Generator
#Copyright 2000-2015 Kitware, Inc.
#Copyright 2000-2011 Insight Software Consortium
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions
#are met:
#
#* Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#
#* Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.
#
#* Neither the names of Kitware, Inc., the Insight Software Consortium,
#  nor the names of their contributors may be used to endorse or promote
#  products derived from this software without specific prior written
#  permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#------------------------------------------------------------------------------
#
#The above copyright and license notice applies to distributions of
#CMake in source and binary form.  Some source files contain additional
#notices of original copyright by their contributors; see each source
#for details.  Third-party software packages supplied with CMake under
#compatible licenses provide their own copyright notices documented in
#corresponding subdirectories.
#
#------------------------------------------------------------------------------
#
#CMake was initially developed by Kitware with the following sponsorship:
#
# * National Library of Medicine at the National Institutes of Health
#   as part of the Insight Segmentation and Registration Toolkit (ITK).
#
# * US National Labs (Los Alamos, Livermore, Sandia) ASC Parallel
#   Visualization Initiative.
#
# * National Alliance for Medical Image Computing (NAMIC) is funded by the
#   National Institutes of Health through the NIH Roadmap for Medical Research,
#   Grant U54 EB005149.
#
# * Kitware, Inc.
#=============================================================================


find_path(SDL_INCLUDE_DIR SDL.h
  HINTS
    ENV SDLDIR
  PATH_SUFFIXES SDL SDL12 SDL11
                # path suffixes to search inside ENV{SDLDIR}
                include/SDL include/SDL12 include/SDL11 include
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

# SDL-1.1 is the name used by FreeBSD ports...
# don't confuse it for the version number.
find_library(SDL_LIBSDL
  NAMES SDL SDL-1.1
  HINTS
    ENV SDLDIR
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
)

# Non-OS X framework versions expect you to also dynamically link to
# SDLmain. This is mainly for Windows and OS X. Other (Unix) platforms
# seem to provide SDLmain for compatibility even though they don't
# necessarily need it.
find_library(SDL_LIBSDLMAIN
  NAMES SDLmain SDLmain-1.1
  HINTS
    ENV SDLDIR
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
  PATHS
  /sw
  /opt/local
  /opt/csw
  /opt
)

# SDL may require threads on your system.
# The Apple build may not need an explicit flag because one of the
# frameworks may already provide it.
# But for non-OSX systems, I will use the CMake Threads package.
if(NOT APPLE)
  find_package(Threads)
endif()

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDLmain -lSDL
if(MINGW)
  set(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
endif()

if(SDL_LIBSDL)
  # For SDLmain
  if(SDL_LIBSDLMAIN AND NOT SDL_BUILDING_LIBRARY AND NOT SDL_INCLUDE_DIR MATCHES ".framework")
    list(FIND SDL_LIBSDL "${SDL_LIBSDLMAIN}" _SDL_MAIN_INDEX)
    if(_SDL_MAIN_INDEX EQUAL -1)
      list(APPEND _SDL_LIBRARY_TEMP "${SDL_LIBSDLMAIN}")
    endif()
    unset(_SDL_MAIN_INDEX)
  endif()

  list(APPEND _SDL_LIBRARY_TEMP "${SDL_LIBSDL}")

  # For OS X, SDL uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the
  # "real" variable in one-shot.
  if(APPLE)
    list(APPEND _SDL_LIBRARY_TEMP "-framework Cocoa")
  endif()

  if(Threads_FOUND)
    list(APPEND _SDL_LIBRARY_TEMP ${CMAKE_THREAD_LIBS_INIT})
  endif()

  # MinGW needs an additional library, mwindows
  # It's total link flags should look like -lmingw32 -lSDLmain -lSDL
  if(MINGW)
    list(INSERT 0 _SDL_LIBRARY_TEMP mingw32)
  endif()

  # Set the final string here so the GUI reflects the final state.
  set(SDL_LIBRARY ${_SDL_LIBRARY_TEMP} CACHE STRING "Full list of required SDL & supporting libraries.")
  unset(_SDL_LIBRARY_TEMP)

  get_filename_component(_SDL_LIBRARY_TEMP ${SDL_LIBSDL} PATH)
  set(SDL_LIBRARY_PATH ${_SDL_LIBRARY_TEMP} CACHE PATH "Path to SDL library.")
  unset(_SDL_LIBRARY_TEMP)
endif()

if(SDL_INCLUDE_DIR AND EXISTS "${SDL_INCLUDE_DIR}/SDL_version.h")
  file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MAJOR "${SDL_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MINOR "${SDL_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL_VERSION_PATCH "${SDL_VERSION_PATCH_LINE}")
  set(SDL_VERSION_STRING ${SDL_VERSION_MAJOR}.${SDL_VERSION_MINOR}.${SDL_VERSION_PATCH})
  unset(SDL_VERSION_MAJOR_LINE)
  unset(SDL_VERSION_MINOR_LINE)
  unset(SDL_VERSION_PATCH_LINE)
  unset(SDL_VERSION_MAJOR)
  unset(SDL_VERSION_MINOR)
  unset(SDL_VERSION_PATCH)
endif()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL
                                  REQUIRED_VARS SDL_LIBRARY SDL_LIBRARY_PATH SDL_INCLUDE_DIR
                                  VERSION_VAR SDL_VERSION_STRING)
