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

#include "StringUtils.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "PFPConfig.h"

std::string to_hex_string(const void * key, size_t length) {
  std::ostringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0');

  for (size_t i = 0; i < length; ++i) {
    ss << std::setw(2)
          << (unsigned int)(reinterpret_cast<const uint8_t*>(key))[i];
  }
  if (pfp::core::PFPConfig::get().get_verbose_level()
        == pfp::core::PFPConfig::get().verbosity::p4profile) {
    std::cout << "Hex string is: " << ss.str() << std::endl;
  }
  return ss.str();
}

std::string to_binary_string(uint8_t * prefix, int width) {
  std::string output(width, '0');
  int byte_index = 0;
  int bit_index  = 0;
  uint8_t mask   = 1 << 7;
  for (; bit_index < width; ++bit_index, mask >>= 1) {
    output[bit_index] = (prefix[byte_index] & mask) ? '1' : '0';
    if (!mask) {
       mask = 1 << 7;
       ++byte_index;
    }
  }
  if (pfp::core::PFPConfig::get().get_verbose_level()
        == pfp::core::PFPConfig::get().verbosity::p4profile) {
  std::cout << " String uitls Binary prefix string is: " << output
        << std::endl;
  }
  return output;
}
