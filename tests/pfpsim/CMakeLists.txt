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

find_package(pfpsim REQUIRED)

IF(${pfpsim_FOUND})

	message(STATUS "pfpsim Found!")
  IF(EXISTS ${PFPSIM_INCLUDE_DIRS})
    message(STATUS "INCLUDE DIR: ${PFPSIM_INCLUDE_DIRS} exists")
  ELSE()
    message(FATAL_ERROR "Include dir ${PFPSIM_INCLUDE_DIRS} does not exist :(")
  ENDIF()

  find_library(PFPSIM_LIB_PATH NAMES ${PFPSIM_LIBRARIES})
  IF("${PFPSIM_LIB_PATH}" STREQUAL "PFPSIM_LIB_PATH-NOTFOUND")
    message(FATAL_ERROR "Libraries ${PFPSIM_LIBRARIES} could not be found")
  else()
    message(STATUS "Libraries ${PFPSIM_LIBRARIES} found at ${PFPSIM_LIB_PATH}")
  endif()

ENDIF()
