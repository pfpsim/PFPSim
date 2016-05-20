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
 * PFPObserver.h
 *
 *  Created on: Jul 17, 2014
 *      Author: kamil
 */
/**
 * @file PFPObserver.h
 * All observers of the PFP Simulations must inherit from interface PFPObserver.
 * The observer must be registered with the NPU through the NPU's attach_observer(PFPObserver*) method.
 */
#ifndef CORE_PFPOBSERVER_H_
#define CORE_PFPOBSERVER_H_

#include <string>
#include <memory>

namespace pfp {
namespace core {

class PFPObserver {
 public:
  /**
   * Function called by the NPU when a counter is added to a module
   * @param module_name    Module name to which counter was added
   * @param counter_name    Name of the counter
   * @param simulation_time  Simulation time at which counter was added (defaults to 0 since counters are typically added before simulation begins)
   */
  virtual void counter_added(const std::string& module_name,
          const std::string& counter_name,
          double simulation_time = 0) = 0;

  /**
   * Function called by the NPU when a counter is removed from a module
   * @param module_name    Module name from which counter was removed
   * @param counter_name    Name of the counter
   * @param simulation_time  Simulation time at which counter was removed (defaults to 0 since counters are typically removed before simulation begins)
   */
  virtual void counter_removed(const std::string& module_name,
          const std::string& counter_name,
          double simulation_time = 0) = 0;

  /**
   * Function called by the NPU when a counter is updated
   * @param module_name    Module containing updated counter
   * @param counter_name    Name of the counter
   * @param new_value      Current value of the counter
   * @param simulation_time  Simulation time at which counter value was updated
   */
  virtual void counter_updated(const std::string& module_name,
          const std::string& counter_name,
          std::size_t new_value,
          double simulation_time) = 0;

  /**
   * Function called by the NPU when data is written by a module
   * @param from_module    Module name of the transmitting module
   * @param data        JSON representation of data
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void data_written(const std::string& from_module,
          const std::shared_ptr<pfp::core::TrType> data,
          double simulation_time) = 0;

  /**
   * Function called by the NPU when data is read by a module
   * @param to_module      Module name of the receiving module
   * @param data        JSON representation of data
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void data_read(const std::string& to_module,
           const std::shared_ptr<pfp::core::TrType> data,
           double simulation_time) = 0;

  /**
   * Function called by the NPU when data is dropped in a module
   * @param in_module      Module name in which data was dropped
   * @param data        JSON representation of data
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void data_dropped(const std::string& in_module,
            const std::shared_ptr<pfp::core::TrType> data,
            const std::string& drop_reason,
            double simulation_time) = 0;

  /**
   * Function called by the NPU when a TEU thread begins
   * @param teu_mod      TEU in which thread was started
   * @param tec_mod      TEC containing the TEU in which the thread was started
   * @param thread_id      ID number of the thread
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void thread_begin(const std::string& teu_mod,
            const std::string& tec_mod,
            std::size_t thread_id,
            std::size_t packet_id,
            double simulation_time) = 0;

  /**
   * Function called by the NPU when a TEU thread ends
   * @param teu_mod      TEU in which thread was ended
   * @param tec_mod      TEC containing the TEU in which the thread was ended
   * @param thread_id      ID number of the thread
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void thread_end(const std::string& teu_mod,
            const std::string& tec_mod,
            std::size_t thread_id,
            std::size_t packet_id,
            double simulation_time) = 0;

  /**
   * Function called by the NPU when a TEU thread starts idling
   * @param teu_mod      TEU in which thread is idling
   * @param tec_mod      TEC containing the TEU in which the thread is idling
   * @param thread_id      ID number of the thread
   * @param simulation_time  Simulation time at which event occurred
   */
  virtual void thread_idle(const std::string& teu_mod,
            const std::string& tec_mod,
            std::size_t thread_id,
            std::size_t packet_id,
            double simulation_time) = 0;

  virtual void core_busy(const std::string& teu_mod,
            const std::string& tec_mod,
            double simulation_time) = 0;

  virtual void core_idle(const std::string& teu_mod,
            const std::string& tec_mod,
            double simulation_time) = 0;

 protected:
  /**
   * Default destructor
   */
  virtual ~PFPObserver() = default;
};
};  // namespace core
};  // namespace pfp
#endif  // CORE_PFPOBSERVER_H_
