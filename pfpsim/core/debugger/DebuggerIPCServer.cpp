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

#include "DebuggerIPCServer.h"
#include <signal.h>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include "Breakpoint.h"
#include "Watchpoint.h"
#include "DebuggerPacket.h"

namespace pfp {
namespace core {
namespace db {

DebuggerIPCServer::DebuggerIPCServer(std::string url, DebugDataManager *dm)
      : data_manager(dm), reply_message(NULL) {
  socket = nn_socket(AF_SP, NN_REP);
  nn_bind(socket, url.c_str());
}

DebuggerIPCServer::~DebuggerIPCServer() {
  delete ulock;
  kill_thread = true;
  nn_shutdown(socket, 0);
  debug_req_thread.join();
}

void DebuggerIPCServer::start() {
  ulock = new std::unique_lock<std::mutex>(start_mutex);
  debug_req_thread = std::thread(&DebuggerIPCServer::requestThread, this);
}

void DebuggerIPCServer::waitForRunCommand() {
  std::lock_guard<std::mutex> guard(start_mutex);
}

std::mutex& DebuggerIPCServer::getBkptMutex() {
  return bkpt_mutex;
}

std::mutex& DebuggerIPCServer::getStopMutex() {
  return stop_mutex;
}

std::condition_variable& DebuggerIPCServer::getBkptConditionVariable() {
  return bkpt_cv;
}

std::condition_variable& DebuggerIPCServer::getStopConditionVariable() {
  return stop_cv;
}

bool DebuggerIPCServer::getContinueCommand() const {
  return continue_command;
}

void DebuggerIPCServer::setContinueCommand(bool b) {
  continue_command = b;
}

bool DebuggerIPCServer::getStopped() const {
  return stopped;
}

void DebuggerIPCServer::setStopped(bool b) {
  stopped = b;
}

void DebuggerIPCServer::setReplyMessage(DebuggerMessage *message) {
  reply_message = message;
}

void DebuggerIPCServer::registerCP(CPDebuggerInterface *cp_debug_if) {
  control_plane = cp_debug_if;
}

void DebuggerIPCServer::send(DebuggerMessage *message) {
  std::string message_string;
  message->SerializeToString(&message_string);
  int bytes_sent = nn_send(socket, message_string.c_str(),
        message_string.size(), 0);
}

PFPSimDebugger::DebugMsg* DebuggerIPCServer::recv() {
  struct nn_pollfd pfd;
  pfd.fd     = socket;
  pfd.events = NN_POLLIN;

  auto rc = nn_poll(&pfd, 1, 100);
  if (rc == 0) {
    return nullptr;
  }

  if (rc == -1) {
    // TODO(gordon)
    printf("Error!");
    exit(1);
  }

  if (pfd.revents & NN_POLLIN) {
    char *buf;
    int bytes = nn_recv(socket, &buf, NN_MSG, 0);
    if (bytes != -1) {
      std::string message_string(buf);
      nn_freemsg(buf);

      PFPSimDebugger::DebugMsg *message = new PFPSimDebugger::DebugMsg();
      message->ParseFromString(message_string);
      return message;
    } else {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}

void DebuggerIPCServer::parseRequest(PFPSimDebugger::DebugMsg *request) {
  switch (request->type()) {
    case PFPSimDebugger::DebugMsg_Type_Run:
    {
      if (!run_called) {
        run_called = true;
        PFPSimDebugger::RunMsg msg;
        msg.ParseFromString(request->message());
        if (msg.has_time_ns()) {
          handleRun(stod(msg.time_ns()));
        } else {
          handleRun();
        }
      } else {
      // TODO(eric): this is a temporary fix for the double run message problem
        SimulationEndMessage *msg = new SimulationEndMessage();
        send(msg);
        delete msg;
      }
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetCounter:
    {
      PFPSimDebugger::GetCounterMsg msg;
      msg.ParseFromString(request->message());
      handleGetCounter(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetAllCounters:
    {
      handleGetAllCounters();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_SetBreakpoint:
    {
      PFPSimDebugger::SetBreakpointMsg msg;
      msg.ParseFromString(request->message());
      handleSetBreakpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_Continue:
    {
      PFPSimDebugger::ContinueMsg msg;
      msg.ParseFromString(request->message());
      if (msg.has_time_ns()) {
        handleContinue(stod(msg.time_ns()));  // alternative to stod?
      } else {
        handleContinue();
      }
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetAllBreakpoints:
    {
      handleGetAllBreakpoints();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_RemoveBreakpoint:
    {
      PFPSimDebugger::RemoveBreakpointMsg msg;
      msg.ParseFromString(request->message());
      handleRemoveBreakpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_WhoAmI:
    {
      handleWhoAmI();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_Next:
    {
      handleNext();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetPacketList:
    {
      PFPSimDebugger::GetPacketListMsg msg;
      msg.ParseFromString(request->message());
      handleGetPacketList(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_SetWatchpoint:
    {
      PFPSimDebugger::SetWatchpointMsg msg;
      msg.ParseFromString(request->message());
      handleSetWatchpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetAllWatchpoints:
    {
      handleGetAllWatchpoints();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_RemoveWatchpoint:
    {
      PFPSimDebugger::RemoveWatchpointMsg msg;
      msg.ParseFromString(request->message());
      handleRemoveWatchpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_Backtrace:
    {
      PFPSimDebugger::BacktraceMsg msg;
      msg.ParseFromString(request->message());
      handleBacktrace(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_EnableDisableBreakpoint:
    {
      PFPSimDebugger::EnableDisableBreakpointMsg msg;
      msg.ParseFromString(request->message());
      handleEnableDisableBreakpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_EnableDisableWatchpoint:
    {
      PFPSimDebugger::EnableDisableWatchpointMsg msg;
      msg.ParseFromString(request->message());
      handleEnableDisableWatchpoint(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_IgnoreModule:
    {
      PFPSimDebugger::IgnoreModuleMsg msg;
      msg.ParseFromString(request->message());
      handleIgnoreModule(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetAllIgnoreModules:
    {
      handleGetAllIgnoreModules();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetSimulationTime:
    {
      handleGetSimulationTime();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_BreakOnPacketDrop:
    {
      PFPSimDebugger::BreakOnPacketDropMsg msg;
      msg.ParseFromString(request->message());
      handleBreakOnPacketDrop(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetDroppedPackets:
    {
      handleGetDroppedPackets();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_CPCommand:
    {
      PFPSimDebugger::CPCommandMsg msg;
      msg.ParseFromString(request->message());
      handleCPCommand(msg);
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetTableEntries:
    {
      handleGetTableEntries();
      break;
    }
    case PFPSimDebugger::DebugMsg_Type_GetParsedPacket:
    {
      PFPSimDebugger::GetParsedPacketMsg msg;
      msg.ParseFromString(request->message());
      handleGetParsedPacket(msg.id());
      break;
    }
    default: {
      sendRequestFailed();
    }
  }
}

void DebuggerIPCServer::requestThread() {
  while (!kill_thread) {
    // non-blocking (timeout after 100ms)
    PFPSimDebugger::DebugMsg *request = recv();
    if (request) {
      // std::cout << "Msg Received!" << std::endl;
      parseRequest(request);
      delete request;
    }
  }
}

void DebuggerIPCServer::waitForStop() {
  std::unique_lock<std::mutex> lock(stop_mutex);
  while (!stopped) {
    stop_cv.wait(lock);
  }
  stopped = false;
  lock.unlock();
}

void DebuggerIPCServer::handleRun(double time_ns) {
  if (time_ns != -1) {
    double current_sim_time = data_manager->getSimulationTime();
    double break_time = current_sim_time + time_ns;
    Breakpoint bkpt(true);
    bkpt.temp = true;
    bkpt.addCondition(Breakpoint::BreakpointCondition::BREAK_AT_TIME,
          std::to_string(break_time));
    data_manager->addBreakpoint(bkpt);
  }
  ulock->unlock();  // release mutex so that program can start
  waitForStop();
  DebuggerMessage *reply;
  if (reply_message == NULL) {
    reply = new GenericAcknowledgeMessage(
          PFPSimDebugger::GenericAcknowledgeMsg_Status_SUCCESS);
  } else {
    reply = reply_message;
  }
  send(reply);
  delete reply;
}

void DebuggerIPCServer::handleContinue(double time_ns) {
  if (time_ns != -1) {
    double current_sim_time = data_manager->getSimulationTime();
    double break_time = current_sim_time + time_ns;
    Breakpoint bkpt(true);
    bkpt.temp = true;
    bkpt.addCondition(Breakpoint::BreakpointCondition::BREAK_AT_TIME,
          std::to_string(break_time));
    data_manager->addBreakpoint(bkpt);
  }
  std::unique_lock<std::mutex> lock(bkpt_mutex);
  continue_command = true;
  lock.unlock();
  bkpt_cv.notify_all();
  waitForStop();
  DebuggerMessage *reply;
  if (reply_message == NULL) {
    reply = new GenericAcknowledgeMessage(
          PFPSimDebugger::GenericAcknowledgeMsg_Status_SUCCESS);
  } else {
    reply = reply_message;
  }
  send(reply);
  delete reply;
}

void DebuggerIPCServer::handleGetAllCounters() {
  std::map<std::string, int> counters = data_manager->getCounters();
  const int num = counters.size();
  std::vector<std::string> names(num);
  std::vector<int> values(num);
  int i = 0;
  for (auto it = counters.begin(); it != counters.end(); it++) {
    names[i] = it->first;
    values[i] = it->second;
    i++;
  }
  AllCounterValuesMessage *message
        = new AllCounterValuesMessage(names.data(), values.data(), num);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleGetCounter(PFPSimDebugger::GetCounterMsg msg) {
  std::string name = msg.name();
  int value = data_manager->getCounterValue(name);
  CounterValueMessage *message = new CounterValueMessage(name, value);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleSetBreakpoint(
      PFPSimDebugger::SetBreakpointMsg msg) {
  int size = msg.condition_list_size();
  Breakpoint br;
  if (msg.temporary() == "1") {
    br.temp = true;
  }
  if (msg.disabled() == "1") {
    br.disabled = true;
  } for (int i = 0; i < size; i++) {
    PFPSimDebugger::BreakpointCondition condition = msg.condition_list(i);
    std::string value = msg.value_list(i);
    if (condition
          == PFPSimDebugger::BreakpointCondition::BREAK_ON_MODULE_READ) {
      br.addCondition(
            Breakpoint::BreakpointCondition::BREAK_ON_MODULE_READ, value);
    } else if (condition
          == PFPSimDebugger::BreakpointCondition::BREAK_ON_MODULE_WRITE) {
      br.addCondition(
            Breakpoint::BreakpointCondition::BREAK_ON_MODULE_WRITE, value);
    } else if (condition
          == PFPSimDebugger::BreakpointCondition::BREAK_ON_PACKET_ID) {
      br.addCondition(
            Breakpoint::BreakpointCondition::BREAK_ON_PACKET_ID, value);
    } else if (condition
          == PFPSimDebugger::BreakpointCondition::BREAK_AT_TIME) {
      br.addCondition(Breakpoint::BreakpointCondition::BREAK_AT_TIME, value);
      br.temp = true;
    }
  }
  data_manager->addBreakpoint(br);
  sendGenericReply();
}

void DebuggerIPCServer::handleGetAllBreakpoints() {
  std::vector<Breakpoint> breakpoints = data_manager->getBreakpointList();
  AllBreakpointValuesMessage *message
        = new AllBreakpointValuesMessage(breakpoints);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleRemoveBreakpoint(
      PFPSimDebugger::RemoveBreakpointMsg msg) {
  int id = stoi(msg.id());
  data_manager->removeBreakpoint(id);
  sendGenericReply();
}

void DebuggerIPCServer::handleWhoAmI() {
  int id = data_manager->whoami();
  WhoAmIReplyMessage *message = new WhoAmIReplyMessage(id);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleNext() {
  int id = data_manager->whoami();
  Breakpoint bkpt(true);
  bkpt.addCondition(Breakpoint::BreakpointCondition::BREAK_ON_PACKET_ID,
        std::to_string(id));
  bkpt.temp = true;
  data_manager->addBreakpoint(bkpt);
  // continue simulation
  handleContinue(-1);
}

void DebuggerIPCServer::handleGetPacketList(
      PFPSimDebugger::GetPacketListMsg msg) {
  std::string module = "";
  if (msg.has_module()) {
    module = msg.module();
  }
  std::map<int, DebuggerPacket> packets = data_manager->getPacketList();
  std::vector<int> ids;
  std::vector<std::string> locations;
  std::vector<double> times;
  int counter = 0;
  for (auto it = packets.begin(); it != packets.end(); it++) {
    DebuggerPacket pk = it->second;
    if ((module == "" || pk.getLocation() == module)
          && !data_manager->checkIgnoreModules(pk.getLocation())) {
      ids.push_back(pk.getID());
      locations.push_back(pk.getLocation());
      times.push_back(pk.getTime());
      counter++;
    }
  }
  PacketListValuesMessage *message = new PacketListValuesMessage(ids.data(),
        locations.data(), times.data(), counter);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleSetWatchpoint(
      PFPSimDebugger::SetWatchpointMsg msg) {
  std::string counter_name = msg.counter_name();
  bool disabled = false;
  if (msg.disabled() == "1") {
    disabled = true;
  }
  Watchpoint wp(counter_name, disabled);
  data_manager->addWatchpoint(wp);
  sendGenericReply();
}

void DebuggerIPCServer::handleGetAllWatchpoints() {
  std::vector<Watchpoint>& watchpoints = data_manager->getWatchpointList();
  AllWatchpointValuesMessage *message
        = new AllWatchpointValuesMessage(watchpoints);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleRemoveWatchpoint(
      PFPSimDebugger::RemoveWatchpointMsg msg) {
  int id = stoi(msg.id());
  data_manager->removeWatchpoint(id);
  sendGenericReply();
}

void DebuggerIPCServer::handleBacktrace(PFPSimDebugger::BacktraceMsg msg) {
  int id;
  if (msg.has_packet_id()) {
    try {
      id = stoi(msg.packet_id());
    } catch (const std::exception& ex) {
      std::cout << ex.what() << std::endl;
      sendRequestFailed();
    }
  } else {
    id = data_manager->whoami();
  }
  DebuggerPacket *pk = data_manager->getPacket(id);
  if (pk != NULL) {
    std::vector<DebuggerPacket::PacketLocation> trace = pk->getTrace();
    const int size = trace.size();
    std::vector<std::string> modules(size);
    std::vector<double> read_times(size);
    std::vector<double> write_times(size);
    for (int i = 0; i < size; i++) {
      modules[i] = trace[i].module;
      read_times[i] = trace[i].read_time;
      write_times[i] = trace[i].write_time;
    }
    BacktraceReplyMessage *message = new BacktraceReplyMessage(id,
          modules.data(), read_times.data(), write_times.data(), size);
    send(message);
    delete message;
  } else {
    sendRequestFailed();
  }
}

void DebuggerIPCServer::handleGetParsedPacket(int id) {
  DebuggerPacket *pk = data_manager->getPacket(id);
  if (pk) {
    auto dbg_info = pk->getDebugInfo();
    if (dbg_info) {

      auto parsed_data = dbg_info->parsed_data();

      ParsedPacketValueMessage message(parsed_data);

      send(&message);
    }
  }

  sendRequestFailed();
}

void DebuggerIPCServer::handleEnableDisableBreakpoint(
      PFPSimDebugger::EnableDisableBreakpointMsg msg) {
  int id = stoi(msg.id());
  if (msg.enable() == "0") {
    data_manager->disableBreakpoint(id);
  } else {
    data_manager->enableBreakpoint(id);
  }
  sendGenericReply();
}

void DebuggerIPCServer::handleEnableDisableWatchpoint(
      PFPSimDebugger::EnableDisableWatchpointMsg msg) {
  int id  = stoi(msg.id());
  if (msg.enable() == "0") {
    data_manager->disableWatchpoint(id);
  } else {
    data_manager->enableWatchpoint(id);
  }
  sendGenericReply();
}

void DebuggerIPCServer::handleIgnoreModule(
      PFPSimDebugger::IgnoreModuleMsg msg) {
  std::string module = msg.module();
  bool del = msg.delete_();
  if (del == true) {
    data_manager->removeIgnoreModule(module);
  } else {
    data_manager->addIgnoreModule(module);
  }
  sendGenericReply();
}

void DebuggerIPCServer::handleGetAllIgnoreModules() {
  std::vector<std::string> &ignore_module_list
        = data_manager->getIgnoreModuleList();
  AllIgnoreModulesMessage *message = new AllIgnoreModulesMessage(
        ignore_module_list.data(), ignore_module_list.size());
  send(message);
  delete message;
}

void DebuggerIPCServer::handleGetSimulationTime() {
  double sim_time = data_manager->getSimulationTime();
  SimulationTimeMessage *message = new SimulationTimeMessage(sim_time);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleBreakOnPacketDrop(
      PFPSimDebugger::BreakOnPacketDropMsg msg) {
  bool on = msg.on();
  data_manager->setBreakOnPacketDrop(on);
  sendGenericReply();
}

void DebuggerIPCServer::handleGetDroppedPackets() {
  std::vector<std::tuple<int, std::string, std::string>> &dropped_packets
        = data_manager->getDroppedPacketList();
  const int num = dropped_packets.size();
  std::vector<int> ids;
  std::vector<std::string> modules;
  std::vector<std::string> reasons;
  for (auto dp = dropped_packets.begin(); dp != dropped_packets.end(); dp++) {
    ids.push_back(std::get<0>(*dp));
    modules.push_back(std::get<1>(*dp));
    reasons.push_back(std::get<2>(*dp));
  }
  DroppedPacketsMessage *message = new DroppedPacketsMessage(
        ids.data(), modules.data(), reasons.data(), num);
  send(message);
  delete message;
}

void DebuggerIPCServer::handleCPCommand(PFPSimDebugger::CPCommandMsg msg) {
  std::string command = msg.command();
  control_plane->do_command(command);
  sendGenericReply();
}

void DebuggerIPCServer::handleGetTableEntries() {
  std::vector<CPDebuggerInterface::TableEntry> entries
        = control_plane->getAllTableEntries();
  TableEntriesMessage *message = new TableEntriesMessage(entries);
  send(message);
  delete message;
}

void DebuggerIPCServer::sendGenericReply() {
  GenericAcknowledgeMessage *message = new GenericAcknowledgeMessage(
        PFPSimDebugger::GenericAcknowledgeMsg_Status_SUCCESS);
  send(message);
  delete message;
}

void DebuggerIPCServer::sendRequestFailed() {
  GenericAcknowledgeMessage *message = new GenericAcknowledgeMessage(
        PFPSimDebugger::GenericAcknowledgeMsg_Status_FAILED);
  send(message);
  delete message;
}

};  // namespace db
};  // namespace core
};  // namespace pfp
