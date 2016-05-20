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

/**
* @file Breakpoint.h
* Defines a class representation of a breakpoint.
*
* Created on: February 3, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_BREAKPOINT_H_
#define CORE_DEBUGGER_BREAKPOINT_H_

#include <string>
#include <iostream>
#include <map>

namespace pfp {
namespace core {
namespace db {

/**
 * @brief Class representation of a pfpdb breakpoint.
 */
class Breakpoint {
 public:
   /**
    * Enumeration representing the various conditions on which the user can break.
    * The user can break when a module reads a packet, a module writes a packet, a specific simulation time is reached or when a packet hits any module.
    */
  enum BreakpointCondition {
    BREAK_ON_MODULE_READ,
    BREAK_ON_MODULE_WRITE,
    BREAK_AT_TIME,
    BREAK_ON_PACKET_ID
  };

  /**
   * Default Constructor
   * @param stealth Indicates whether the breakpoint is internal or set by the user.
   */
  explicit Breakpoint(bool stealth = false);

  /**
   * Function to compare two Breakpoint objects.
   * @param  br object to compare to.
   * @return    true if they are equal, otherwise false.
   */
  bool isEqual(Breakpoint br);

  /**
   * Add a condition to the Breakpoint.
   * @param condition condition to be added.
   * @param value     value to break on.
   */
  void addCondition(BreakpointCondition condition, std::string value);

  /**
   * Get the unique ID of the Breakpoint
   * @return the unique ID.
   */
  int getID() const;

  //! Map containing all the conditions of the Breakpoint
  //! and the associated value.
  std::map<BreakpointCondition, std::string> conditions;
  //! Indicates whether the Breakpoint is temporary.
  //! If temporary, it will be destroyed when hit.
  bool temp;
  //! Indicates whether the Breakpoint is disabled.
  //! If disabled, it will not be hit but will still exist so that it
  //! can be enabled later.
  bool disabled;

 private:
  //! Unique identifier
  int id;
  //! Maintains the ID of the next Breakpoint that will be created.
  static int next_id;
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_BREAKPOINT_H_
