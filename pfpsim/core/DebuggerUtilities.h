/*
 * PFPSim: Library for the Programmable Forwarding Plane Simulation Framework
 *
 * Copyright (C) 2016 Concordia Univ., Montreal
 *     Samar Abdi
 *     Umair Aftab
 *     Gordon Bailey
 *     Faras Dewal
 *     Shafigh Parsazad
 *     Eric Tremblay
 *
 * Copyright (C) 2016 Ericsson
 *     Bochra Boughzala
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef CORE_DEBUGGERUTILITIES_H_
#define CORE_DEBUGGERUTILITIES_H_

#include "debugger/CPDebuggerInterface.h"
#include "debugger/DebugObserver.h"
#include "PFPObject.h"

class DebuggerUtilities {
 public:
  DebuggerUtilities();
  virtual ~DebuggerUtilities() = default;

  /**
   * Register ControlPlane Instance to the debugger object
   * @param cp_debug_if
   */
  void registerControlPlaneToDebugger(
       pfp::core::db::CPDebuggerInterface *cp_debug_if);
  /**
   * Thread function to service notifications from all observers
   * @param object which launches the thread [top]
   */
  void notify_observers(pfp::core::PFPObject* object);
  //! Observer for the debugger
  std::shared_ptr<pfp::core::db::DebugObserver> debug_obs;
};

#endif  // CORE_DEBUGGERUTILITIES_H_
