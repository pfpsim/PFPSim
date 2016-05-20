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

find_package(pfp-p4 REQUIRED)

IF(${pfp-p4_FOUND})

	message(STATUS "pfp-p4 Found!")
  IF(EXISTS ${PFP_P4_INCLUDE_DIRS})
    message(STATUS "INCLUDE DIR: ${PFP_P4_INCLUDE_DIRS} exists")
  ELSE()
    message(FATAL_ERROR "Include dir ${PFP_P4_INCLUDE_DIRS} does not exist :(")
  ENDIF()

  IF(EXISTS ${PFP_P4_LIBRARIES})
    message(STATUS "Libraries ${PFP_P4_LIBRARIES} exists")
  else()
    message(FATAL_ERROR "Libraries ${PFP_P4_LIBRARIES} does not exist")
  endif()

ELSE()
  message(FATAL_ERROR "pfp-p4 not found!!!!")
ENDIF()
