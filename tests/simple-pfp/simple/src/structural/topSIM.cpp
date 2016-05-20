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

#include "topSIM.h"
/*PE SIM Constructor*/
topSIM::topSIM(sc_module_name& nm, pfp::core::PFPObject* parent,std::string configfile ):
  r (std::make_shared<reader>("r",this)),
  w (std::make_shared<writer>("w",this)),
  q ("q",this),
  pfp::core::PFPObject(pfp::core::convert_to_string(nm), parent),
  sc_module(nm)
{
	/* Bindings */
  r->input.bind(q);
  w->output.bind(q);
  AddChildModule(r->module_name(),r.get());
  AddChildModule(w->module_name(),w.get());
}
/* Empty Function - Reserved for future use*/
void topSIM::init_SIM(){
	/*Init Sub PE instances*/
	r->init();
	w->init();
}
