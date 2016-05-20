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

#ifndef CORE_PFPCONTEXT_H_
#define CORE_PFPCONTEXT_H_

#include "PFPObject.h"

namespace pfp {
namespace core {

class PFPContext {
 public:
  void ensure_top_initialized();

  static PFPContext & get_current_context();

 private:
  PFPContext() = default;

  std::unique_ptr<PFPObject> top_instance{nullptr};
  static std::unique_ptr<PFPContext> instance;
};

}  // namespace core
}  // namespace pfp

#endif  // CORE_PFPCONTEXT_H_
