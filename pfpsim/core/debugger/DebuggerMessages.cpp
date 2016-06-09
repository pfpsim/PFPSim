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

#include "DebuggerMessages.h"
#include <string>
#include <vector>

#include "../TrType.h"

namespace pfp {
namespace core {
namespace db {

DebuggerMessage::DebuggerMessage(PFPSimDebugger::DebugMsg_Type type) {
  message.set_type(type);
}

DebuggerMessage::~DebuggerMessage() {
  message.release_message();
}

bool DebuggerMessage::SerializeToString(std::string* output) {
  return message.SerializeToString(output);
}

std::string DebuggerMessage::DebugString() {
  return message.DebugString();
}

PFPSimDebugger::DebugMsg_Type DebuggerMessage::type() {
  return message.type();
}

CounterValueMessage::CounterValueMessage(std::string name, int value)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_CounterValue) {
  PFPSimDebugger::CounterValueMsg counter_value_msg;
  counter_value_msg.set_name(name);
  counter_value_msg.set_value(value);
  message.set_message(counter_value_msg.SerializeAsString());
}

AllCounterValuesMessage::AllCounterValuesMessage(std::string *name_list,
      int *value_list, int num)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_AllCounterValues) {
  PFPSimDebugger::AllCounterValuesMsg all_counters_message;
  for (int i = 0; i < num; i++) {
    all_counters_message.add_name_list(name_list[i]);
    all_counters_message.add_value_list(value_list[i]);
  }
  message.set_message(all_counters_message.SerializeAsString());
}

BreakpointHitMessage::BreakpointHitMessage(int id, std::string module,
      int packet_id, double sim_time, bool read)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_BreakpointHit) {
  PFPSimDebugger::BreakpointHitMsg breakpoint_hit_message;
  breakpoint_hit_message.set_id(id);
  breakpoint_hit_message.set_module(module);
  breakpoint_hit_message.set_packet_id(packet_id);
  breakpoint_hit_message.set_time_ns(sim_time);
  if (read) {
    breakpoint_hit_message.set_read("1");
  } else {
    breakpoint_hit_message.set_read("0");
  }
  message.set_message(breakpoint_hit_message.SerializeAsString());
}

GenericAcknowledgeMessage::GenericAcknowledgeMessage(
      PFPSimDebugger::GenericAcknowledgeMsg_Status status)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_GenericAcknowledge) {
  PFPSimDebugger::GenericAcknowledgeMsg generic_acknowledge;
  generic_acknowledge.set_status(status);
  message.set_message(generic_acknowledge.SerializeAsString());
}

AllBreakpointValuesMessage::AllBreakpointValuesMessage(
  std::vector<Breakpoint> breakpoint_list)
  : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_AllBreakpointValues) {
  PFPSimDebugger::AllBreakpointValuesMsg all_bkpt_values;
  for (auto bkpt = breakpoint_list.begin();
        bkpt != breakpoint_list.end(); bkpt++) {
    all_bkpt_values.add_id_list(bkpt->getID());
    if (bkpt->temp) {
      all_bkpt_values.add_temporary("1");
    } else {
      all_bkpt_values.add_temporary("0");
    }
    if (bkpt->disabled) {
      all_bkpt_values.add_disabled("1");
    } else {
      all_bkpt_values.add_disabled("0");
    }
    PFPSimDebugger::AllBreakpointValuesMsg_BreakpointConditionList
          *bkpt_conditions = all_bkpt_values.add_breakpoint_condition_list();
    for (auto it = bkpt->conditions.begin();
          it != bkpt->conditions.end(); it++) {
      if (it->first == Breakpoint::BreakpointCondition::BREAK_ON_MODULE_READ) {
        bkpt_conditions->add_condition_list(
              PFPSimDebugger::BreakpointCondition::BREAK_ON_MODULE_READ);
      } else if (it->first
            == Breakpoint::BreakpointCondition::BREAK_ON_MODULE_WRITE) {
        bkpt_conditions->add_condition_list(
              PFPSimDebugger::BreakpointCondition::BREAK_ON_MODULE_WRITE);
      } else if (it->first
            == Breakpoint::BreakpointCondition::BREAK_ON_PACKET_ID) {
        bkpt_conditions->add_condition_list(
              PFPSimDebugger::BreakpointCondition::BREAK_ON_PACKET_ID);
      } else if (it->first == Breakpoint::BreakpointCondition::BREAK_AT_TIME) {
        bkpt_conditions->add_condition_list(
              PFPSimDebugger::BreakpointCondition::BREAK_AT_TIME);
      }
      bkpt_conditions->add_value_list(it->second);
    }
  }
  message.set_message(all_bkpt_values.SerializeAsString());
}

WhoAmIReplyMessage::WhoAmIReplyMessage(int packet_id)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_WhoAmIReply) {
  PFPSimDebugger::WhoAmIReplyMsg whoami_reply;
  whoami_reply.set_packet_id(packet_id);
  message.set_message(whoami_reply.SerializeAsString());
}

PacketListValuesMessage::PacketListValuesMessage(int *id_list,
      std::string *location_list, double *time_list, int num)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_PacketListValues) {
  PFPSimDebugger::PacketListValuesMsg packet_list_values;
  for (int i = 0; i < num; i++) {
    packet_list_values.add_id_list(id_list[i]);
    packet_list_values.add_location_list(location_list[i]);
    packet_list_values.add_time_list(time_list[i]);
  }
  message.set_message(packet_list_values.SerializeAsString());
}

WatchpointHitMessage::WatchpointHitMessage(int id, std::string counter_name,
      int old_value, int new_value)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_WatchpointHit) {
  PFPSimDebugger::WatchpointHitMsg watchpoint_hit;
  watchpoint_hit.set_id(id);
  watchpoint_hit.set_counter_name(counter_name);
  watchpoint_hit.set_old_value(old_value);
  watchpoint_hit.set_new_value(new_value);
  message.set_message(watchpoint_hit.SerializeAsString());
}

AllWatchpointValuesMessage::AllWatchpointValuesMessage(
      std::vector<Watchpoint> watchpoint_list)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_AllWatchpointValues) {
  PFPSimDebugger::AllWatchpointValuesMsg all_watchpoint_values;
  int num = watchpoint_list.size();
  for (int i = 0; i < num; i++) {
    all_watchpoint_values.add_id_list(watchpoint_list[i].getID());
    all_watchpoint_values.add_name_list(watchpoint_list[i].getCounterName());
    if (watchpoint_list[i].disabled) {
      all_watchpoint_values.add_disabled("1");
    } else {
      all_watchpoint_values.add_disabled("0");
    }
  }
  message.set_message(all_watchpoint_values.SerializeAsString());
}

BacktraceReplyMessage::BacktraceReplyMessage(int id,
      std::string *module_list, double *read_time_list,
      double *write_time_list, int num)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_BacktraceReply) {
  PFPSimDebugger::BacktraceReplyMsg backtrace_reply;
  backtrace_reply.set_packet_id(id);
  for (int i = 0; i < num; i++) {
    backtrace_reply.add_module_list(module_list[i]);
    backtrace_reply.add_read_time_list(read_time_list[i]);
    backtrace_reply.add_write_time_list(write_time_list[i]);
  }
  message.set_message(backtrace_reply.SerializeAsString());
}

SimulationEndMessage::SimulationEndMessage()
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_SimulationEnd) {
  PFPSimDebugger::SimulationEndMsg sim_end;
  message.set_message(sim_end.SerializeAsString());
}

SimulationStoppedMessage::SimulationStoppedMessage(
      std::string module, int packet_id, double time_ns, bool read)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_SimulationStopped) {
  PFPSimDebugger::SimulationStoppedMsg sim_stopped;
  sim_stopped.set_module(module);
  sim_stopped.set_packet_id(packet_id);
  sim_stopped.set_time(time_ns);
  sim_stopped.set_read(read);
  message.set_message(sim_stopped.SerializeAsString());
}

AllIgnoreModulesMessage::AllIgnoreModulesMessage(
      std::string *module_list, int num)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_AllIgnoreModules) {
  PFPSimDebugger::AllIgnoreModulesMsg all_ignore_modules;
  for (int i = 0; i < num; i++) {
    all_ignore_modules.add_module_list(module_list[i]);
  }
  message.set_message(all_ignore_modules.SerializeAsString());
}

SimulationTimeMessage::SimulationTimeMessage(double sim_time)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_SimulationTime) {
  PFPSimDebugger::SimulationTimeMsg sim_time_msg;
  sim_time_msg.set_time_ns(sim_time);
  message.set_message(sim_time_msg.SerializeAsString());
}

PacketDroppedMessage::PacketDroppedMessage(int id,
      std::string module, std::string reason)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_PacketDropped) {
  PFPSimDebugger::PacketDroppedMsg packet_dropped;
  packet_dropped.set_packet_id(id);
  packet_dropped.set_module(module);
  packet_dropped.set_reason(reason);
  message.set_message(packet_dropped.SerializeAsString());
}

DroppedPacketsMessage::DroppedPacketsMessage(int *id_list,
      std::string *module_list, std::string *reason_list, int num)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_DroppedPackets) {
  PFPSimDebugger::DroppedPacketsMsg dropped_packets;
  for (int i = 0; i < num; i++) {
    dropped_packets.add_packet_id_list(id_list[i]);
    dropped_packets.add_module_list(module_list[i]);
    dropped_packets.add_reason_list(reason_list[i]);
  }
  message.set_message(dropped_packets.SerializeAsString());
}

TableEntriesMessage::TableEntriesMessage(
      std::vector<CPDebuggerInterface::TableEntry> table_entries)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_TableEntries) {
  PFPSimDebugger::TableEntriesMsg msg;
  for (auto it = table_entries.begin(); it != table_entries.end(); it++) {
    PFPSimDebugger::TableEntriesMsg_TableEntry *entry = msg.add_entry_list();
    entry->set_table_name(it->table_name);
    entry->set_action_name(it->action_name);
    entry->set_handle(it->handle);
    PFPSimDebugger::TableEntriesMsg_TableEntryStatus status
          = PFPSimDebugger::TableEntriesMsg_TableEntryStatus_NONE;
    switch (it->status) {
      case CPDebuggerInterface::TableEntryStatus::OK:
      {
        status = PFPSimDebugger::TableEntriesMsg_TableEntryStatus_OK;
        break;
      }
      case CPDebuggerInterface::TableEntryStatus::INSERTING:
      {
        status = PFPSimDebugger::TableEntriesMsg_TableEntryStatus_INSERTING;
        break;
      }
      case CPDebuggerInterface::TableEntryStatus::DELETING:
      {
        status = PFPSimDebugger::TableEntriesMsg_TableEntryStatus_DELETING;
        break;
      }
      case CPDebuggerInterface::TableEntryStatus::MODIFYING:
      {
        status = PFPSimDebugger::TableEntriesMsg_TableEntryStatus_MODIFYING;
        break;
      }
      default:
      {
        break;
      }
    }
    entry->set_status(status);
    entry->add_match_key_list(it->match_key);
    for (auto data = it->action_data.begin();
          data != it->action_data.end(); data++) {
      entry->add_action_data_list(*data);
    }
  }

  message.set_message(msg.SerializeAsString());
}

RawPacketValueMessage::RawPacketValueMessage(const std::vector<uint8_t> & data)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_RawPacketValue) {

  PFPSimDebugger::RawPacketValueMsg msg;

  msg.set_value(data.data(), data.size());

  message.set_message(msg.SerializeAsString());

}

ParsedPacketValueMessage::ParsedPacketValueMessage(
    const std::vector<DebugInfo::Header> & headers)
      : DebuggerMessage(PFPSimDebugger::DebugMsg_Type_ParsedPacketValue) {

  PFPSimDebugger::ParsedPacketValueMsg msg;

  for (auto & h : headers) {
    auto pb_h = msg.add_headers();
    pb_h->set_name(h.name);

    for (auto & f : h.fields) {
      auto pb_f = pb_h->add_fields();

      pb_f->set_name(f.name);
      pb_f->set_value(f.value.data(), f.value.size());
    }
  }

  message.set_message(msg.SerializeAsString());

}


};  // namespace db
};  // namespace core
};  // namespace pfp
