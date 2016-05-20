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
* @file DebugDataManager.h
* Defines class which stores any data about taht simulation that is required by pfpdb.
*
* Created on: January 28, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_DEBUGDATAMANAGER_H_
#define CORE_DEBUGGER_DEBUGDATAMANAGER_H_

#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <tuple>

#include "Breakpoint.h"
#include "Watchpoint.h"
#include "DebuggerPacket.h"

namespace pfp {
namespace core {
namespace db {

/**
 * Stores any data acquired from the simulation from the observer so that it may be fetched by the server and sent to the debugger.
 */
class DebugDataManager {
 public:
  /**
   * Default Constructor
   */
  DebugDataManager();

  /**
   * Add a counter.
   * @param name Name of counter.
   */
  void addCounter(std::string name);

  /**
   * Remove a counter.
   * @param name Name of counter.
   */
  void removeCounter(std::string name);

  /**
   * Update the value of a counter.
   * @param name Name of counter.
   * @param val  New counter value.
   */
  void updateCounter(std::string name, int val);

  /**
   * Add a Breakpoint.
   * @param br Breakpoint to add.
   */
  void addBreakpoint(Breakpoint br);

  /**
   * Remove a Breakpoint.
   * @param identifier ID of Breakpoint to remove.
   */
  void removeBreakpoint(int identifier);

  /**
   * Enable a Breakpoint. Does nothing if it is already enabled.
   * @param id ID of Breakpoint to enable.
   */
  void enableBreakpoint(int id);

  /**
   * Disable a Breakpoint. Does nothing if it is already disabled.
   * @param id ID of Breakpoint to disable.
   */
  void disableBreakpoint(int id);

  /**
   * Add a new packet or update an existing one.
   * @param id     ID of packet.
   * @param module Name of module the packet is currently in.
   * @param time_  Time of update.
   * @param read   Indicates whether the update is for a read or a write. True = read, False = write.
   */
  void updatePacket(int id, std::string module, double time_, bool read);

  /**
   * Remove a packet.
   * @param id ID of packet.
   */
  void removePacket(int id);

  /**
   * Add a Watchpoint.
   * @param wp Watchpoint to add.
   */
  void addWatchpoint(Watchpoint wp);

  /**
   * Remove a Watchpoint.
   * @param id ID of Watchpoint to remove.
   */
  void removeWatchpoint(int id);

  /**
   * Enable a Watchpoint. Does nothing if the Watchpoint is already enabled.
   * @param id ID of Watchpoint to enable.
   */
  void enableWatchpoint(int id);

  /**
   * Disable a Watchpoint. Does nothing if the Watchpoint is already disabled.
   * @param id ID of Watchpoint to disable.
   */
  void disableWatchpoint(int id);

  /**
   * Get the ID of the packet that is the debugger is currently following or focusing on.
   * @return ID of packet.
   */
  int whoami();

  /**
   * Set which packet is currently be followed or focused on.
   * @param id ID of packet.
   */
  void set_whoami(int id);

  /**
   * Add a module to ignore.
   * Ignored modules will not appear in the output of certain debugger commands and the debugger will not break on these modules.
   * @param mod Name of module to ignore.
   */
  void addIgnoreModule(std::string mod);

  /**
   * Check if the given module is being ignored.
   * @param  mod Name of module to check.
   * @return     true if the module is ignored, false if it is not.
   */
  bool checkIgnoreModules(std::string mod);

  /**
   * Remove a module from the list of ignored modules.
   * @param mod Name of module to stop ignoring.
   */
  void removeIgnoreModule(std::string mod);

  /**
   * Add a packet to the list of dropped packets.
   * @param id     ID of packet.
   * @param module Module it was dropped in.
   * @param reason Reason for which it was dropped.
   */
  void addDroppedPacket(int id, std::string module, std::string reason);

  /**
   * Set whether the debugger should break when a packet is dropped.
   * @param b True will enable the breaking on drop packets. False will disable it.
   */
  void setBreakOnPacketDrop(bool b);

  /**
   * Check if the debugger is currently set to break when a packet is dropped.
   * @return true if it is set to break, false if it is not.
   */
  bool getBreakOnPacketDrop();

  /**
   * Get the value of a counter.
   * @param  name Name of counter.
   * @return      Current value of counter.
   */
  int getCounterValue(std::string name);

  /**
   * Get map of counters and their values.
   * @return      Map with counter name as the key and value of the counter as the value.
   */
  std::map<std::string, int> getCounters();

  /**
   * Get Breakpoint at given index from list of Breakpoints.
   * @param  index Index in the list.
   * @return       Breakpoint at given index.
   */
  Breakpoint getBreakpoint(int index);

  /**
   * Get list of Breakpoints.
   * @return Vector of Breakpoints.
   */
  std::vector<Breakpoint>& getBreakpointList();

  /**
   * Get number of Breakpoints in list.
   * @return Number of Breakpoints.
   */
  int getNumberOfBreakpoints();

  /**
   * Get current simulation time.
   * @return Current simulation time.
   */
  double getSimulationTime();

  /**
   * Get DebuggerPacket with given ID>
   * @param  id ID of desired packet.
   * @return    Packet with given ID. Returns NULL if it does not exist.
   */
  DebuggerPacket* getPacket(int id);

  /**
   * Get the map containing the DebuggerPacket as the key and the DebuggerPacket object as the value.
   * @return Map of DebuggerPackets.
   */
  std::map<int, DebuggerPacket>& getPacketList();

  /**
   * Get list of Watchpoints.
   * @return Vector of Watchpoints.
   */
  std::vector<Watchpoint>& getWatchpointList();

  /**
   * Get list of modules that are being ignored.
   * @return Vector containing the names of the modules being ignored.
   */
  std::vector<std::string>& getIgnoreModuleList();

  /**
   * Get vector of tuple representing a dropped packet. The first element is the packet ID, the second element is the module in which the packet was dropped and the third element is the reason for which it was dropped.
   * @return Vector of dropped packets.
   */
  std::vector<std::tuple<int, std::string,
        std::string>>& getDroppedPacketList();

  /**
   * Set the current simulation time.
   * @param time_ns Current simulation time in nanoseconds.
   */
  void setSimulationTime(double time_ns);

 private:
  //! Map with counter name as key and counter value as the value.
  std::map<std::string, int> counters;
  //! Mutex to make sure only one thread access the variables
  //! of this class at a time.
  std::mutex mutex_;
  //! Map of packets in simulator with their id as the key.
  std::map<int, DebuggerPacket> packet_list;
  //! List of Breakpoint objects.
  std::vector<Breakpoint> breakpoint_list;
  //! simulation time in ns
  double simulation_time;
  //! whoami packet id
  int current_packet_id;
  //! List of Watchpoint objects.
  std::vector<Watchpoint> watchpoint_list;
  //! Next available Watchpoint ID.
  int next_watchpoint_id;
  //! list of modules to be ignored.
  std::vector<std::string> ignore_module_list;
  //! List of packets that were dropped.
  std::vector<std::tuple<int, std::string, std::string>> dropped_packet_list;
  //! Flag which indicates if the debugger should break when a packet
  //! is dropped.
  bool break_packet_dropped;
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_DEBUGDATAMANAGER_H_
