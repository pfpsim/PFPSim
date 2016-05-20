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
* @file DebuggerPacket.h
* Defines a class representation of a packet in the debugger context.
*
* Created on: February 8, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_DEBUGGERPACKET_H_
#define CORE_DEBUGGER_DEBUGGERPACKET_H_

#include <string>
#include <vector>

namespace pfp {
namespace core {
namespace db {

/**
 * Class representation of a packet for pfpdb.
 */
class DebuggerPacket {
 public:
  /**
   * Data structure to represent the location of a packet as well as the times at which it entered and left this location.
   */
  class PacketLocation {
   public:
    PacketLocation(): module(""), read_time(-1), write_time(-1) {}

    std::string module;
    double read_time;
    double write_time;
  };

  /**
   * Empty Constructor
   */
  DebuggerPacket();

  /**
   * Constructor
   * @param id ID of packet.
   * @param location Current module the packet is in.
   * @param time_ns Current simulation time in nanoseconds.
   */
  DebuggerPacket(int id, std::string location, double time_ns);

  /**
   * Update the time at which the packet entered a module.
   * @param mod   Module name.
   * @param rtime Time at which the packet entered the module.
   */
  void updateTraceReadTime(std::string mod, double rtime);

  /**
   * Update the time at which the packet left a module.
   * @param mod   Module name.
   * @param wtime Time at which the packet left the module.
   */
  void updateTraceWriteTime(std::string mod, double wtime);

  /**
   * Get the packet ID.
   * @return Packet id.
   */
  int getID() const;

  /**
   * Get current packet location (which module the packet is in).
   * @return Packet location.
   */
  std::string getLocation() const;

  /**
   * Get the time of the last update (read or write) to this packet.
   * @return Time of last update.
   */
  double getTime() const;

  /**
   * Get backtrace of the packet.
   * Includes which modules it when into as well as the times at which it entered and left the module.
   */
  std::vector<PacketLocation> getTrace() const;

  /**
   * Set the current location of the packet.
   * @param loc Module the packet is currently in.
   */
  void setCurrentLocation(std::string loc);

  /**
   * Set the current time of the packet. Normally corresponds to the last update on the packet.
   * @param t Current Time.
   */
  void setTime(double t);

 private:
  int packet_id;    /*!< Packet ID. */
  std::string current_location;    /*!< Current Location. */
  double last_notify_time;    /*!< Time of last update. */
  std::vector<PacketLocation> trace;   /*!< Backtrace of packet. */
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_DEBUGGERPACKET_H_
