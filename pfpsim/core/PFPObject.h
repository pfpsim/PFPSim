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

#ifndef CORE_PFPOBJECT_H_
#define CORE_PFPOBJECT_H_
#include <string>
#include <vector>
#include <map>
#include "systemc.h"  // NOLINT(build/include)
#include "tlm.h"  // NOLINT(build/include)
#include "TrType.h"
#include "PFPConfig.h"
#include "LMTQueue.h"
#include "MTQueue.h"
#include "PFPObserver.h"
#include "./promptcolors.h"
#include "ConfigurationParameters.h"

#define GetParam(key) get_param_value(configMap, key)
#define GetParamI(key) atoi(get_param_value(configMap, key).c_str())

/*
 * Pre-Processor for NPU Log Levels.
 */

// Format: CommonMacroName_No.ofargs

// Concatenate macro name with number of inputs to base macro
#define CATMACRO(A, B) A ## B
// Select the name based on number of args.
#define SELECTMACRO(NAME, NUM) CATMACRO(NAME ## _, NUM)
// Get count for macros   // Add here for additional number of arguments
#define GET_COUNT(_1, _2, COUNT, ...) COUNT
// Get args         // Add here in reverse for additional number of args.
#define VA_SIZE(...) GET_COUNT(__VA_ARGS__, 2, 1)
#define VA_SELECT(NAME, ...)                                                  \
        SELECTMACRO(NAME, VA_SIZE(__VA_ARGS__))(__VA_ARGS__)  // selector
#define npulog(...) VA_SELECT(LOG, __VA_ARGS__)  // Base Macro

#define npu_error(error_msg) {                                                \
      cerr << "Error: " << (error_msg) << __FILE__ << ":"                     \
      << __LINE__ << endl; sc_stop(); }

#define LOG_1(STAT)\
  if (pfp::core::PFPConfig::get().get_verbose_level()                      \
      != pfp::core::PFPConfig::get().verbosity::minimal                    \
      && pfp::core::PFPConfig::get().get_verbose_level()                   \
      == pfp::core::PFPConfig::get().verbosity::debug) {                   \
      sc_core::sc_time now = sc_time_stamp();                                 \
    cout << txtgrn << std::fixed << now.to_double()/1000 << " ns "            \
          << txtrst << "@ " << txtrst << " ";                                 \
    STAT                                                                      \
    cout << txtrst;                                                           \
}

#define LOG_2(LEVEL, STAT)                                                    \
    if (pfp::core::PFPConfig::get().get_verbose_level()                    \
          == pfp::core::PFPConfig::get().verbosity::LEVEL) {               \
        sc_core::sc_time now = sc_time_stamp();                               \
    std::string modnametemp = module_name_;                                   \
    cout << txtgrn << std::fixed << now.to_double()/1000 << " ns "            \
          << txtrst << "@ " << On_Purple << modnametemp << txtrst << " ";     \
    STAT                                                                      \
    cout << txtrst;                                                           \
    }

//-------------------------------------------------------------------

namespace pfp {
namespace core {
inline std::string convert_to_string(sc_module_name nm) {
  return static_cast<const char*>(nm);
}

class PFPObject {
 public:
  PFPObject();
  PFPObject(const std::string& module_name, PFPObject* parent = 0,
        bool enable_dicp = false);
  PFPObject(const std::string& module_name, std::string BaseConfigFile,
        std::string InstanceConfigFile = "", PFPObject* parent = 0,
        bool enable_dicp = false);
  PFPObject(const PFPObject &other, bool enable_dicp = false);
  virtual ~PFPObject() = default;

  /*--------Configuration of Modules -------------*/
  void LoadBaseConfiguration(std::string);
  void LoadInstanceConfiguration(std::string);

  pfp::core::ConfigurationParameterNode GetParameter(std::string param);
  pfp::core::ConfigurationParameterNode
  GetParameterfromParent(std::string param, PFPObject* parent);

  std::vector<std::string> ModuleHierarchy() const;
  /* ----- DICP ----- */
  const bool dicp_enabled;

  /* --- Module ---- */
  const std::string& module_name() const;
  const std::string& fully_qualified_module_name() const;
  PFPObject* GetParent();

  /* --- Observers --- */
  /**
   * Add a counter to the PFPObject
   * @param counter_name  Name of the counter
   * @param counter_value  Value of the counter (default: 0)
   * @return        True if the counter was added; false if the counter already exists
   */
  virtual bool add_counter(const std::string& counter_name,
        std::size_t counter_value = 0);
  /**
   * Set the value of a counter
   * @param counter_name  Name of the counter
   * @param counter_value  Value of the counter
   * @return        True if the value was changed; false if the PFPObject does not contain the counter
   */
  virtual bool set_counter(const std::string& counter_name,
        std::size_t counter_value);
  /**
   * Remove a counter from the PFPObject
   * @param counter_name  Name of the counter
   * @return        True if the counter was removed; false if the PFPObject does not contain the counter
   */
  virtual bool remove_counter(const std::string& counter_name);
  /**
   * Get the value of a counter
   * Note: This will throw a std::out_of_range exception if the counter does not exist in the PFPObject
   * @param counter_name  Name of the counter
   * @return        Value of the counter
   */
  virtual std::size_t counter_value(const std::string& counter_name) const;
  /**
   * Increment the specified counter
   * @param counter_name  Name of the counter
   * @return        True if the counter was incremented
   */
  virtual bool increment_counter(const std::string& counter_name);
  virtual bool increment_counter(const std::string& counter_name,
        const int incr_amount);
  /**
   * Decrement the specified counter
   * @param counter_name  Name of the counter
   * @return        True if the counter was decremented
   */
  virtual bool decrement_counter(const std::string& counter_name);
  virtual bool decrement_counter(const std::string& counter_name,
        const int decr_amount);
  /**
   * Attach an observer that will be notified when events occur
   * @param observer  Observer to attach
   */
  virtual void attach_observer(std::shared_ptr<PFPObserver> observer);
  /**
   * Notify all attached observers when a counter is changed
   * @param counter_name  Name of the counter
   * @param counter_value  New value of the counter
   * @param sim_time    Simulation time at which the event occurred
   */
  virtual void notify_counter_changed(const std::string& counter_name,
        std::size_t counter_value, double sim_time);
  /**
   * Notify all attached observers when a counter is added to the PFPObject
   * @param counter_name  Name of the counter
   * @param sim_time    Simulation time at which the counter was added (this will usually be 0)
   */
  virtual void notify_counter_added(const std::string& counter_name,
        double sim_time);
  /**
   * Notify all attached observers when a counter is removed from the PFPObject
   * @param counter_name  Name of the counter
   * @param sim_time    Simulation time at which the counter was removed (this will usually be 0)
   */
  virtual void notify_counter_removed(const std::string& counter_name,
        double sim_time);
  /**
   * Get the number of counters in the PFPObject
   * @return  Number of counters
   */
  virtual std::size_t num_counters() const;
  /**
   * Add a child module to the PFPObject
   * @param module_name name of module
   * @param module pointer to the module
   */
  void AddChildModule(std::string module_name, PFPObject* module);

  // TODO(umair) why are all notify_data_* templated when PFPObserver only
  // handles TrType anyway...
  /**
   * Notify all observers of data written
   * @param data    Data that was written
   * @param sim_time  Simulation time of the event
   */
  template <typename DATA_TYPE>
  void notify_data_written(const std::shared_ptr<DATA_TYPE> data,
        double sim_time) {
    for (auto& each_observer : observers_) {
      auto func = std::bind(&PFPObserver::data_written, each_observer,
            fully_qualified_module_name(), data, sim_time);
      events_.push(func);
    }
  }
  /**
   * Notify all observers of data read
   * @param data    Data that was read
   * @param sim_time  Simulation time of the event
   */
  template <typename DATA_TYPE>
  void notify_data_read(const std::shared_ptr<DATA_TYPE> data,
        double sim_time) {
    for (auto& each_observer : observers_) {
      auto func = std::bind(&PFPObserver::data_read, each_observer,
            fully_qualified_module_name(), data, sim_time);
      events_.push(func);
    }
  }
  /**
   * Notify all observers of data drop
   * @param data    Data that was dropped
   * @param sim_time  Simulation time of the event
   */
  template <typename DATA_TYPE>
  void notify_data_dropped(const std::shared_ptr<DATA_TYPE> data,
        std::string& drop_reason, double sim_time) {
    for (auto& each_observer : observers_) {
      auto func = std::bind(&PFPObserver::data_dropped, each_observer,
            fully_qualified_module_name(), data, drop_reason, sim_time);
      events_.push(func);
    }
  }

  template <typename DATA_TYPE>
  void drop_data(const std::shared_ptr<DATA_TYPE> data,
        std::string drop_reason) {
          notify_data_dropped(data,
                              drop_reason,
                              sc_time_stamp().to_default_time_units());
  }

  pfp::core::ConfigurationParameterNode SimulationParameters;
  static MTQueue<std::function<void(void)>> events_;  /*!< Global event queue */

 protected:
  const std::string GlobalConfigPath;
  const std::string module_name_;
  mutable std::string fully_qualified_module_name_;
  PFPObject* parent_;                /*!< Parent of this PFPObject */
  std::map<std::string, std::string> configMap;  /*!< Configuration Map used >*/
  //! Store counters and values
  std::map<std::string, std::size_t> counters_;
  //! List of observers attached to this PFPObject
  std::vector<std::shared_ptr<PFPObserver>> observers_;
  //! Internal list of submodules
  std::map<std::string, PFPObject*> childModules_;
};

};  // namespace core
};  // namespace pfp
#endif  // CORE_PFPOBJECT_H_
