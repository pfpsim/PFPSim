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

project(pfpsim)

cmake_minimum_required(VERSION 2.8.7)

#--------------OPTION FLAGS------------------------------------------------
option(PFPSIM "PFPSIM" ON)  # cmake -DPFPSIMCORE=ON|OFF
option(PFP-P4 "PFP-P4" ON)  #cmake -DPFP-P4=ON|OFF

#--------------Add Subdirectories------------------------------------------------
if("${PFP-P4}" MATCHES "ON")
  message (STATUS "Install PFP-P4? Yes")
  add_subdirectory(pfp-p4)
else ()
  message (STATUS "Install PFP-P4? No")
endif()

if("${PFPSIM}" MATCHES "ON")
  message (STATUS "Install PFPSIM? Yes")
  add_subdirectory(pfpsim)
else ()
  message (STATUS "Install PFPSIM? No")
endif()

#------------------------ uninstall target -----------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
