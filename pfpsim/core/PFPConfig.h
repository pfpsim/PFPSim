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
 * PFPConfig.h
 *
 *  Created on: Oct 18, 2014
 *      Author: Kamil
 */
/**
 * @class PFPConfig
 * WARNING: This is a singleton
 *       It might be better to just change this to a global variable
 *       Needs a compiler that is highly compliant with C++11
 */
#ifndef CORE_PFPCONFIG_H_
#define CORE_PFPCONFIG_H_

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "systemc.h"

#define INPUT_FILE_ERROR -7
#define USER_ERROR_FATAL -10
#define SPCONFIG(param)             \
pfp::core::GetParameter(param);
#define SPSETCONFIGPATH(path)       \
pfp::core::PFPConfig::get().SetConfigFilePath(path);
#define SPSETOUTPUTDIRPATH(path)    \
pfp::core::PFPConfig::get().SetOutputDirPath(path);
#define SPSETARGS(args)             \
pfp::core::PFPConfig::get().set_command_line_arg_vector(args);
#define SPARG(key)                  \
pfp::core::PFPConfig::get().get_command_line_arg(key)
#define CONFIGROOT                  \
pfp::core::PFPConfig::get().getConfigFilePath()
#define OUTPUTDIR                   \
pfp::core::PFPConfig::get().getOutputDirPath()
#define ISVERBOSITY(level) \
pfp::core::PFPConfig::get().get_verbose_level() \
    == pfp::core::PFPConfig::get().verbosity::level
#define SET_PFP_DEBUGGER_FLAG(debugger_enabled) \
pfp::core::PFPConfig::get().set_debugger_flag(debugger_enabled)
#define PFP_DEBUGGER_ENABLED \
pfp::core::PFPConfig::get().debugger_flag_status()

namespace pfp {
namespace core {

class PFPConfig {
 public:
  /**
   * Get a reference to the configuration class
   * @return  Singleton reference to configuration
   */
  static PFPConfig& get();

  void SetConfigFilePath(std::string path);
  std::string getConfigFilePath() const;
  void SetOutputDirPath(std::string path);
  std::string getOutputDirPath() const;

  #define VERBOSITY_LEVELS_LINE_MARKER __LINE__
  #define VERBOSITY_LEVELS_FILE_MARKER __FILE__
  enum verbosity {
    normal,
    minimal,
    p4profile,
    profile,
    debugger,
    debug,
    PROFILE_LEVELS = debug + 1  /*returns the number of enums in verbosity*/
  };
  /*
   * Warning:
   * enum is implicitly tied to vector, index positions of levels in both containers should be same
   * so that it returns the same enum value from the map
   */
  std::vector<std::string> verbositylevels {
        "normal", "minimal", "p4profile", "profile", "debugger", "debug"};

  /**
   * Pop the currently running parameter values
   * PFPConfig only allows the NPU to call simulation complete
   * TODO: Once implemented, change this so that a simulation controller calls simulation complete
   */
  const verbosity get_verbose_level();
  void set_verbose_level(PFPConfig::verbosity);
  void set_verbose_level(std::string verbosity_level);

  void set_command_line_arg_vector(std::vector<std::string> & v);
  std::string & get_command_line_arg(std::string key);
  void set_debugger_flag(bool flag_value);
  bool debugger_flag_status();


 private:
  /**
   * Private constructor
   */
  PFPConfig();

  PFPConfig(const PFPConfig &) = delete;
  PFPConfig& operator=(const PFPConfig &) = delete;
  PFPConfig(PFPConfig &&) = delete;
  PFPConfig& operator=(PFPConfig &&) = delete;

 private:
  std::string verbose;
  verbosity verbo;
  sc_mutex pop_guard;
  std::string ConfigFilePath;
  std::string OutputDirPath;
  std::map<std::string, std::string> configMap;
  std::map<std::string, std::string> argMap;
  /* Map that returns the enum from the string*/
  std::map<std::string, uint64_t> verbosity_levels_map;
  bool debugger_flag;
};
};  // namespace core
};  // namespace pfp
#endif  // CORE_PFPCONFIG_H_
