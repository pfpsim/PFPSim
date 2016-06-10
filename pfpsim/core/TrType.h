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
 * TrType.h
 *
 *  Created on: Aug 13, 2015
 *      Author: shafigh
 */

#ifndef CORE_TRTYPE_H_
#define CORE_TRTYPE_H_

#include <iostream>
#include <string>
#include <memory>
#include <vector>

namespace pfp {
namespace core {

/**
 *  Class which is used to hold info about a packet that
 *  can be shown in the debugger.
 */
class DebugInfo {
 public:
  virtual ~DebugInfo() = default;

  typedef std::vector<uint8_t> RawData;

  struct Field {
    const std::string name;
    const RawData     value;

    Field(std::string n, RawData v)
      : name(n), value(v) {}
  };

  struct Header {
    const std::string name;
    const std::vector<Field> fields;

    Header(std::string n, std::vector<Field> f)
      : name(n), fields(f) {}
  };

  // Get the raw unparsed data of a packet.
  virtual RawData raw_data() const = 0;

  virtual RawData field_value(const std::string & field_name) const = 0;

  // Get the parsed representation of a packet. Makes the (reasonable)
  // assumption that a packet is an ordered list of named headers,
  // each of which is just an ordered list of named fields.
  virtual std::vector<Header> parsed_data() const = 0;

  // Check if this DebugInfo is still valid (refers to a packet which still
  // exists inside the simulation)
  virtual bool valid() const = 0;
};

class TrType {
 public:
  TrType() = default;
  explicit TrType(std::size_t id):id_(id) { }
  virtual ~TrType() = default;
  virtual void id(std::size_t id) {
    id_ = id;
  }
  virtual std::size_t id() const {
    return id_;
  }
  virtual std::string data_type() const = 0;

  /**
   * Check whether this packet should be watched by the debugger.
   * By default return false, implementing classes that want to
   * be debuggable should return true. Classes which want to be
   * debugged should also be sure that distinct objects have
   * distinct IDs
   */
  virtual bool debuggable() const {
    return false;
  }

  /**
   * Get a pointer to an object describing the packet held by this TrType.
   * This method will only be called if `debuggable` returns true.
   */
  virtual std::shared_ptr<const DebugInfo> debug_info() const {
    return nullptr;
  }

 private:
  std::size_t id_; /*<! id for the transaction */
};

};  // namespace core
};  // namespace pfp


#endif  // CORE_TRTYPE_H_
