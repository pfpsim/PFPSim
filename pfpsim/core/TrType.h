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

namespace pfp {
namespace core {

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

 private:
  std::size_t id_; /*<! id for the transaction */
};

};  // namespace core
};  // namespace pfp


#endif  // CORE_TRTYPE_H_
