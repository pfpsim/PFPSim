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

#include "PFPObject.h"
#include <string>
#include <vector>

#define SIM_ERROR_FATAL -1337

namespace pfp {
namespace core {

MTQueue<std::function<void(void)>> PFPObject::events_;

PFPObject::PFPObject(const std::string& module_name,
      std::string BaseConfigFile, std::string InstanceConfigFile,
      PFPObject* parent, bool enable_dicp):
          GlobalConfigPath(CONFIGROOT),
          dicp_enabled(enable_dicp),
          module_name_(module_name),
          parent_(parent) {
//  cout<<module_name_<<" BaseConfigFile is:"<<BaseConfigFile<<endl;
//  cout<<module_name_<<" InstanceConfigFile is:"<<InstanceConfigFile<<endl;
  LoadBaseConfiguration(BaseConfigFile);
  LoadInstanceConfiguration(InstanceConfigFile);
//  cout<<"SPFor: "<<module_name_<<" @:"<<this<<" parent is:"<<parent_<<endl;
}


PFPObject::PFPObject(const std::string& module_name,
      PFPObject* parent,  bool enable_dicp):
          GlobalConfigPath(CONFIGROOT),
          dicp_enabled(enable_dicp),
          module_name_(module_name),
          parent_(parent) {
//  cout<<"SPFor: "<<module_name_<<" @:"<<this<<" parent is:"<<parent_<<endl;
}

PFPObject::PFPObject(const PFPObject &other, bool enable_dicp)
      : module_name_(other.module_name()),
      parent_(other.parent_),
      dicp_enabled(enable_dicp) {
}


PFPObject::PFPObject()
      : GlobalConfigPath("./"),
      dicp_enabled(false),
      module_name_("NOT Initialized") {
  std::cerr << "Module Construction has gone wrong" << std::endl;
  std::cerr << "[BASE MODULE CONSTRUCTION] PFPSIM PANIC _EXITING_" << std::endl;
  exit(SIM_ERROR_FATAL);
}

void PFPObject::LoadBaseConfiguration(std::string BaseConfigurationFile) {
  SimulationParameters
        = pfp::core::ConfigurationParameterNode(BaseConfigurationFile);

  npulog(std::cout << "Base Configuration: " << On_Red << BaseConfigurationFile
        << txtrst << " for: " << std::endl
        << SimulationParameters.get() << std::endl;)
}

void
PFPObject::LoadInstanceConfiguration(std::string InstanceConfigurationFile) {
  std::string searchstring = InstanceConfigurationFile;
  // Remove the global config path from the string to check if
  // there is a Instance config file
  std::string::size_type i = searchstring.find(GlobalConfigPath);
  if (i != std::string::npos) {
    searchstring.erase(i, GlobalConfigPath.length());
  }
  if (searchstring != "") {
    npulog(cout << "File Opened:" << On_Red << InstanceConfigurationFile
          << txtrst << endl;)
    pfp::core::ConfigurationParameterNode InstanceParameters
          = pfp::core::ConfigurationParameterNode(InstanceConfigurationFile);
    auto FilledParam = SimulationParameters.MatchandFill(InstanceParameters);
    SimulationParameters = FilledParam;

    npulog(cout << On_Yellow << "Instance Json Config: " << txtrst << endl
          << InstanceParameters.get() << std::endl;)
    npulog(cout << On_Yellow << "Matched and Filled for Instance: " << txtrst
          << endl << FilledParam.get() << endl;)
  }
  npulog(cout << "Instance Config Loaded" << module_name() << endl;)
}


pfp::core::ConfigurationParameterNode
PFPObject::GetParameter(std::string param) {
  auto result = SimulationParameters.LocalSearch(param);
  if (result == nullptr && module_name_ != "top") {
    return GetParameterfromParent(param, this->GetParent());
  } else {
    return result;
  }
  // return SimulationParameters[param].get();
}

pfp::core::ConfigurationParameterNode
PFPObject::GetParameterfromParent(std::string param, PFPObject* parent) {
  auto result = parent->SimulationParameters.LocalSearch(param);
  if (parent->module_name() !="top") {
    if (result == nullptr) {
      return GetParameterfromParent(param, parent->GetParent());
    } else {
      return result;
    }
  } else if (parent->module_name() == "top") {
    if (result == nullptr) {
      std::cerr << "Parameter Heirarchy Search Failed: "
            << parent->module_name() << " for: " << param << std::endl;
      exit(0);
    } else {
      return result;
    }
  }
}

const std::string& PFPObject::module_name() const {
  return module_name_;
}

const std::string& PFPObject::fully_qualified_module_name() const {
  static std::string name = "";

  if (name == "") {
    std::stringstream ss;
    bool first = true;
    for (const auto & s : ModuleHierarchy()) {
      if (!first) {
        ss << '.';
      } else {
        first = false;
      }
      ss << s;
    }
    name = ss.str();
  }

  return name;
}

PFPObject* PFPObject::GetParent() {
    return parent_;
}


std::vector<std::string> PFPObject::ModuleHierarchy() const {
  std::vector<std::string> hierarchy;
  hierarchy.push_back(module_name());
  std::string name = module_name();
  std::size_t found = name.find("top");
  if (found == std::string::npos) {  // Current Module is not Top
    std::vector<std::string> result_p = parent_->ModuleHierarchy();
    std::vector<std::string> result;
    result.reserve(hierarchy.size()+result_p.size());
    result.insert(result.end(), hierarchy.begin(), hierarchy.end());
    result.insert(result.end(), result_p.begin(), result_p.end());
    return result;
  } else {  // TOP
    std::vector<std::string> result1;
    result1.push_back(module_name());
    return result1;
  }
}

//========================================//
//                Observers               //
//========================================//

bool PFPObject::add_counter(const std::string& counter_name,
      std::size_t counter_value) {
  if (counters_.find(counter_name) == counters_.end()) {
    counters_.emplace(counter_name, counter_value);
    notify_counter_added(counter_name, sc_time_stamp().to_default_time_units());
    return true;
  } else {
    return false;
  }
}

bool PFPObject::set_counter(const std::string& counter_name,
      std::size_t counter_value) {
  try {
    if (num_counters() == 0) {
      return false;
    } else {
      counters_.at(counter_name) = counter_value;
      notify_counter_changed(counter_name, counter_value,
            sc_time_stamp().to_default_time_units());
      return true;
    }
  }
  catch(std::out_of_range& e) {
    return false;
  }
}

bool PFPObject::remove_counter(const std::string& counter_name) {
  try {
    if (num_counters() == 0) {
      return false;
    } else {
      counters_.erase(counter_name);
      notify_counter_removed(counter_name,
            sc_time_stamp().to_default_time_units());
      return true;
    }
  }
  catch(std::out_of_range &e) {
    return false;
  }
}

std::size_t PFPObject::counter_value(const std::string& counter_name) const {
  return counters_.at(counter_name);
}

bool PFPObject::increment_counter(const std::string& counter_name) {
  try {
    ++counters_.at(counter_name);
    notify_counter_changed(counter_name, counters_.at(counter_name),
          sc_time_stamp().to_default_time_units());
    return true;
  }
  catch(std::out_of_range &e) {
    return false;
  }
}

bool PFPObject::increment_counter(const std::string& counter_name,
      const int incr_amount) {
  try {
    counters_.at(counter_name) += incr_amount;
    notify_counter_changed(counter_name, counters_.at(counter_name),
          sc_time_stamp().to_default_time_units());
    return true;
  }
  catch(std::out_of_range &e) {
    return false;
  }
}

bool PFPObject::decrement_counter(const std::string& counter_name) {
  try {
    if (counters_.at(counter_name) == 0) {
      return false;
    } else {
      --counters_.at(counter_name);
      notify_counter_changed(counter_name, counters_.at(counter_name),
            sc_time_stamp().to_default_time_units());
      return true;
    }
  }
  catch(std::out_of_range &e) {
    return false;
  }
}

bool PFPObject::decrement_counter(const std::string& counter_name,
      const int decr_amount) {
  try {
    if (counters_.at(counter_name) == 0) {
      return false;
    } else {
      counters_.at(counter_name) -= decr_amount;
      notify_counter_changed(counter_name, counters_.at(counter_name),
            sc_time_stamp().to_default_time_units());
      return true;
    }
  }
  catch(std::out_of_range &e) {
    return false;
  }
}

void PFPObject::attach_observer(std::shared_ptr<PFPObserver> observer) {
  for (auto child : childModules_) {
    child.second->attach_observer(observer);
  }
  observers_.push_back(observer);
}

void PFPObject::notify_counter_changed(const std::string& counter_name,
      std::size_t counter_value, double sim_time) {
  for (auto& each_observer : observers_) {
    auto func = std::bind(&PFPObserver::counter_updated, each_observer,
          module_name(), counter_name, counter_value, sim_time);
    events_.push(func);
  }
}

void PFPObject::notify_counter_added(const std::string& counter_name,
      double sim_time) {
  for (auto& each_observer : observers_) {
    each_observer->counter_added(module_name(), counter_name, sim_time);
  }
}

void PFPObject::notify_counter_removed(const std::string& counter_name,
      double sim_time)  {
  for (auto& each_observer : observers_) {
    auto func = std::bind(&PFPObserver::counter_removed, each_observer,
          module_name(), counter_name, sim_time);
    events_.push(func);
  }
}

std::size_t PFPObject::num_counters() const {
  return counters_.size();
}

void PFPObject::AddChildModule(std::string module_name,
      PFPObject* module) {
  childModules_[module_name] = module;
}

};  // namespace core
};  // namespace pfp
