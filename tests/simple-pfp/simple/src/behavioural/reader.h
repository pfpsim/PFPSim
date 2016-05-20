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

#ifndef reader_H_
#define reader_H_
#include "../structural/readerSIM.h"
class reader: public readerSIM
{
public:
  SC_HAS_PROCESS(reader);
	/*Constructor*/
	reader(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");
	/*Destructor*/
	virtual ~reader() = default;
public:
	void init();
private:
	void reader_PortServiceThread();
	void readerThread(std::size_t thread_id);
	std::vector<sc_process_handle> ThreadHandles;
};
#endif /*reader_H_*/
