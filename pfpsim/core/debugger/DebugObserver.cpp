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

#include "DebugObserver.h"
#include <iostream>
#include <string>
#include <vector>
#include "Breakpoint.h"
#include "Watchpoint.h"
#include "../PacketBase.h"

namespace pfp {
namespace core {
namespace db {

DebugObserver::DebugObserver() {
  // create data manager
  data_manager = new DebugDataManager();

  // start ipc server
  ipc_server
        = new DebuggerIPCServer("ipc:///tmp/pfpsimdebug.ipc", data_manager);
  ipc_server->start();
}

DebugObserver::~DebugObserver() {
  while (enable) {
    SimulationEndMessage *msg = new SimulationEndMessage();
    ipc_server->setReplyMessage(msg);
    notifyServer();
    pause();
  }
  delete ipc_server;
}

void DebugObserver::waitForRunCommand() {
  ipc_server->waitForRunCommand();
}

void DebugObserver::counter_added(const std::string& module_name,
      const std::string& counter_name, double simulation_time) {
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Counter Added: " << module_name << ": "
          << counter_name << " @ " << simulation_time << std::endl;
  }

  data_manager->addCounter(counter_name);
  updateSimulationTime(simulation_time);
}

void DebugObserver::counter_removed(const std::string& module_name,
      const std::string& counter_name, double simulation_time) {
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Counter Removed: " << module_name << ": "
          << counter_name << " @ " << simulation_time << std::endl;
  }

  data_manager->removeCounter(counter_name);
  updateSimulationTime(simulation_time);
}

void DebugObserver::counter_updated(const std::string& module_name,
      const std::string& counter_name,
      std::size_t new_value,
      double simulation_time) {
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Counter Updated: " << module_name << ": "
        << counter_name << ": " << new_value << " @ "
        << simulation_time << std::endl;
  }
  int old_value = data_manager->getCounterValue(counter_name);
  if (old_value == -1) {
    old_value = 0;
  }

  int trace_id = data_manager->getCounterTraceId(counter_name);
  if (trace_id >= 0) {
    ipc_server->updateTrace(trace_id, new_value);
  }

  data_manager->updateCounter(counter_name, static_cast<int>(new_value));
  updateSimulationTime(simulation_time);
  std::vector<Watchpoint>& watchpoints = data_manager->getWatchpointList();
  for (auto it = watchpoints.begin(); it != watchpoints.end(); it++) {
    if (counter_name == it->getCounterName() && !it->disabled) {
      WatchpointHitMessage *watchpoint_hit_msg
          = new WatchpointHitMessage(
                it->getID(), counter_name, old_value, new_value);
      ipc_server->setReplyMessage(watchpoint_hit_msg);
      notifyServer();
      pause();
    }
  }
}

void DebugObserver::data_written(const std::string& from_module,
      const std::shared_ptr<TrType> data, double simulation_time) {
  if (!data->debuggable()) {
    return;
  }
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Data Written" << " @ " << simulation_time
          << " from " << from_module << ":\nType: " << data->data_type()
          << "\nPacket ID: " << data->id() << std::endl;
  }
  updateSimulationTime(simulation_time);

  data_manager->updatePacket(data->id(), data->debug_info(),
                             from_module, simulation_time, false);

  if (!data_manager->checkIgnoreModules(from_module)) {
    checkBreakpointHit(from_module, data->id(), simulation_time, false);
  }
}

void DebugObserver::data_read(const std::string& to_module,
      const std::shared_ptr<TrType> data, double simulation_time) {
  if (!data->debuggable()) {
    return;
  }

  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Data Read" << " @ "
          << std::fixed << simulation_time << " ns to " << to_module
          << ":\nType: " << data->data_type()
          << "\nPacket ID: " << data->id() << std::endl;
  }

  updateSimulationTime(simulation_time);
  data_manager->updatePacket(data->id(), data->debug_info(),
                             to_module, simulation_time, false);

  if (!data_manager->checkIgnoreModules(to_module)) {
    checkBreakpointHit(to_module, data->id(), simulation_time, true);
  }
}

void DebugObserver::data_dropped(const std::string& in_module,
      const std::shared_ptr<TrType> data,
      const std::string& drop_reason,
      double simulation_time) {
  if (!data->debuggable()) {
    return;
  }

  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Data Dropped " << "@ " << simulation_time
          << " in " << in_module << ":\nType: " << data->data_type()
          << "\nPacket ID: " << data->id() << "\nSource: " << std::endl;
  }

  updateSimulationTime(simulation_time);

  data_manager->addDroppedPacket(data->id(), in_module, drop_reason);

  if (data_manager->getBreakOnPacketDrop()) {
    PacketDroppedMessage *msg
          = new PacketDroppedMessage(data->id(), in_module, drop_reason);
    ipc_server->setReplyMessage(msg);
    notifyServer();
    pause();
  }
}

void DebugObserver::thread_begin(const std::string& teu_mod,
      const std::string& tec_mod, std::size_t thread_id,
      std::size_t packet_id, double simulation_time) {
    if (VERBOSE) {
      std::cout << "DEBUGOBS: " << "Thread Started: " << teu_mod << ": "
            << tec_mod << ": " << thread_id << ": " << packet_id << " @ "
            << simulation_time << std::endl;
    }
    updateSimulationTime(simulation_time);
}

void DebugObserver::thread_end(const std::string& teu_mod,
      const std::string& tec_mod, std::size_t thread_id,
      std::size_t packet_id, double simulation_time) {
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Thread Ended: " << teu_mod << ": "
          << tec_mod << ": " << thread_id << ": " << packet_id
          << " @ " << simulation_time << std::endl;
  }
  updateSimulationTime(simulation_time);
}

void DebugObserver::thread_idle(const std::string& teu_mod,
      const std::string& tec_mod, std::size_t thread_id,
      std::size_t packet_id, double simulation_time) {
  if (VERBOSE) {
    std::cout << "DEBUGOBS: " << "Thread Idle: " << teu_mod << ": "
          << tec_mod << ": " << thread_id << ": " << packet_id
          << " @ " << simulation_time << std::endl;
  }
  updateSimulationTime(simulation_time);
}

void DebugObserver::core_busy(const std::string& teu_mod,
      const std::string& tec_mod, double simulation_time) {
  updateSimulationTime(simulation_time);
}

void DebugObserver::core_idle(const std::string& teu_mod,
      const std::string& tec_mod, double simulation_time) {
  updateSimulationTime(simulation_time);
}

void DebugObserver::enableDebugger() {
  enable = true;
}

void DebugObserver::pause() {
  std::mutex& m = ipc_server->getBkptMutex();
  std::condition_variable& cv = ipc_server->getBkptConditionVariable();
  std::unique_lock<std::mutex> lock(m);
  while (!ipc_server->getContinueCommand()) {
    cv.wait(lock);
  }
  ipc_server->setContinueCommand(false);
  lock.unlock();
}

void DebugObserver::notifyServer() {
  std::mutex& m = ipc_server->getStopMutex();
  std::condition_variable& cv = ipc_server->getStopConditionVariable();
  std::unique_lock<std::mutex> lock(m);
  ipc_server->setStopped(true);
  lock.unlock();
  cv.notify_all();
}

void DebugObserver::updateSimulationTime(double sim_time) {
  if (sim_time > data_manager->getSimulationTime()) {
    data_manager->setSimulationTime(sim_time);
  }
}

void DebugObserver::updateWhoAmI(int packet_id) {
  data_manager->set_whoami(packet_id);
}

void DebugObserver::checkBreakpointHit(std::string module, int packet_id,
      double sim_time, bool read) {
  bool break_hit = true;
  std::vector<Breakpoint>& breakpoints = data_manager->getBreakpointList();
  if (breakpoints.size() == 0) {
    break_hit = false;
  }
  Breakpoint *hit_bkpt = NULL;
  for (auto bkpt = breakpoints.begin(); bkpt != breakpoints.end(); bkpt++) {
    for (auto cond = bkpt->conditions.begin();
          cond != bkpt->conditions.end(); cond++) {
      if (cond->first == Breakpoint::BreakpointCondition::BREAK_ON_MODULE_READ
              && (!read || cond->second != module)) {
        break_hit = false;
        break;
      } else if (cond->first
              == Breakpoint::BreakpointCondition::BREAK_ON_MODULE_WRITE
              && (read || cond->second != module)) {
        break_hit = false;
        break;
      } else if (cond->first
              == Breakpoint::BreakpointCondition::BREAK_ON_PACKET_ID
              && cond->second != std::to_string(packet_id)) {
        break_hit = false;
        break;
      } else if (cond->first
              == Breakpoint::BreakpointCondition::BREAK_AT_TIME
              && stof(cond->second) > sim_time) {
        break_hit = false;
        break;
      }
    }
    if (break_hit == true) {
      hit_bkpt = &(*bkpt);
      break;
    } else {
      break_hit = true;
    }
  }

  if (hit_bkpt && hit_bkpt->disabled == false) {
    updateWhoAmI(packet_id);
    // Check if its a stealth breakpoint
    if (hit_bkpt->getID() != -1) {
      BreakpointHitMessage *bkpt_hit_msg = new BreakpointHitMessage(
              hit_bkpt->getID(), module, packet_id, sim_time, read);
      ipc_server->setReplyMessage(bkpt_hit_msg);
      data_manager->removeBreakpoint(-1);
    } else {
      SimulationStoppedMessage *sim_stopped_msg
            = new SimulationStoppedMessage(module, packet_id, sim_time, read);
      ipc_server->setReplyMessage(sim_stopped_msg);
    }
    // remove bkpt from list if temporary
    if (hit_bkpt->temp == true) {
      data_manager->removeBreakpoint(hit_bkpt->getID());
    }
    notifyServer();
    pause();
  }
}

void DebugObserver::registerCP(CPDebuggerInterface *cp_debug_if) {
  ipc_server->registerCP(cp_debug_if);
}

};  // namespace db
};  // namespace core
};  // namespace pfp
