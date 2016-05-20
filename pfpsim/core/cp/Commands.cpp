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

#include "Commands.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cassert>

std::ostream & operator<< (std::ostream & os, const pfp::cp::Bytes & b) {
  // Save state of stream
  std::ios old_state(nullptr);
  old_state.copyfmt(os);

  // Apply our iomanips and print the hex prefix
  os << std::hex << std::setfill('0') << "0x";

  // Print out the hex representation of our Bytes in MSB first order
  for (auto it = b.rbegin(), end = b.rend(); it != end; ++it) {
    os << std::setw(2) << static_cast<int>(*it);
  }

  // Restore state of stream
  os.copyfmt(old_state);

  return os;
}

std::ostream & operator<< (std::ostream & os, const pfp::cp::MatchKey & mk) {
  mk.print(os);
  return os;
}

namespace pfp {
namespace cp {

Action::Action(std::string name) : name(name) {}

const std::string & Action::get_name() const {
  return name;
}

void Action::add_param(Bytes param) {
  params.push_back(param);
}

void Action::print() {
  std::cout << "   Action: " << name << "( " << std::endl;
  for (auto & p : params) {
    std::cout << "    " << p << std::endl;
  }
  std::cout << "   )" << std::endl;
}

const std::vector<Bytes> & Action::get_params() const {
  return params;
}

MatchKey::MatchKey(Bytes v)
  : data(v) {}

const Bytes & MatchKey::get_data() const {
  return data;
}

size_t MatchKey::get_prefix_len() const {
  assert(!"Not implemented");
  return 0;
}

const Bytes & MatchKey::get_mask() const {
  static Bytes b;
  assert(!"Not implemented");
  return b;
}

ExactKey::ExactKey(Bytes v)
  : MatchKey(v) {
  }

MatchKey::Type ExactKey::get_type() const {
  return MatchKey::Type::EXACT;
}

void ExactKey::print(std::ostream & os) const {
  os << data;
}

LpmKey::LpmKey(Bytes data, size_t prefix_len)
  :MatchKey(data), prefix_len(prefix_len) {
  }

MatchKey::Type LpmKey::get_type() const {
  return MatchKey::Type::LPM;
}

size_t LpmKey::get_prefix_len() const {
  return prefix_len;
}

void LpmKey::print(std::ostream & os) const {
  os << data << " / " << prefix_len;
}

TernaryKey::TernaryKey(Bytes data, Bytes mask)
  :MatchKey(data), mask(mask) { }

void TernaryKey::print(std::ostream & os) const {
  os << data << " & " << mask;
}

MatchKey::Type TernaryKey::get_type() const {
  return MatchKey::Type::TERNARY;
}

const Bytes & TernaryKey::get_mask() const {
  return mask;
}


void Command::set_table_name(std::string s) {
  table_name = s;
}

const std::string & Command::get_table_name() const {
  return table_name;
}

std::shared_ptr<CommandResult> Command::failure_result() {
  return std::shared_ptr<CommandResult>(
        new FailedResult(this->shared_from_this()));
}

void InsertCommand::print() {
  std::cout << "Insert command!" << std::endl
    << "  table: " << table_name << std::endl;
  for (auto & k : *keys) {
    std::cout << "  " << *k << std::endl;
  }
  action->print();
}

void InsertCommand::set_match_keys(MatchKeyContainer * v) {
  keys.reset(v);
}

void InsertCommand::set_action(Action * a) {
  action.reset(a);
}

const MatchKeyContainer & InsertCommand::get_keys() const {
  return *keys;
}

const Action & InsertCommand::get_action() const {
  return * action;
}

std::shared_ptr<CommandResult> InsertCommand::success_result(size_t handle) {
  return std::shared_ptr<CommandResult>(
        new InsertResult(this->shared_from_this(), handle));
}

void ModifyCommand::print() {
  std::cout << "Modify command!" << std::endl
    << "  table : " << table_name << std::endl
    << "  handle: " << handle << std::endl;
  action->print();
}

void ModifyCommand::set_handle(size_t h) {
  handle = h;
}

size_t ModifyCommand::get_handle() const {
  return handle;
}

void ModifyCommand::set_action(Action * a) {
  action.reset(a);
}

const Action & ModifyCommand::get_action() const {
  return *action;
}

std::shared_ptr<CommandResult> ModifyCommand::success_result() {
  return std::shared_ptr<CommandResult>(
        new ModifyResult(this->shared_from_this()));
}

void DeleteCommand::print() {
  std::cout << "Delete command!" << std::endl
    << "  table : " << table_name << std::endl
    << "  handle: " << handle << std::endl;
}
void DeleteCommand::set_handle(size_t h) {
  handle = h;
}

size_t DeleteCommand::get_handle() const {
  return handle;
}

std::shared_ptr<CommandResult> DeleteCommand::success_result() {
  return std::shared_ptr<CommandResult>(
        new DeleteResult(this->shared_from_this()));
}

void BootCompleteCommand::print() {
  std::cout << "Boot Complete Command!" << std::endl;
}

// Control plane agent passes itself to the message ...
std::shared_ptr<CommandResult>
CommandProcessor::accept_command(const std::shared_ptr<Command> & cmd) {
  return cmd->process(this);
}

// The message then passes itself back to the Control Plane Agent with the
// correct type
#define PROCESS(TYPE) \
  std::shared_ptr<CommandResult> TYPE::process(CommandProcessor * cp) { \
    return cp->process(this); \
  }

PROCESS(InsertCommand)
PROCESS(ModifyCommand)
PROCESS(DeleteCommand)
PROCESS(BootCompleteCommand)

#undef PROCESS


void ResultProcessor::accept_result(
      const std::shared_ptr<CommandResult> & res) {
  return res->process(this);
}

CommandResult::CommandResult(std::shared_ptr<Command> cmd)
  : command(cmd) {}

InsertResult::InsertResult(std::shared_ptr<Command> cmd, size_t handle)
  : CommandResult(cmd), handle(handle) {}

#define PROCESS(TYPE) \
    void TYPE::process(ResultProcessor * rp) { \
      return rp->process(this); \
    }

PROCESS(InsertResult)
PROCESS(ModifyResult)
PROCESS(DeleteResult)
PROCESS(FailedResult)

#undef PROCESS

};  // namespace cp
};  // namespace pfp
