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
* @file DebuggerIPCServer.h
* Defines class that services requests from pfpdb.
*
* Created on: January 27, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_DEBUGGERIPCSERVER_H_
#define CORE_DEBUGGER_DEBUGGERIPCSERVER_H_
#include <nanomsg/reqrep.h>
#include <nanomsg/nn.h>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "DebuggerMessages.h"
#include "DebugDataManager.h"
#include "CPDebuggerInterface.h"
#include "../proto/PFPSimDebugger.pb.h"

namespace pfp {
namespace core {
namespace db {

/**
 * Establishes communication with pfpdb python script and services its requests.
 * The inter-process communication is accomplished via messages send using nanomsg.
 */
class DebuggerIPCServer {
 public:
  /**
   * Constructor.
   * @param url URL for IPC communication.
   * @param dm  Pointer to the DebugDataManager.
   */
  DebuggerIPCServer(std::string url, DebugDataManager *dm);

  /**
   * Destructor.
   */
  ~DebuggerIPCServer();

  /**
   * Starts the thread which services the requests from pfpdb.
   */
  void start();

  /**
   * Called via the DebugObserver after SystemC elaboration to halt the simulation until the run command is sent.
   */
  void waitForRunCommand();

  /**
   * Get reference to mutex used to halt simulation when a breakpoint is hit.
   * @return Reference to mutex.
   */
  std::mutex& getBkptMutex();

  /**
   * Get reference to mutex used to halt DebuggerIPCServer while the simulation is running.
   * @return Reference to mutex.
   */
  std::mutex& getStopMutex();

  /**
   * Get reference to condition variable used to notify the DebugObserver that a continue or next command has been received and thus th simulation may proceed.
   * @return Reference to condition variable.
   */
  std::condition_variable& getBkptConditionVariable();

  /**
   * Get reference to condition variable used by the DebugObserver to notify the DebuggerIPCServer that the simulation has halted.
   * @return Reference to condition variable
   */
  std::condition_variable& getStopConditionVariable();

  /**
   * Get whether a continue command has been received.
   * @return True when a continue has been recevied, otherwise, false.
   */
  bool getContinueCommand() const;

  /**
   * Set the flag indicating if the continue command has been received.
   * @param b New value of flag.
   */
  void setContinueCommand(bool b);

  /**
   * Get whether the simulation has stopped.
   * @return True if the simulation is stopped, otherwise, false.
   */
  bool getStopped() const;

  /**
   * Set flag that indicates if the simulation is stopped.
   * @param b New value of flag.
   */
  void setStopped(bool b);

  /**
   * Set the message that will be send to pfpdb the next time the simulation is halted.
   * @param message Message object to send.
   */
  void setReplyMessage(DebuggerMessage *message);

  /**
   * Called via the DebugObserver to register a Control Plane module to the debugger so that pfpdb may interact with the Control Plane to insert, modify and delete table entries as well as obtain the current state of the tables.
   * @param cp_debug_if Control Plane object.
   */
  void registerCP(CPDebuggerInterface *cp_debug_if);

 private:
  //! socket which binds to ipc url
  int socket;
  //! thread which services requests from debugger
  std::thread debug_req_thread;
  //! Indicates when the server thread should terminate
  bool kill_thread = false;
  //! pointer to data_manager object
  DebugDataManager *data_manager;
  //! mutex that blocks the simulation before sc_start
  std::mutex start_mutex;
  //! mutex that blocks the simulation when a breakpoint is hit
  std::mutex bkpt_mutex;
  //! mutex that blocks the ipc server thread and
  //! thus the PFPSimDebugger when the simulation is running
  std::mutex stop_mutex;
  //! lock used on start_mutex
  std::unique_lock<std::mutex> *ulock;
  //! condition variable to notify the simulation when it should unblock
  //! after a breakpoint hit
  std::condition_variable bkpt_cv;
  //! condition variable to notify the ipc server thread when
  //! the simulation has stopped
  std::condition_variable stop_cv;
  //! indicates when a continue command is sent
  bool continue_command = false;
  //! indicates when the simulation has stopped due to a breakpoint hit
  bool stopped = false;
  //! Indicates if the run command has been called or not.
  bool run_called = false;

  //! pointer to message that should be sent when the simulation stops
  DebuggerMessage *reply_message;

  //! Pointer to registered CPDebuggerInterface object.
  CPDebuggerInterface *control_plane;

  /**
   * Send a DebuggerMessage to pfpdb.
   * @param message Message to send.
   */
  void send(DebuggerMessage *message);

  /**
   * Receive a request message from pfpdb.
   * @return Request from pfpdb.
   */
  PFPSimDebugger::DebugMsg* recv();

  /**
   * Parse the request from pfpdb and call the appropriate function to handle the particular request.
   * @param request Message from pfpdb.
   */
  void parseRequest(PFPSimDebugger::DebugMsg *request);

  /**
   * Thread which listens on the IPC channel and services any request from the debugger.
   */
  void requestThread();

  /**
   * Halts the DebuggerIPCServer until the simulation stops, where control is given back to pfpdb.
   */
  void waitForStop();

  /**
   * Member functions used to handle each type of message that pfpdb can send.
   */
  void handleRun(double time_ns = -1);
  void handleContinue(double time_ns = -1);
  void handleGetAllCounters();
  void handleGetCounter(PFPSimDebugger::GetCounterMsg msg);
  void handleSetBreakpoint(PFPSimDebugger::SetBreakpointMsg msg);
  void handleGetAllBreakpoints();
  void handleRemoveBreakpoint(PFPSimDebugger::RemoveBreakpointMsg msg);
  void handleWhoAmI();
  void handleNext();
  void handleGetPacketList(PFPSimDebugger::GetPacketListMsg msg);
  void handleSetWatchpoint(PFPSimDebugger::SetWatchpointMsg msg);
  void handleGetAllWatchpoints();
  void handleRemoveWatchpoint(PFPSimDebugger::RemoveWatchpointMsg msg);
  void handleBacktrace(PFPSimDebugger::BacktraceMsg msg);
  void handleEnableDisableBreakpoint(
        PFPSimDebugger::EnableDisableBreakpointMsg msg);
  void handleEnableDisableWatchpoint(
        PFPSimDebugger::EnableDisableWatchpointMsg msg);
  void handleIgnoreModule(PFPSimDebugger::IgnoreModuleMsg msg);
  void handleGetAllIgnoreModules();
  void handleGetSimulationTime();
  void handleBreakOnPacketDrop(PFPSimDebugger::BreakOnPacketDropMsg msg);
  void handleGetDroppedPackets();
  void handleCPCommand(PFPSimDebugger::CPCommandMsg msg);
  void handleGetTableEntries();

  void handleGetParsedPacket(int id);
  void handleGetRawPacket(int id);
  void handleGetPacketField(int id, std::string field_name);

  void handleStartTracing(PFPSimDebugger::StartTracingMsg & msg);
  /**
   * Send pfpdb a generic reply so that it may regain control.
   */
  void sendGenericReply();

  /**
   * Send pfpdb a message that indicates that the previous request failed.
   */
  void sendRequestFailed();
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_DEBUGGERIPCSERVER_H_
