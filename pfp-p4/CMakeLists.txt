#
# PFPSim: Library for the Programmable Forwarding Plane Simulation Framework
#
# Copyright (C) 2016 Concordia Univ., Montreal
#     Samar Abdi
#     Umair Aftab
#     Gordon Bailey
#     Faras Dewal
#     Shafigh Parsazad
#     Eric Tremblay
#
# Copyright (C) 2016 Ericsson
#     Bochra Boughzala
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

cmake_minimum_required( VERSION 2.8.7)
PROJECT(PFP-P4)#TODO(LS):Check with samar about this thing.

set(PFP_P4_MAJOR_VERSION 0)
set(PFP_P4_MINOR_VERSION 0)
set(PFP_P4_PATCH_VERSION 0)
set(PFP_P4_VERSION ${PFP_P4_MAJOR_VERSION}.${PFP_P4_MINOR_VERSION}.${PFP_P4_PATCH_VERSION})

#--------Set install paths (TODO(gb) factor out to toplevel
# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

#-------------- Packaging CPACK --------------
#TODO(LS): add for more packaging Src tarbals etc
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Samar Abdi <pfpsim.help@gmail.com>") #required
SET(CPACK_PACKAGE_VERSION_MAJOR ${PFP_P4_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${PFP_P4_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${PFP_P4_PATCH_VERSION})
INCLUDE(CPack)

#--------------Compiler Flags--------------------------------------------------
include (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG ("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG ("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  if (COMPILER_SUPPORTS_CXX11)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CONF_CXX "clang")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
      set(CONF_CXX "gcc")
    endif()
  else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif ()
else()
  message (FATAL "MSVC is not supported - Please use GCC or CLANG ")
endif ()

configure_file(cmake-configure-template "${CMAKE_CURRENT_SOURCE_DIR}/cmake-configure" @ONLY)

include(ExternalProject)

set(PFP_P4_LIB     ${CMAKE_CURRENT_SOURCE_DIR}/libpfp-p4.a)
set(PFP_P4_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/pfp-p4)

ExternalProject_Add(pfp-p4    # Name for custom target
 #--Download step--------------
  # URL of git repo
  GIT_REPOSITORY "https://github.com/pfpsim/p4-behavioral-model.git"
  # Git branch name, commit id or tag
  GIT_TAG "pfp-compiler-test"
  # Should certificate for https be checked

 #--Update/Patch step----------
 UPDATE_COMMAND ""  # Source work-tree update command
 #--Configure step-------------
  # Source dir to be used for build
  SOURCE_DIR "p4-behavioral-model"
  # Build tree configuration command
  CONFIGURE_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake-configure"
 #--Build step-----------------
  # Command to drive the native build
  BUILD_COMMAND "make" "-j2" # We don't know n of cores, but atleast we can -j2
  # Use source dir for build dir
  BUILD_IN_SOURCE true
 #--Install step---------------
  # Command to drive install after build
  INSTALL_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake-extract" ${PFP_P4_LIB} ${PFP_P4_INCLUDE}
 ##--Test step------------------
  # Add test step executed before install step
  TEST_BEFORE_INSTALL false
  # Add test step executed after install step
  TEST_AFTER_INSTALL false
)

install(FILES ${PFP_P4_LIB}
  DESTINATION ${INSTALL_LIB_DIR}
  COMPONENT pfp-p4)
install(DIRECTORY ${PFP_P4_INCLUDE}
  DESTINATION ${INSTALL_INCLUDE_DIR}/pfpsim
  COMPONENT pfp-p4)

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")

set(CONF_INCLUDE_DIRS
"${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")

get_filename_component(PFP_P4_INCLUDE_FNAME ${PFP_P4_INCLUDE} NAME)
set(PFP_P4_INCLUDE_FULL_PATH ${INSTALL_INCLUDE_DIR}/pfpsim/${PFP_P4_INCLUDE_FNAME})

set(PFP_P4_CMAKE_CONFIG pfp-p4Config.cmake)
configure_file(${PFP_P4_CMAKE_CONFIG}.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PFP_P4_CMAKE_CONFIG}" @ONLY)

set(PFP_P4_CMAKE_VERSION pfp-p4ConfigVersion.cmake)
configure_file(${PFP_P4_CMAKE_VERSION}.in ${PROJECT_BINARY_DIR}/${PFP_P4_CMAKE_VERSION} @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PFP_P4_CMAKE_CONFIG}"
  ${PROJECT_BINARY_DIR}/${PFP_P4_CMAKE_VERSION}
DESTINATION ${INSTALL_CMAKE_DIR}/pfp-p4
COMPONENT pfp-p4)
