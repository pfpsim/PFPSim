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
* @file Watchpoint.h
* Defines a class representation of a watchpoint for the debugger.
*
* Created on: February 11, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_WATCHPOINT_H_
#define CORE_DEBUGGER_WATCHPOINT_H_

#include <string>
#include <iostream>

namespace pfp {
namespace core {
namespace db {

/**
 * Class representation of watchpoints. Watchpoints can be set on counters.
 * When set, the user will be notified if the value of a counter changes.
 */
class Watchpoint {
 public:
  /**
   * Default Constructor.
   */
  Watchpoint();

  /**
   * Constructor
   * @param name Name of counter the Watchpoint is set on.
   * @param dis = Indicates if the Watchpoint is disabled.
   */
  explicit Watchpoint(std::string name, bool dis = false);

  /**
   * Get the unique ID of the Watchpoint.
   * @return ID of Watchpoint.
   */
  int getID() const;

  /**
   * Get name of counter the Watchpoint is set on.
   * @return Name of counter.
   */
  std::string getCounterName() const;

  bool disabled;   /*!< Indicates if the Watchpoint is disabled. */

 private:
  int id;   /*!< Unique ID of Watchpoint. */
  static int next_id;   /*!< ID of next Watchpoint that will be created. */
  std::string counter_name;    /*!< Name of counter the Watchpoint is set on. */
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_WATCHPOINT_H_
