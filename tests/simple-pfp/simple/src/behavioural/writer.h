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

#ifndef writer_H_
#define writer_H_
#include "../structural/writerSIM.h"
class writer: public writerSIM
{
public:
  SC_HAS_PROCESS(writer);
	/*Constructor*/
	writer(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");
	/*Destructor*/
	virtual ~writer() = default;
public:
	void init();
private:
	void writer_PortServiceThread();
	void writerThread(std::size_t thread_id);
	std::vector<sc_process_handle> ThreadHandles;
};
#endif /*writer_H_*/
