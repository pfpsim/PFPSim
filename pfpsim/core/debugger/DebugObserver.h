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
* @file DebugObserver.h
* Observer for simulation which maintains a record of the simulation via the DebugDataManager so that the debugger may get the necessary information.
*
* Created on: January 22, 2016
* Author: Eric Tremblay
*/

#ifndef CORE_DEBUGGER_DEBUGOBSERVER_H_
#define CORE_DEBUGGER_DEBUGOBSERVER_H_

#include <string>
#include <map>
#include "../TrType.h"
#include "../PFPObserver.h"
#include "DebuggerIPCServer.h"
#include "DebugDataManager.h"
#include "DebuggerPacket.h"

#define VERBOSE 0

namespace pfp {
namespace core {
namespace db {

/**
* Observer for pfpdb.
* The observer gets notified of any events within the simulation and is responsible for maintaining relevant information for the debugger.
* This object should only be created if the user compiles the PFPSim with the appropriate debug flag.
* It is the user's responsibility to notify this observer appropriately.
*/
class DebugObserver : public PFPObserver {
 public:
  /**
   * Default Constructor.
   */
  DebugObserver();

  /**
   * Destructor.
   */
  ~DebugObserver();

  /**
   * Function called by the simulation when a counter is added to a module
   * @param module_name		Module name to which counter was added
   * @param counter_name		Name of the counter
   * @param simulation_time	Simulation time at which counter was added (defaults to 0 since counters are typically added before simulation begins)
   */
  virtual void counter_added(const std::string& module_name,
        const std::string& counter_name, double simulation_time = 0);

  /**
   * Function called by the simulation when a counter is removed from a module
   * @param module_name		Module name from which counter was removed
   * @param counter_name		Name of the counter
   * @param simulation_time	Simulation time at which counter was removed (defaults to 0 since counters are typically removed before simulation begins)
   */
  virtual void counter_removed(const std::string& module_name,
        const std::string& counter_name, double simulation_time = 0);

  /**
   * Function called by the simulation when a counter is updated
   * @param module_name		Module containing updated counter
   * @param counter_name		Name of the counter
   * @param new_value			Current value of the counter
   * @param simulation_time	Simulation time at which counter value was updated
   */
  virtual void counter_updated(const std::string& module_name,
        const std::string& counter_name,
        std::size_t new_value,
        double simulation_time);

  /**
   * Function called by the simulation when data is written by a module
   * @param from_module		Module name of the transmitting module
   * @param data				JSON representation of data
   * @param simulation_time	Simulation time at which event occurred
   */
  void data_written(const std::string& from_module,
        const std::shared_ptr<TrType> data,
        double simulation_time) override;

  /**
   * Function called by the simulation when data is read by a module
   * @param to_module			Module name of the receiving module
   * @param data				JSON representation of data
   * @param simulation_time	Simulation time at which event occurred
   */
  void data_read(const std::string& to_module,
        const std::shared_ptr<TrType> data,
        double simulation_time) override;

  /**
   * Function called by the simulation when data is dropped in a module
   * @param in_module			Module name in which data was dropped
   * @param data				JSON representation of data
   * @param simulation_time	Simulation time at which event occurred
   */
  void data_dropped(const std::string& in_module,
        const std::shared_ptr<TrType> data,
        const std::string& drop_reason,
        double simulation_time) override;

  /**
   * Function called by the simulation when a TEU thread begins
   * @param teu_mod			TEU in which thread was started
   * @param tec_mod			TEC containing the TEU in which the thread was started
   * @param thread_id			ID number of the thread
   * @param simulation_time	Simulation time at which event occurred
   */
  virtual void thread_begin(const std::string& teu_mod,
        const std::string& tec_mod, std::size_t thread_id,
        std::size_t packet_id, double simulation_time);

  /**
   * Function called by the simulation when a TEU thread ends
   * @param teu_mod			TEU in which thread was ended
   * @param tec_mod			TEC containing the TEU in which the thread was ended
   * @param thread_id			ID number of the thread
   * @param simulation_time	Simulation time at which event occurred
   */
  virtual void thread_end(const std::string& teu_mod,
        const std::string& tec_mod, std::size_t thread_id,
        std::size_t packet_id, double simulation_time);

  /**
   * Function called by the simulation when a TEU thread starts idling
   * @param teu_mod			TEU in which thread is idling
   * @param tec_mod			TEC containing the TEU in which the thread is idling
   * @param thread_id			ID number of the thread
   * @param simulation_time	Simulation time at which event occurred
   */
  virtual void thread_idle(const std::string& teu_mod,
        const std::string& tec_mod, std::size_t thread_id,
        std::size_t packet_id, double simulation_time);

  virtual void core_busy(const std::string& teu_mod,
        const std::string& tec_mod, double simulation_time);
  virtual void core_idle(const std::string& teu_mod,
        const std::string& tec_mod, double simulation_time);

  /**
   * Called after SystemC elaboration to halt the simulation until the run command is sent.
   */
  void waitForRunCommand();

  /**
   * Register a Control Plane (which implements CPDebuggerInterface) to the debugger.
   * This gives the debugger the ability to insert, modify and delete table entries as well as obtain the state of the tables.
   * @param cp_debug_if Control Plane object.
   */
  void registerCP(CPDebuggerInterface *cp_debug_if);

  /**
   * Enables the debugger.
   */
  void enableDebugger();

 private:
  DebuggerIPCServer *ipc_server;   /*!< Pointer to IPCServer. */
  DebugDataManager *data_manager;    /*!< Pointer to DebugDataManager. */
  bool enable = false;   /*!< Indicates if the debugger is enabled. */

  /**
   * Function called to block the simulation from within the observer
   */
  void pause();

  /**
   * Function called to notify the server that the simulation has stopped and that it can now give back control to the debugger
   */
  void notifyServer();

  /**
   * Update the current simulation time to a more recent value.
   * @param sim_time New simulation time in nanoseconds.
   */
  void updateSimulationTime(double sim_time);

  /**
   * Update which packet is currently be followed or focused on.
   * @param packet_id ID of packet.
   */
  void updateWhoAmI(int packet_id);

  /**
   * Check to see if the module, packet id or simulation time should cause a breakpoint hit.
   * @param module    Current module name.
   * @param packet_id ID of current packet.
   * @param sim_time  Current simulation time in nanoseconds.
   * @param read      Indicates if the packet is entering or exiting the module.If it's not a read than it's a write.
   */
  void checkBreakpointHit(std::string module, int packet_id,
        double sim_time, bool read);
};

};  // namespace db
};  // namespace core
};  // namespace pfp

#endif  // CORE_DEBUGGER_DEBUGOBSERVER_H_
