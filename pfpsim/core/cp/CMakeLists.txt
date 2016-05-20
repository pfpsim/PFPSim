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

cmake_minimum_required (VERSION 2.8.7)
set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} --std=c++11 -stdlib=libstdc++ -ggdb")
include(FindBISONCPP.cmake)
include(FindFLEXCPP.cmake)

set(ParserClass  "CommandParser" )
set(ScannerClass "CommandScanner")

BISONCPP_TARGET(${ParserClass}
                ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y
                ${ParserClass}.ipp ${ParserClass}
                COMPILE_FLAGS
                 "--scanner=${ScannerClass}.h --scanner-class-name=${ScannerClass}")

FLEXCPP_TARGET (${ScannerClass} ${CMAKE_CURRENT_SOURCE_DIR}/tokens.l  ${ScannerClass}.ipp ${ScannerClass})

ADD_FLEXCPP_BISONCPP_DEPENDENCY(${ScannerClass} ${ParserClass})

# For our NAMESPACE_HACK we need to wrap the generated c++ files
# This custom command lets cmake know about the dependenciy between
# The generated files and the wrapping for it.
add_custom_command(OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.cpp
                   COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE_HACK_GEN_WRAPPER ${ParserClass} > ${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.cpp
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.ipp)
add_custom_command(OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.cpp
                   COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE_HACK_GEN_WRAPPER ${ScannerClass} > ${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.cpp
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.ipp)

set(CP_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/Commands.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.cpp
  PARENT_SCOPE)

set(CP_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}base.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}base.h"
  ${CMAKE_CURRENT_SOURCE_DIR}/Commands.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE_HACK_BEGIN
  ${CMAKE_CURRENT_SOURCE_DIR}/NAMESPACE_HACK_END
  PARENT_SCOPE)

add_custom_target(cp
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ParserClass}.cpp
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ScannerClass}.cpp)
