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

#include "Breakpoint.h"
#include <string>
#include <utility>

namespace pfp {
namespace core {
namespace db {

int Breakpoint::next_id = 0;

Breakpoint::Breakpoint(bool stealth): temp(false), disabled(false) {
  if (!stealth) {
    id = next_id++;
  } else {
    id = -1;
  }
}

bool Breakpoint::isEqual(Breakpoint br) {
  for (auto it = conditions.begin(); it != conditions.end(); it++) {
    try {
      if ((br.conditions.at(it->first)) != it->second) {
        return false;
      }
    } catch (std::out_of_range e) {
      return false;
    }
  }
  return true;
}

void Breakpoint::addCondition(BreakpointCondition condition,
      std::string value) {
  conditions.insert(std::pair<BreakpointCondition,
        std::string>(condition, value));
}

int Breakpoint::getID() const {
  return id;
}

};  // namespace db
};  // namespace core
};  // namespace pfp
