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

#ifndef PFPSIM_H_
#define PFPSIM_H_

//----------------------------- core -----------------------------//
#include "core/PFPConfig.h"
#include "core/PFPObject.h"
#include "core/StringUtils.h"
#include "core/TrType.h"
#include "core/PacketBase.h"
#include "core/MTQueue.h"
#include "core/LMTQueue.h"
#include "core/PFPObserver.h"
#include "core/ConfigurationParameters.h"
#include "core/promptcolors.h"
#include "core/json.hpp"
#include "core/DebuggerUtilities.h"
#include "core/pfp_main.h"

//------------------------- core/debugger -------------------------//
#include "core/debugger/CPDebuggerInterface.h"

//---------------------------- core/cp ----------------------------//
#include "core/cp/Commands.h"
#include "core/cp/CommandParser.h"

//---------------------------- doxygen ----------------------------//

/**
 * @namespace pfp
 * @brief Namespace for all PFPSim classes.
 */
namespace pfp {
  /**
   * @namespace pfp::core
   * @brief Namespace for all PFPSIMCore classes
   */
  namespace core {  // NOLINT(runtime/indentation_namespace)
    /**
     * @namespace pfp::core::db
     * @brief Namespace for all debugger classes within PFPSim.
     */
    namespace db {  // NOLINT(runtime/indentation_namespace)

    };  // namespace db
  };  // namespace core
  /**
   * @namespace pfp::cp
   * @brief Namespace for all ControlPlane Protocol
   */
  namespace cp {  // NOLINT(runtime/indentation_namespace)

  };  // namespace cp
};  // namespace pfp


/**
 * @mainpage
 *
 * @tableofcontents
 *
 * @section intro_sec Introduction
 * // TODO
 *
 * @section install_sec Installation
 * // TODO
 * @subsection dependencies Dependencies
 *   - See @ref debug_depends
 */

#endif  // PFPSIM_H_
