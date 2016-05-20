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

#include <getopt.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "PFPConfig.h"
#include "PFPContext.h"

using pfp::core::PFPConfig;
using pfp::core::PFPContext;

void exit_usage(const char * name) {
  cout << "PFPSim-generated simulation model:" << endl
      << "Usage:" << endl
      << "   " << name
      << " [(-c|--config-root) <path>] [(-v|--verbosity) ] [-X<option>]+"
      << endl
      << "   " << name << " --help|-h" << endl;
  exit(1);
}

void parse_args(int argc, char **argv, std::string & config,
      std::string& verbose_level,
      std::vector<std::string> & user_args,
      std::string& outputdir,
      bool& debugger_enabled) {
  static struct option long_options[] = {
      {"config-root" , required_argument , 0 , 'c' } ,
      {"verbosity"   , required_argument , 0 , 'v' } ,
      {"help"        , no_argument       , 0 , 'h' } ,
      {"output"      , required_argument , 0 , 'o' } ,
      {"debugger"     , no_argument      , 0 , 'd' } ,
      {"UserOpt"     , optional_argument , 0 , 'X' } ,
      {0             , 0                 , 0 ,  0  }
  };
  int c;
  const char * config_root = NULL;
  const char * verbosity = NULL;
  const char * output = NULL;
  do {
    c = getopt_long(argc, argv, "hc:X:v:o:d", long_options, NULL);
    switch (c) {
    case 'h':
      // If they want help give it to them then exit
      exit_usage(argv[0]);
    case 'c':
      if (config_root == NULL) {
        config_root = optarg;  // optarg is magically filled
      } else {
        // Make sure it's only specified once
        exit_usage(argv[0]);
      }
      break;
    case 'v':
      if (verbosity == NULL) {
        verbosity = optarg;  // http://linux.die.net/man/3/getopt_long
      } else {
        exit_usage(argv[0]);  // Make sure it's only specified once in the input
      }
      break;
    case 'o':
      if (output == NULL) {
        output = optarg;
      } else {
        exit_usage(argv[0]);  // Make sure it's only specified once in the input
      }
      break;
    case 'X':
      user_args.push_back(std::string(optarg)+"="+std::string(argv[optind++]));
      break;
    case 'd':
    {
      debugger_enabled = true;
      break;
    }
    case '?':
      // Some bad argument was passed, getopt will print
      // an error message, we'll just remind about the usage
      // then exit
      exit_usage(argv[0]);
    case -1: break;  // we're done
    default:
      cout << "Internal Error. ?? getopt returned " << c << endl;
    }
  } while (c != -1);

  if (optind < argc) {
    cout << "Unexpected argument " <<  argv[optind] << endl;
    exit_usage(argv[0]);
  }
  if (!config_root) {
    config_root = "./Configs/";
  }
  if (!verbosity) {
    verbosity = "normal";
  }
  if (!output) {
    output = "./";
  }
  config = config_root;
  verbose_level = verbosity;
  outputdir = output;
}

extern int pfp_main(int sc_argc, char* sc_argv[]);

void pfp_pause() {
  sc_pause();
}

void pfp_start() {
  PFPContext::get_current_context().ensure_top_initialized();
  sc_start();
}

/**
 * Called from pfp_boot.cpp from sc_main()
 * @param  sc_argc [description]
 * @param  sc_argv [description]
 * @return         [description]
 */
int pfp_elab_and_sim(int sc_argc, char* sc_argv[]) {
  std::cout.precision(3);
  std::vector<std::string> user_args;
  std::string config_root;
  std::string verbosity_level;
  std::string output_dir;
  bool debugger_enabled = false;

  parse_args(sc_argc, sc_argv,
            config_root,
            verbosity_level,
            user_args,
            output_dir,
            debugger_enabled);

  size_t slash_pos = config_root.rfind('/');

  if (slash_pos == std::string::npos || slash_pos != (config_root.size() - 1)) {
    config_root += '/';
  }

  cout << "config root is: " << config_root << endl;
  cout << "Verbostiy Level is: " << verbosity_level <<endl;
  if (user_args.size()) {
    cout << "User args" << endl;
    for (std::string opt : user_args) {
      cout << "  " <<  opt << endl;
    }
  }
  cout << "Output dir is: " << output_dir <<endl;

  pfp::core::PFPConfig::get().set_verbose_level(verbosity_level);
  SPSETCONFIGPATH(config_root);
  SPSETOUTPUTDIRPATH(output_dir);
  SPSETARGS(user_args);
  SET_PFP_DEBUGGER_FLAG(debugger_enabled);

  auto returnval = pfp_main(sc_argc, sc_argv);

  return returnval;
}

