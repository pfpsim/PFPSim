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



configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

ADD_SUBDIRECTORY(debugger "${CMAKE_CURRENT_BINARY_DIR}/debugger")
ADD_SUBDIRECTORY(cp)

set(DEBUGGER_HEADERS ${DEBUGGER_HEADERS} PARENT_SCOPE)
set(CP_HEADERS ${CP_HEADERS} PARENT_SCOPE)

set(PFPSIMCoreSources
${CMAKE_CURRENT_SOURCE_DIR}/PFPObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.cpp
${CMAKE_CURRENT_SOURCE_DIR}/PFPConfig.cpp
${CMAKE_CURRENT_SOURCE_DIR}/PFPContext.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationParameters.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerUtilities.cpp
${CMAKE_CURRENT_SOURCE_DIR}/pfp_main.cpp
${DEBUGGER_SRC}
)

set(PFPSIM_INCLUDE_HEADERS
${CMAKE_CURRENT_SOURCE_DIR}/PFPObject.h
${CMAKE_CURRENT_SOURCE_DIR}/PFPConfig.h
${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationParameters.h
${CMAKE_CURRENT_SOURCE_DIR}/TrType.h
${CMAKE_CURRENT_SOURCE_DIR}/MTQueue.h
${CMAKE_CURRENT_SOURCE_DIR}/LMTQueue.h
${CMAKE_CURRENT_SOURCE_DIR}/promptcolors.h
${CMAKE_CURRENT_SOURCE_DIR}/PacketBase.h
${CMAKE_CURRENT_SOURCE_DIR}/PFPObserver.h
${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.h
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerUtilities.h
${CMAKE_CURRENT_SOURCE_DIR}/pfp_main.h
)

set(PFPSIM_LIBRARY
  ${PFPSIMCoreSources}
  ${PFPSIM_INCLUDE_HEADERS}
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  PARENT_SCOPE)

set(CP_FILES
  ${CP_SRC}
  ${CP_HEADERS}
  PARENT_SCOPE)

set(PFPSIM_INSTALL_FILES
  ${PFPSIM_INCLUDE_HEADERS}
  ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  PARENT_SCOPE
  )
