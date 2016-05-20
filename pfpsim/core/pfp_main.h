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

#ifndef CORE_PFP_MAIN_H_
#define CORE_PFP_MAIN_H_

#include <string>
#include <vector>

void exit_usage(const char * name);

void parse_args(int argc, char **argv, std::string & config,
      std::string& verbose_level,
      std::vector<std::string> & user_args,
      std::string& outputdir,
      bool& debugger_enabled);


void pfp_pause();

void pfp_start();

/**
 * Called from pfp_boot.cpp from sc_main()
 * @param  sc_argc [description]
 * @param  sc_argv [description]
 * @return         [description]
 */
int pfp_elab_and_sim(int sc_argc, char* sc_argv[]);

#endif  // CORE_PFP_MAIN_H_
