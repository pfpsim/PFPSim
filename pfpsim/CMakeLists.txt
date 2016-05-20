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
PROJECT(PFPSIM)#TODO(LS):Check with samar about this thing.

execute_process(COMMAND .travis/get_version_from_tag.sh major OUTPUT_VARIABLE PFPSIM_MAJOR_VERSION)
execute_process(COMMAND .travis/get_version_from_tag.sh minor OUTPUT_VARIABLE PFPSIM_MINOR_VERSION)
execute_process(COMMAND .travis/get_version_from_tag.sh patch OUTPUT_VARIABLE PFPSIM_PATCH_VERSION)
set(PFPSIM_VERSION ${PFPSIM_MAJOR_VERSION}.${PFPSIM_MINOR_VERSION}.${PFPSIM_PATCH_VERSION})
message("PFPSim version is ${PFPSIM_VERSION}")

ADD_SUBDIRECTORY(core)

#--------Set install paths
# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

#-------------- Packaging CPACK --------------
#TODO(LS): add for more packaging Src tarbals etc
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Samar Abdi <pfpsim.help@gmail.com>") #required
SET(CPACK_PACKAGE_VERSION_MAJOR ${PFPSIM_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${PFPSIM_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${PFPSIM_PATCH_VERSION})
INCLUDE(CPack)

#------------------------ Build
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -std=c++11 ")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
endif()

#--------------Compiler Flags--------------------------------------------------
include (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG ("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG ("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  if (COMPILER_SUPPORTS_CXX11)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fmessage-length=0 -MMD -MP -Wno-reorder -Wno-return-type -Wno-unused -Wno-overloaded-virtual")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -fcolor-diagnostics")
    else()
      message(FATAL "UNSUPPORTED COMPILER: ${CMAKE_CXX_COMPILER} --- USE GCC or CLANG")
    endif()
  else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif ()
else()
  message (FATAL "MSVC is not supported - Please use GCC or CLANG ")
endif ()

# PROTOBUF
include(FindProtobuf)
find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})

if(EXISTS ${PROTOBUF_PROTOC_EXECUTABLE})
  message(STATUS "Found PROTOBUF Compiler: ${PROTOBUF_PROTOC_EXECUTABLE}")
else()
  message(FATAL_ERROR "Could not find PROTOBUF Compiler")
endif()

# Generate protobuf files
set(PROTO_FILE_NAME "PFPSimDebugger")

set(PROTO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/core/proto/${PROTO_FILE_NAME}.pb.cc")
set(PROTO_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/core/proto/${PROTO_FILE_NAME}.pb.h")

add_custom_command(
  OUTPUT ${PROTO_SRC}
         ${PROTO_HEADER}
  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ARGS -I=${CMAKE_CURRENT_SOURCE_DIR}/core/proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/core/proto ${CMAKE_CURRENT_SOURCE_DIR}/core/proto/${PROTO_FILE_NAME}.proto
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/proto/${PROTO_FILE_NAME}.proto"
  COMMENT "Running C++ protocol buffer compiler" VERBATIM
)

set_source_files_properties(${PROTO_SRC} ${PROTO_HEADER} PROPERTIES GENERATED TRUE)
set_source_files_properties(${CP_SRC} ${CP_HEADERS} PROPERTIES GENERATED TRUE)

set_source_files_properties(${CP_FILES} PROPERTIES GENERATED TRUE)

#--Find Packages
# Add find_package modules for SystemC and TLM
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/core/cmake/modules/")
# Find SystemC and TLM
find_package (SystemC 2.3 REQUIRED)
find_package (TLM REQUIRED)
find_library (LIBNANOMSG nanomsg REQUIRED)
find_library (LIBPCAP pcap REQUIRED)

include_directories (${SystemC_INCLUDE_DIRS} ${TLM_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Support for SystemC dynamic processes
add_definitions (-DSC_INCLUDE_DYNAMIC_PROCESSES)

#----------------------
set(PFPSIM_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/pfpsim.h)

set(PFPSIM_LIBRARY
  ${PFPSIM_LIBRARY}
  ${PFPSIM_HEADER}
  ${PROTO_SRC}
  ${PROTO_HEADER}
  ${PROTOBUF_LIBRARY}
  ${CP_FILES}
  ${SystemC_LIBRARIES})

add_library(pfpsim STATIC ${PFPSIM_LIBRARY})
add_dependencies(pfpsim cp)
get_property(PFPSIM_INC_DIR TARGET pfpsim PROPERTY INCLUDE_DIRECTORIES)
set_target_properties(pfpsim PROPERTIES
PUBLIC_HEADER "${PFPSIM_INSTALL_FILES}")

set(INSTALL_PFPSIM_DIR "${INSTALL_INCLUDE_DIR}/pfpsim/pfpsim")
set(INSTALL_PFPSIM_CORE_DIR "${INSTALL_PFPSIM_DIR}/core")

install(TARGETS pfpsim
  # IMP: Add the PFPSIM library to the "export-set"
  EXPORT pfpsimTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib
  PUBLIC_HEADER DESTINATION "${INSTALL_PFPSIM_CORE_DIR}"
  COMPONENT core)

install(FILES ${PFPSIM_HEADER}
  DESTINATION ${INSTALL_PFPSIM_DIR}
  COMPONENT pfpsim)

# Install generated proto header
install(FILES ${PROTO_HEADER}
  DESTINATION "${INSTALL_PFPSIM_CORE_DIR}/proto"
  COMPONENT core)

# Install debugger headers
install(FILES ${DEBUGGER_HEADERS}
  DESTINATION "${INSTALL_PFPSIM_CORE_DIR}/debugger/"
  COMPONENT core)

# Install cp headers
install(FILES ${CP_HEADERS}
  DESTINATION "${INSTALL_PFPSIM_CORE_DIR}/cp/"
  COMPONENT core)

  #-------------- CMake Install Steps

  # Make relative paths absolute (needed later on)
  foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
      set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
  endforeach()
  #-------
  # Add all targets to the build-tree export set
  export(TARGETS pfpsim FILE "${PROJECT_BINARY_DIR}/pfpsimTargets.cmake")
  # Create the PFPSIMConfig.cmake and PFPSIMConfigVersion files
  file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}/pfpsim" "${INSTALL_INCLUDE_DIR}")

  # install build-dir
  set(CONF_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
  )
  set(CONF_EXPORT_TARGETS
    "pfpsim"
  )

  configure_file(pfpsimConfig.cmake.in "${PROJECT_BINARY_DIR}/pfpsimConfig.cmake" @ONLY)
  # install tree
  set(CONF_INCLUDE_DIRS
    "\${PFPSIM_CMAKE_DIR}/${REL_INCLUDE_DIR}"

  )
  configure_file(pfpsimConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/pfpsimConfig.cmake" @ONLY)
  # both
  configure_file(pfpsimConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/pfpsimConfigVersion.cmake" @ONLY)
  # Install PFPSIMConfig.cmake & PFPSIMConfigVersion.cmake & FindSystemC/TLM
  install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/pfpsimConfig.cmake"
    "${PROJECT_BINARY_DIR}/pfpsimConfigVersion.cmake"
    "${PROJECT_SOURCE_DIR}/core/cmake/modules/FindSystemC.cmake"
    "${PROJECT_SOURCE_DIR}/core/cmake/modules/FindTLM.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}/pfpsim"
    COMPONENT core)
  # Install the export set for use with the install tree/thing
  install(EXPORT pfpsimTargets DESTINATION
    "${INSTALL_CMAKE_DIR}/pfpsim"
    COMPONENT core)

#--------Doxygen----------------
# add a target [doc] to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)

  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile)
  add_custom_target(doc
    COMMAND doxygen ${doxyfile_in}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

endif(DOXYGEN_FOUND)
