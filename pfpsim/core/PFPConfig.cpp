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
 * PFPConfig.cpp
 *
 *  Created on: Oct 19, 2014
 *      Author: Kamil
 */

// #define cores_8

// #include "Configuration.h"
#include "PFPConfig.h"
#include <string>
#include <vector>

namespace pfp {
namespace core {

PFPConfig::PFPConfig() {
  if (verbositylevels.size() == verbosity::PROFILE_LEVELS) {
    for (int i = 0; i < verbositylevels.size(); i++) {
      verbosity_levels_map.emplace(verbositylevels[i], i);
    }
  } else {
    std::cerr << " Verbosity Levels declaration error @"
          << VERBOSITY_LEVELS_FILE_MARKER << ":"
          << VERBOSITY_LEVELS_LINE_MARKER << std::endl;
    std::cerr << "   Levels declared in enum   : "
          << verbosity::PROFILE_LEVELS << std::endl;
    std::cerr << "   Levels declared in vector : "
          << verbositylevels.size() << std::endl;
    exit(USER_ERROR_FATAL);
  }
}

PFPConfig& PFPConfig::get() {
  static PFPConfig instance;
  return instance;
}

const PFPConfig::verbosity PFPConfig::get_verbose_level() {
  return verbo;
}
void PFPConfig::set_verbose_level(PFPConfig::verbosity level) {
  verbo = level;
}
void PFPConfig::set_verbose_level(std::string verbosity_level) {
  auto search = verbosity_levels_map.find(verbosity_level);
  if (search != verbosity_levels_map.end()) {
    verbo = static_cast<verbosity>(search->second);
  } else {
    std::cerr << "Error: Setting Verbose Level: " << verbosity_level
          << " is invalid" << endl;
    std::cerr << "Valid verbose levels are: ";
    for (auto level : verbositylevels) {
      std::cerr << level << ", ";
    }
    std::cerr << endl;
    std::cerr << " Verbosity Levels declared @" << VERBOSITY_LEVELS_FILE_MARKER
          << ":" << VERBOSITY_LEVELS_LINE_MARKER << endl;
    exit(USER_ERROR_FATAL);
  }
}

void
PFPConfig::set_command_line_arg_vector(std::vector<std::string> & args) {
  for (auto & arg : args) {
    auto equals = arg.find('=');
    // No equals sign, that's a paddlin'
    // https://www.youtube.com/watch?v=ZXQR-cPXlmY
    if (equals == std::string::npos) {
       std::stringstream err;
       err << "Invalid key-value pair syntax " << arg
            << " in command line args";
       throw std::runtime_error(err.str());
    }

    auto key = arg.substr(0, equals);
    auto val = arg.substr(equals + 1);

    argMap[key] = val;
  }
}

std::string & PFPConfig::get_command_line_arg(std::string key) {
  auto it = argMap.find(key);

  if (it == argMap.end()) {
    std::stringstream err;
    err << "Missing required command line arg " << key;
    throw std::runtime_error(err.str());
  } else {
    return it->second;
  }
}

void PFPConfig::SetConfigFilePath(std::string path) {
  ConfigFilePath = path;
}
std::string PFPConfig::getConfigFilePath()const {
  return ConfigFilePath;
}
void PFPConfig::SetOutputDirPath(std::string path) {
  OutputDirPath = path;
}
std::string PFPConfig::getOutputDirPath()const {
  return OutputDirPath;
}
void PFPConfig::set_debugger_flag(bool flag_value) {
  debugger_flag = flag_value;
}
bool PFPConfig::debugger_flag_status() {
  return debugger_flag;
}

};  // namespace core
};  // namespace pfp
