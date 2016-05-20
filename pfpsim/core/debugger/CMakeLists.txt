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

set(DEBUGGER_SRC
${DEBUGGER_SRC}
${CMAKE_CURRENT_SOURCE_DIR}/DebugObserver.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerMessages.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerIPCServer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebugDataManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Breakpoint.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerPacket.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Watchpoint.cpp
${CMAKE_CURRENT_SOURCE_DIR}/CPDebuggerInterface.cpp
PARENT_SCOPE
)

set(DEBUGGER_HEADERS
${DEBUGGER_HEADERS}
${CMAKE_CURRENT_SOURCE_DIR}/DebugObserver.h
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerMessages.h
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerIPCServer.h
${CMAKE_CURRENT_SOURCE_DIR}/DebugDataManager.h
${CMAKE_CURRENT_SOURCE_DIR}/Breakpoint.h
${CMAKE_CURRENT_SOURCE_DIR}/DebuggerPacket.h
${CMAKE_CURRENT_SOURCE_DIR}/Watchpoint.h
${CMAKE_CURRENT_SOURCE_DIR}/CPDebuggerInterface.h
PARENT_SCOPE
)
