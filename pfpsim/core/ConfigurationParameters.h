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

/*
*	Author: Lemniscate Snickets
* Created On: 04/04/2016
*/
#ifndef CORE_CONFIGURATIONPARAMETERS_H_
#define CORE_CONFIGURATIONPARAMETERS_H_

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;

namespace pfp{
namespace core{

class PFPObject;
class ConfigurationParameterNode {
 public:
  typedef json Matrix;  // Dom representation is in json format using json lib.

  explicit ConfigurationParameterNode(std::string filename);
  ConfigurationParameterNode(Matrix object);  // NOLINT(runtime/explicit)
  ConfigurationParameterNode();

  /**
   * [operator[] overloads for searching and chaining to maintain intuitive syntax Default behaviour is for hierarchy lookups]
   * @param  position [index position in list ]
   * @return          [description]
   */
  ConfigurationParameterNode  operator[] (const int position) {
    // std::cout << "Operator["<<position<<"] called"<<std::endl;
    auto returnvalue = HeirarchalSearch(position);
    return ConfigurationParameterNode(returnvalue);
  }

  /**
  * [operator[] overloads for searching and chaining to maintain intuitive syntax Default behaviour is for hierarchy lookups]
   * @param  Value [key in a keyvalue pair]
   * @return       [description]
   */
  ConfigurationParameterNode  operator[] (const std::string Value) {
    // std::cout<<"Operator["<<Value<<"] called " <<std::endl;
    auto returnvalue = HeirarchalSearch(Value);
    return ConfigurationParameterNode(returnvalue);
  }
  /**
   * Getter to access the [value for KVP] or [value at the position in a list] after search.
   * @return [object of json type]
   */
  Matrix get();

  template<typename T>
  T get() {
    return ConfigurationParameters.get<T>();
  }

  Matrix LocalSearch(std::string param);
  Matrix LocalSearch(int position);
  Matrix HeirarchalSearch(std::string param);
  Matrix HeirarchalSearch(int position);
  Matrix Searchinparent(std::string Value, pfp::core::PFPObject* ParentModule);
  Matrix Searchinparent(int position, pfp::core::PFPObject* ParentModule);


  struct CompareResult{
    bool result;
    Matrix resultobject;
  };

  CompareResult CompareStructure(Matrix compareto, Matrix golden);
  ConfigurationParameterNode MatchandFill(
          ConfigurationParameterNode matchagainst);
  void raiseError(std::string message, Matrix compare, Matrix golden);
  void raiseError(std::string);

 private:
  /*Variable that stores the JSON/XML DOM object*/
  Matrix ConfigurationParameters;
};
};  // namespace core
};  // namespace pfp

#endif  // CORE_CONFIGURATIONPARAMETERS_H_
