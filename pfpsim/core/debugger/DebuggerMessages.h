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
* @file DebuggerMessages.h
* Defines the message objects that can be sent as replies to pfpdb through the DebuggerIPCServer
*
* Created on: January 27, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_DEBUGGERMESSAGES_H_
#define CORE_DEBUGGER_DEBUGGERMESSAGES_H_

#include <string>
#include <vector>
#include <iostream>

#include "../proto/PFPSimDebugger.pb.h"
#include "Breakpoint.h"
#include "Watchpoint.h"
#include "CPDebuggerInterface.h"

#include "../TrType.h"

namespace pfp {
namespace core {
namespace db {

/**
 * Base class for an messages that will be sent to pfpdb via the DebuggerIPCServer.
 */
class DebuggerMessage {
 public:
   /**
    * Default Constructor
    * @param type The type of the message as indicate in the proto file. This allows pfpdb to figure out what message it is receiving.
    */
  explicit DebuggerMessage(PFPSimDebugger::DebugMsg_Type type);

  /**
   * Destructor
   */
  ~DebuggerMessage();

  /**
   * Serializes the message to a string using protobuf so that it can easily be send via the IPC.
   * @param  output The serialized string.
   * @return        True if the serialization was successful, false otherwise.
   */
  bool SerializeToString(std::string* output);

  /**
   * Creates a human-readable string representation of the message so that it can be printed in a log.
   * @return Human-readable string representation of the message.
   */
  std::string DebugString();

  /**
   * Get the type of the message.
   * @return Type of message.
   */
  PFPSimDebugger::DebugMsg_Type type();

 protected:
  PFPSimDebugger::DebugMsg message;   /*!< protobuf message object. */
};

/**
 * Message which returns the value of a counter.
 */
class CounterValueMessage: public DebuggerMessage {
 public:
  CounterValueMessage(std::string name, int value);
};

/**
 * Message which returns the value of all the counters.
 */
class AllCounterValuesMessage: public DebuggerMessage {
 public:
  AllCounterValuesMessage(std::string *name_list, int *value_list, int num);
};

/**
 * Message which indicates that a breakpoint has been hit.
 */
class BreakpointHitMessage: public DebuggerMessage {
 public:
  BreakpointHitMessage(int id, std::string module, int packet_id,
        double sim_time, bool read);
};

/**
 * Message which is sent to indicate that a request was successful or not when the debugger isn't expecting any information back.
 */
class GenericAcknowledgeMessage: public DebuggerMessage {
 public:
  GenericAcknowledgeMessage(
        PFPSimDebugger::GenericAcknowledgeMsg_Status status);
};

/**
 * Message which returns the list of all breakpoints.
 */
class AllBreakpointValuesMessage: public DebuggerMessage {
 public:
  explicit AllBreakpointValuesMessage(std::vector<Breakpoint> breakpoint_list);
};

/**
 * Message which returns the ID of the packet which is current being followed.
 */
class WhoAmIReplyMessage: public DebuggerMessage {
 public:
  explicit WhoAmIReplyMessage(int packet_id);
};

/**
 * Message which returns the list of all the packets currently within the simulation.
 */
class PacketListValuesMessage: public DebuggerMessage {
 public:
  PacketListValuesMessage(int *id_list, std::string *location_list,
        double *time_list, int num);
};

/**
 * Message which indicates that a watchpoint has been hit.
 */
class WatchpointHitMessage: public DebuggerMessage {
 public:
  WatchpointHitMessage(int id, std::string counter_name,
        int old_value, int new_value);
};

/**
 * Message which returns the list of all the watchpoints.
 */
class AllWatchpointValuesMessage: public DebuggerMessage {
 public:
  explicit AllWatchpointValuesMessage(std::vector<Watchpoint> watchpoint_list);
};

/**
 * Message which returns the backtrace of a packet.
 */
class BacktraceReplyMessage: public DebuggerMessage {
 public:
  BacktraceReplyMessage(int id, std::string *module_list,
        double *read_time_list, double *write_time_list, int num);
};

/**
 * Message which indicates that the simulation has ended.
 */
class SimulationEndMessage: public DebuggerMessage {
 public:
  SimulationEndMessage();
};

/**
 * Message which indicates that the simulation has stopped.
 */
class SimulationStoppedMessage: public DebuggerMessage {
 public:
  SimulationStoppedMessage(std::string module, int packet_id,
        double time_ns, bool read);
};

/**
 * Message which returns the list of all the modules that are being ignored.
 */
class AllIgnoreModulesMessage: public DebuggerMessage {
 public:
  AllIgnoreModulesMessage(std::string *module_list, int num);
};

/**
 * Message which returns the current simulation time.
 */
class SimulationTimeMessage: public DebuggerMessage {
 public:
  explicit SimulationTimeMessage(double sim_time);
};

/**
 * Message which indicates that a packet has been dropped.
 */
class PacketDroppedMessage: public DebuggerMessage {
 public:
  PacketDroppedMessage(int id, std::string module, std::string reason);
};

/**
 * Message which returns the list of all dropped packets.
 */
class DroppedPacketsMessage: public DebuggerMessage {
 public:
  DroppedPacketsMessage(int *id_list, std::string *module_list,
        std::string *reason_list, int num);
};

/**
 * Message which returns the list of all the table entries in all of the tables.
 */
class TableEntriesMessage: public DebuggerMessage {
 public:
  TableEntriesMessage(
        std::vector<CPDebuggerInterface::TableEntry> table_entries);
};

class ParsedPacketValueMessage: public DebuggerMessage {
 public:
  ParsedPacketValueMessage(const std::vector<DebugInfo::Header> & headers);
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_DEBUGGERMESSAGES_H_
