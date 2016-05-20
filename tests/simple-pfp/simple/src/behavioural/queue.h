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

#ifndef queue_H_
#define queue_H_

#include "../structural/queueSIM.h"
#include "qw.h"
#include "qr.h"

template<typename T>
class queue:
public qw<T>,
public qr<T>,
public queueSIM
{
public:
  /* CE Consturctor */
	queue(sc_module_name nm,pfp::core::PFPObject* parent = 0, std::string configfile="");
  /* User Implementation of the Virtual Functions in the Interface.h */

  void put(T v) override {
    q.push(v);
  }

  T get() override {
    T tmp;
    q.pop(tmp);
    return tmp;
  }

private:
  MTQueue<T> q;

};

/*
	queue Consturctor
 */
template<typename T>
queue<T>::queue(sc_module_name nm, pfp::core::PFPObject* parent, std::string configfile):
	queueSIM(nm,parent,configfile)
{

}

#endif /* queue_H_ */
