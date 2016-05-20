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

#include "ConfigurationParameters.h"
#include <string>
#include "PFPObject.h"
#include "promptcolors.h"

namespace pfp {
namespace core {

ConfigurationParameterNode::ConfigurationParameterNode(std::string filename) {
  std::ifstream ifs(filename);
  if (!ifs.fail()) {
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    ConfigurationParameters = json::parse(content);
  } else {
    std::cerr << On_Purple << "Could not open file: " << txtrst << On_Red
          << filename << txtrst << std::endl;
    exit(-1);
  }
}

ConfigurationParameterNode::ConfigurationParameterNode(
      ConfigurationParameterNode::Matrix object)
      :ConfigurationParameters(object) {
}

ConfigurationParameterNode::ConfigurationParameterNode()
      :ConfigurationParameters(nullptr) {
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::Searchinparent(std::string param,
      pfp::core::PFPObject* CurrentModule) {
  std::string name = CurrentModule->module_name();
  std::size_t found = name.find("top");
  // Current Module is not Top, its parent might be top
  if (found == std::string::npos) {
    auto result = CurrentModule->SimulationParameters.LocalSearch(param);
    // Do a local search if it fails then call its parent.
    if (result == nullptr) {
      return CurrentModule->SimulationParameters.Searchinparent(param,
              CurrentModule->GetParent());
    } else {
      return result;
    }
  } else {  // We have reached the top module of the hierarchy
    auto result = CurrentModule->SimulationParameters.LocalSearch(param);
    if (result == nullptr) {
      std::cerr << "Parameter: " << param
            << " not found in Hierarchal Search. "
            << "Please check configuration files" << endl;
      exit(USER_ERROR_FATAL);
    } else {
      return result;
    }
  }
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::Searchinparent(int position,
      pfp::core::PFPObject* CurrentModule) {
  std::string name = CurrentModule->module_name();
  std::size_t found = name.find("top");
  // Current Module is not Top, its parent might be top
  if (found == std::string::npos) {
    auto result = CurrentModule->SimulationParameters.LocalSearch(position);
    // Do a local search if it fails then call its parent.
    if (result == nullptr) {
      return CurrentModule->SimulationParameters.Searchinparent(position,
            CurrentModule->GetParent());
    } else {
      return result;
    }
  } else {  // We have reached the top module of the hierarchy
    auto result = CurrentModule->SimulationParameters.LocalSearch(position);
    if (result == nullptr) {
      std::cerr << "Array Lookup: " << position
            << " not found in HierarchalSearch. "
            << "Please check configuration files" << endl;
      exit(USER_ERROR_FATAL);
    } else {
      return result;
    }
  }
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::HeirarchalSearch(std::string param) {
  for (json::iterator it = ConfigurationParameters.begin();
        it != ConfigurationParameters.end(); ++it) {
     if (it.key() == param) {
         return it.value();
     }
  }

  // No Match go to a parent
  sc_object* current_scmodule
        = sc_get_current_process_handle().get_parent_object();
  if (dynamic_cast<pfp::core::PFPObject*>(current_scmodule)) {
    return Searchinparent(param,
          dynamic_cast<pfp::core::PFPObject*>(current_scmodule)->GetParent());
  } else {
    std::cerr << On_Red << "[PFPSIM runtime error]: " << txtrst
          << "Not in a PFPSIM module heirarichal search for param: "
          << param << " failed" << endl;
    exit(0);
  }
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::HeirarchalSearch(int position) {
  if (position < ConfigurationParameters.size()) {
      return ConfigurationParameters.at(position);
  } else {
    sc_object* current_scmodule
          = sc_get_current_process_handle().get_parent_object();
    if (dynamic_cast<pfp::core::PFPObject*>(current_scmodule)) {
     return Searchinparent(position,
          dynamic_cast<pfp::core::PFPObject*>(current_scmodule));
    } else {
      std::cerr << On_Red << "[PFPSIM runtime error]: " << txtrst
            << "Not in a PFPSIM module heirarichal search for param: "
            << position << " failed" << endl;
      exit(0);
    }
  }
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::LocalSearch(std::string param) {
  for (json::iterator it = ConfigurationParameters.begin();
        it != ConfigurationParameters.end(); ++it) {
     if (it.key() == param) {
         return it.value();
     }
  }
  return nullptr;  // Search Failed return a nullptr
}

ConfigurationParameterNode::Matrix
ConfigurationParameterNode::LocalSearch(int position) {
  if (position < ConfigurationParameters.size()) {
      return ConfigurationParameters.at(position);
  } else {
    return nullptr;  // Search failed return a nullptr
  }
}

ConfigurationParameterNode::Matrix ConfigurationParameterNode::get() {
  return ConfigurationParameters;
}


ConfigurationParameterNode::CompareResult
ConfigurationParameterNode::CompareStructure(
      ConfigurationParameterNode::Matrix compare,
      ConfigurationParameterNode::Matrix golden) {
  // cout<<" ----Comapring:"<<endl
  //     <<" ----Golden:   "<<golden<<endl
  //     <<" ----Test:     "<<compare <<endl;
  // Determine if the compare objects are arrays or objects
  if (golden.is_structured() && compare.is_structured()) {
    Matrix::iterator compareit = compare.begin();
    Matrix::iterator goldenit  = golden.begin();
    if (golden.is_object() && compare.is_object()) {  // dict
      for (; compareit != compare.end(); ++compareit, ++goldenit) {
        if (golden.find(compareit.key()) == golden.end()) {
              std::string searchkey = compareit.key();
              raiseError("Key Not found [KVP] for key " + searchkey
                    + " in [test]", compare, golden);
        } else {
          // Match
          // cout<< " Matched for "<<compareit.key()<<endl;
          auto result = CompareStructure(compareit.value(),
                golden[compareit.key()]);
          golden[compareit.key()] = result.resultobject;
        }
      }
      return CompareResult{true, Matrix(golden)};
    } else if (golden.is_array() && compare.is_array()) {  // list
      if (golden.size() == compare.size()) {
        for (; compareit != compare.end(); ++compareit, ++goldenit) {
          auto result = CompareStructure(*compareit, *goldenit);
          *goldenit = result.resultobject;
        }
      } else {
        raiseError("Length not same [List]", compare, golden);
      }
      return CompareResult{true, Matrix(golden)};
    } else {
      raiseError(
            "[JSON Lib error] Object is structured, but not an [object,array]");
    }
  } else if (golden.is_primitive() && compare.is_primitive()) {
    if (golden.type() == compare.type()) {
      if (golden != compare) {
        // raiseError("Value not same for primitives",compare,golden);
        // comapre = golden
        // Values are different Here and thats ok they are of the same type.
        return CompareResult{true, Matrix(compare)};
      } else {
        return CompareResult{true, Matrix(golden)};
      }
    } else {
      raiseError("Primitive Value type is not the same", compare, golden);
    }
  } else {
    std::cerr << "JSON FILE ERROR" << "UNSUPPORTED TYPES IN PARSING";
    exit(0);
  }
  return CompareResult{false, Matrix(nullptr)};
}

ConfigurationParameterNode
ConfigurationParameterNode::MatchandFill(
      ConfigurationParameterNode matchagainst) {
  auto compareresult = CompareStructure(matchagainst.get(),
        ConfigurationParameters);

  if (compareresult.result) {
    // cout<<On_Blue<<"Golden obj filled with Instance objects: "<<txtrst<<endl
    //     <<compareresult.resultobject<<endl;
    return compareresult.resultobject;
  } else {
    raiseError("Config File Match & Fill Internal Error - [Unreacheable state] -- Uncaught Error in Compare Structure");  // NOLINT(whitespace/line_length)
  }
  // Statment is never reached just added to stop clang complaining.
  return(ConfigurationParameterNode(json(nullptr)));
}

void ConfigurationParameterNode::raiseError(std::string message,
      ConfigurationParameterNode::Matrix compare,
      ConfigurationParameterNode::Matrix golden) {
  std::cerr << On_Red << "[PFPSIM runtime error]: " << txtrst << std::endl
            << "Paramter Configruation File Error: " << message << std::endl
            << "(Golden) for:   " << golden << std::endl
            << "(Test) against: " << compare << std::endl;
  exit(EXIT_FAILURE);
}

void ConfigurationParameterNode::raiseError(std::string message) {
  std::cerr << On_Red << "[PFPSIM Runtime Error]" << txtrst << std::endl
            << "Paramter Configruation File Error: " << std::endl
            <<  message << endl;
  exit(EXIT_FAILURE);
}

};  // namespace core
};  // namespace pfp
