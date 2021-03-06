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

#include "DebugDataManager.h"
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <tuple>

namespace pfp {
namespace core {
namespace db {

DebugDataManager::DebugDataManager()
      : trace_id(0),
      simulation_time(0.0),
      current_packet_id(-1),
      next_watchpoint_id(0),
      break_packet_dropped(false) {}

void DebugDataManager::addCounter(std::string name) {
  std::lock_guard<std::mutex> guard(mutex_);
  counters.insert(std::pair<std::string, int>(name, 0));
}

void DebugDataManager::removeCounter(std::string name) {
  std::lock_guard<std::mutex> guard(mutex_);
  counters.erase(name);
}

int DebugDataManager::addCounterTrace(const std::string & name) {
  std::lock_guard<std::mutex> guard(mutex_);

  // If there's no counter with this name, then we can't set a trace
  if (counters.find(name) == counters.end()) {
    return -1;
  }

  // If a trace already exists, we won't make a new one.
  if (counter_traces.find(name) != counter_traces.end()) {
    return -2;
  }

  // Creating the trace itself is really as simple as assigning it
  // an ID.
  int id = trace_id++;

  counter_traces[name] = id;

  return id;
}

int DebugDataManager::addLatencyTrace(const std::string & from_module_name,
                                      const std::string & to_module_name) {
  std::lock_guard<std::mutex> guard(mutex_);

  int id = trace_id++;

  latency_read_triggers[from_module_name].push_back({id, to_module_name});

  return id;
}

int DebugDataManager::addThroughputTrace(const std::string & module_name) {
  std::lock_guard<std::mutex> guard(mutex_);

  int id = trace_id++;

  throughput_triggers[module_name] = {id, -1};

  return id;
}

int DebugDataManager::getCounterTraceId(const std::string & name) {
  auto it = counter_traces.find(name);

  if (it == counter_traces.end()) {
    return -1;
  } else {
    return it->second;
  }
}

void DebugDataManager::updateCounter(std::string name, int val) {
  std::lock_guard<std::mutex> guard(mutex_);
  auto it = counters.find(name);
  if (it != counters.end()) {
    counters[name] = val;
  } else {
    // TODO(eric): Make this nicer
    throw "COUNTER DOES NOT EXIST";
  }
}

int DebugDataManager::getCounterValue(std::string name) {
  std::lock_guard<std::mutex> guard(mutex_);
  auto it = counters.find(name);
  if (it != counters.end()) {
    return it->second;
  } else {
    return -1;
  }
}

std::map<std::string, int> DebugDataManager::getCounters() {
  std::lock_guard<std::mutex> guard(mutex_);
  return counters;
}

void DebugDataManager::addBreakpoint(Breakpoint br) {
  std::lock_guard<std::mutex> guard(mutex_);
  for (auto bkpt = breakpoint_list.begin();
        bkpt != breakpoint_list.end(); bkpt++) {
    if (bkpt->isEqual(br)) {
      return;
    }
  }
  breakpoint_list.push_back(br);
}

void DebugDataManager::removeBreakpoint(int identifier) {
  std::lock_guard<std::mutex> guard(mutex_);
  for (auto bkpt = breakpoint_list.begin();
        bkpt != breakpoint_list.end(); bkpt++) {
    if (bkpt->getID() == identifier) {
      breakpoint_list.erase(bkpt);
      break;
    }
  }
}

void DebugDataManager::enableBreakpoint(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto bkpt = breakpoint_list.begin();
        bkpt != breakpoint_list.end(); bkpt++) {
    if (bkpt->getID() == id) {
      bkpt->disabled = false;
      break;
    }
  }
}

void DebugDataManager::disableBreakpoint(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto bkpt = breakpoint_list.begin();
        bkpt != breakpoint_list.end(); bkpt++) {
    if (bkpt->getID() == id) {
      bkpt->disabled = true;
      break;
    }
  }
}



std::vector<DebugDataManager::TraceData>
DebugDataManager::updatePacket(int id,
                               std::shared_ptr<const DebugInfo> di,
                               std::string module, double time_,
                               bool read) {
  std::lock_guard<std::mutex> guard(mutex_);

  auto check_pk = packet_list.find(id);
  if (check_pk == packet_list.end()) {
    DebuggerPacket pk(id, module, time_);
    packet_list[id] = pk;
  }

  DebuggerPacket& packet = packet_list.at(id);

  packet.setDebugInfo(di);

  if (read) {
    packet.setTime(time_);
    packet.setCurrentLocation(module);
    packet.updateTraceReadTime(module, time_);

    auto it = latency_read_triggers.find(module);
    if (it != latency_read_triggers.end()) {
      // For each latency trace triggered by this read:
      for (auto & trigger : it->second) {
        // We store a trigger for the write-module of that latency trace,
        // associated with the packet id, and the trace id and read timestamp.
        // Multiple latency traces can end at the same module, which is why we
        // push back instead of just setting a single entry
        latency_write_triggers[trigger.write_module_name][id].push_back(
            {trigger.trace_id, time_});
      }
    }
    // No new trace updates;
    return {};

  } else {
    packet.updateTraceWriteTime(module, time_);

    std::vector<DebugDataManager::TraceData> trace_updates;

    // Latency Updates
    // TODO(gordon) factor out into function
    {
      auto it = latency_write_triggers.find(module);
      if (it != latency_write_triggers.end()) {
        // And if any of those correspond to this particular packet
        auto pack_it = it->second.find(id);
        if (pack_it != it->second.end()) {
          // Then for each of those triggers
          for (auto & trigger : pack_it->second) {
            // We update it's associated trace with the difference between the
            // read time and the write time.
            trace_updates.push_back({trigger.trace_id,
                                     time_ - trigger.read_time});
          }
          // Then we delete the set of triggers for this packet ID
          it->second.erase(pack_it);
          // We don't bother deleting the outer map entry, because if writes
          // to this module have been part of any trace, they likely will be
          // again, and there's no point in deleting the map if it's likely
          // just going to be recreated again. On the other hand, once we've
          // seen a particular packet id, we don't expect to ever see it again.
        }
      }
    }

    // Throughput updates
    // TODO(gordon) factor out into function
    {
      auto it = throughput_triggers.find(module);
      if (it != throughput_triggers.end()) {
        double & last_time = it->second.last_time;
        if (last_time >= 0) {
          // Divide time delta by 1e9 to get time in seconds, then take the
          // reciprocal to get packets per second
          trace_updates.push_back({it->second.trace_id,
                                   (1000*1000*1000)/(time_ - last_time)});
        }
        last_time = time_;
      }
    }

    return trace_updates;
  }
}

void DebugDataManager::removePacket(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  packet_list.erase(id);
}

void DebugDataManager::addWatchpoint(Watchpoint wp) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto it = watchpoint_list.begin(); it != watchpoint_list.end(); it++) {
    if (it->getCounterName() == wp.getCounterName()) {
      return;
    }
  }
  watchpoint_list.push_back(wp);
}

void DebugDataManager::removeWatchpoint(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto it = watchpoint_list.begin(); it != watchpoint_list.end(); it++) {
    if (it->getID() == id) {
      watchpoint_list.erase(it);
      break;
    }
  }
}

void DebugDataManager::enableWatchpoint(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto it = watchpoint_list.begin(); it != watchpoint_list.end(); it++) {
    if (it->getID() == id) {
      it->disabled = false;
      break;
    }
  }
}

void DebugDataManager::disableWatchpoint(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  // TODO(gordon) Don't do a linear search!
  for (auto it = watchpoint_list.begin(); it != watchpoint_list.end(); it++) {
    if (it->getID() == id) {
      it->disabled = true;
      break;
    }
  }
}

int DebugDataManager::whoami() {
  std::lock_guard<std::mutex> guard(mutex_);
  return current_packet_id;
}
void DebugDataManager::set_whoami(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  current_packet_id = id;
}

void DebugDataManager::addIgnoreModule(std::string mod) {
  std::lock_guard<std::mutex> guard(mutex_);
  for (auto it = ignore_module_list.begin();
        it != ignore_module_list.end(); it++) {
    if (mod == *it) {
      return;
    }
  }
  ignore_module_list.push_back(mod);
}

bool DebugDataManager::checkIgnoreModules(std::string mod) {
  std::lock_guard<std::mutex> guard(mutex_);
  for (auto it = ignore_module_list.begin();
        it != ignore_module_list.end(); it++) {
    if (*it == mod) {
      return true;
    }
  }
  return false;
}

void DebugDataManager::removeIgnoreModule(std::string mod) {
  std::lock_guard<std::mutex> guard(mutex_);
  for (auto it = ignore_module_list.begin();
        it != ignore_module_list.end(); it++) {
    if (*it == mod) {
      ignore_module_list.erase(it);
      break;
    }
  }
}

void DebugDataManager::addDroppedPacket(int id, std::string module,
      std::string reason) {
  std::lock_guard<std::mutex> guard(mutex_);
  dropped_packet_list.push_back(
        std::tuple<int, std::string, std::string>(id, module, reason));
}

void DebugDataManager::setBreakOnPacketDrop(bool b) {
  std::lock_guard<std::mutex> guard(mutex_);
  break_packet_dropped = b;
}

bool DebugDataManager::getBreakOnPacketDrop() {
  std::lock_guard<std::mutex> guard(mutex_);
  return break_packet_dropped;
}

Breakpoint DebugDataManager::getBreakpoint(int index) {
  std::lock_guard<std::mutex> guard(mutex_);
  return breakpoint_list[index];
}

std::vector<Breakpoint>& DebugDataManager::getBreakpointList() {
  std::lock_guard<std::mutex> guard(mutex_);
  return breakpoint_list;
}

int DebugDataManager::getNumberOfBreakpoints() {
  std::lock_guard<std::mutex> guard(mutex_);
  return breakpoint_list.size();
}

double DebugDataManager::getSimulationTime() {
  std::lock_guard<std::mutex> guard(mutex_);
  return simulation_time;
}

DebuggerPacket* DebugDataManager::getPacket(int id) {
  std::lock_guard<std::mutex> guard(mutex_);
  try {
    return &packet_list.at(id);
  } catch (std::out_of_range e) {
    return NULL;
  }
}

std::map<int, DebuggerPacket>& DebugDataManager::getPacketList() {
  std::lock_guard<std::mutex> guard(mutex_);
  return packet_list;
}

std::vector<Watchpoint>& DebugDataManager::getWatchpointList() {
  std::lock_guard<std::mutex> guard(mutex_);
  return watchpoint_list;
}

std::vector<std::string>& DebugDataManager::getIgnoreModuleList() {
  std::lock_guard<std::mutex> guard(mutex_);
  return ignore_module_list;
}

std::vector<std::tuple<int, std::string, std::string>>&
DebugDataManager::getDroppedPacketList() {
  std::lock_guard<std::mutex> guard(mutex_);
  return dropped_packet_list;
}

void DebugDataManager::setSimulationTime(double time_ns) {
  std::lock_guard<std::mutex> guard(mutex_);
  simulation_time = time_ns;
}

};  // namespace db
};  // namespace core
};  // namespace pfp
