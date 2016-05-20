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

#include "DebuggerPacket.h"
#include <string>
#include <vector>

namespace pfp {
namespace core {
namespace db {

DebuggerPacket::DebuggerPacket() {}

DebuggerPacket::DebuggerPacket(int id, std::string location, double time_ns)
      : packet_id(id), current_location(location), last_notify_time(time_ns) {}

void DebuggerPacket::updateTraceReadTime(std::string mod, double rtime) {
  PacketLocation pl;
  pl.module = mod;
  pl.read_time = rtime;
  trace.push_back(pl);
}

void DebuggerPacket::updateTraceWriteTime(std::string mod, double wtime) {
  for (auto pl = trace.rbegin(); pl != trace.rend(); pl++) {
    if (pl->module == mod && pl->write_time == -1) {
      pl->write_time = wtime;
      return;
    }
  }
  PacketLocation pl;
  pl.module = mod;
  pl.write_time = wtime;
  trace.push_back(pl);
}

int DebuggerPacket::getID() const {
  return packet_id;
}

std::string DebuggerPacket::getLocation() const {
  return current_location;
}

double DebuggerPacket::getTime() const {
  return last_notify_time;
}

std::vector<DebuggerPacket::PacketLocation> DebuggerPacket::getTrace() const {
  return trace;
}

void DebuggerPacket::setCurrentLocation(std::string loc) {
  current_location = loc;
}

void DebuggerPacket::setTime(double t) {
  last_notify_time = t;
}

};  // namespace db
};  // namespace core
};  // namespace pfp
