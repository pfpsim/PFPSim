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
* @file CPDebuggerInterface.h
* Defines an interface to be implemented by Control Plane modules for compatibility with the debugger.
*
* Created on: March 2, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_CPDEBUGGERINTERFACE_H_
#define CORE_DEBUGGER_CPDEBUGGERINTERFACE_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace pfp {
namespace core {
namespace db {

/**
 * Interface for Control Plane module that is necessary for it to work with pfpdb.
 * It defines what operations must be supported for the debugger to work.
 * Provides functions to maintain a list of entries so that it may be accessed by the debugger.
 * It is the user's responsibility to call these functions at the appropriate time so that the debugger remains up-to-date.
 */
class CPDebuggerInterface {
 public:
  enum TableEntryStatus {
    OK,
    INSERTING,
    DELETING,
    MODIFYING,
    NONE
  };

  /**
   * Data Structure used to represent table entries so that they can be stored.
   */
  class TableEntry {
   public:
    TableEntry() {}
    TableEntry(std::string table_name, std::string match_key,
          std::string action_name, std::vector<std::string> action_data,
          uint64_t handle, CPDebuggerInterface::TableEntryStatus status = NONE)
          : table_name(table_name), match_key(match_key),
          action_name(action_name), action_data(action_data),
          handle(handle), status(status) {}

    //! Name of table in which the entry is found.
    std::string table_name;
    //! Key used to perform the match.
    std::string match_key;
    //! Name of the action that is used if this entry is hit.
    std::string action_name;
    //! Data inserted into the action if this entry is hit.
    std::vector<std::string> action_data;
    //! Handle (or unique identifier) corresponding to this entry.
    uint64_t handle;
    CPDebuggerInterface::TableEntryStatus status;
  };

  virtual void do_command(std::string command) = 0;

  /**
   * Update the handle of an entry in internal list of entries. Useful if the handle is not known when addTableEntry is called
   * This handle is necessary for modifying or deleting an entry.
   * @param table_name  Name of table in which the entry is found.
   * @param match_key   Key of the entry.
   * @param action_name Name of the action of the entry.
   * @param handle      New unique identifier for entry.
   */
  void updateHandle(std::string table_name, std::string match_key,
                    std::string action_name, uint64_t handle);

  /**
   * Add a table entry to internal list of entries
   * @param table_name  Name of table to which the entry belongs.
   * @param match_key   Match key for the entry.
   * @param action_name Name of action associated with entry.
   * @param action_data Data used with the action for this entry.
   */
  void addTableEntry(std::string table_name, std::string match_key,
        std::string action_name, std::vector<std::string> action_data,
        uint64_t handle = -1);

  /**
   * Print the internal list of entries to stdout. Useful for debugging.
   */
  void printTableEntries();

  /**
   * Update or modify an entry that has already been added to the internal list of entries.
   * @param table_name  Name of table to which the entry belongs.
   * @param handle      Unique identifier of the entry.
   * @param action_name Name of the new action for the entry.
   * @param action_data The new data to be used with the action.
   */
  void updateTableEntry(std::string table_name, uint64_t handle,
        std::string action_name, std::vector<std::string> action_data);

  /**
   * Delete an entry from the internal list of entries.
   * @param table_name [description]
   * @param handle     [description]
   */
  void deleteTableEntry(std::string table_name, uint64_t handle);

  // confirm an update/modify
  void confirmUpdateEntry(std::string table_name, uint64_t handle);

  // confirm a update / delete
  void confirmDeleteEntry(std::string table_name, uint64_t handle);

  /**
   * Get internal list of table entries.
   */
  std::vector<CPDebuggerInterface::TableEntry> getAllTableEntries();

 protected:
  //! List of table entries (in a more convenient data structure)
  std::map<std::string, std::map<std::string,
        std::vector<TableEntry>>> table_entries;
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_CPDEBUGGERINTERFACE_H_
