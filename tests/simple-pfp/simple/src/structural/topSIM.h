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

#ifndef topSIM_H_
#define topSIM_H_
#include "pfpsim/pfpsim.h"
#include "../behavioural/writer.h"
#include "../behavioural/reader.h"
#include "../behavioural/queue.h"

class topSIM:
  public pfp::core::PFPObject,
  public sc_module
{
public:
  /*Constructor*/
  topSIM(sc_module_name& nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");
  /*Destructor*/
  virtual ~topSIM() = default;
  /*PEInstances*/
  std::shared_ptr<reader> r;
  std::shared_ptr<writer> w;
  /*CEInstances*/
  queue<std::shared_ptr<pfp::core::TrType>> q;
protected:
  virtual void init_SIM() final; /* Empty Function - Reserved for future use*/
};
#endif /*topSIM_H_*/
