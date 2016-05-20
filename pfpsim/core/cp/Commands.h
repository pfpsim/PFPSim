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

#ifndef CORE_CP_COMMANDS_H_
#define CORE_CP_COMMANDS_H_

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>

namespace pfp {
namespace cp {

typedef std::vector<uint8_t> Bytes;

// Command Parameter types


class Action {
 public:
  explicit Action(std::string name);
  void add_param(Bytes param);
  void print();
  const std::vector<Bytes> & get_params() const;
  const std::string & get_name() const;
 private:
  std::string name;
  std::vector<Bytes> params;
};

class MatchKey {
 public:
  enum Type {
    EXACT, LPM, TERNARY
  };

  explicit MatchKey(Bytes v);

  virtual void print(std::ostream & os) const = 0;
  virtual ~MatchKey() = default;

  const Bytes & get_data() const;
  virtual size_t get_prefix_len() const;
  virtual const Bytes & get_mask() const;
  virtual Type get_type() const = 0;
 protected:
  const Bytes data;
};


class ExactKey : public MatchKey {
 public:
  explicit ExactKey(Bytes v);
  void print(std::ostream & os) const override;
  virtual ~ExactKey() = default;
  Type get_type() const override;
};

class LpmKey : public MatchKey {
 public:
  LpmKey(Bytes data, size_t prefix_len);
  void print(std::ostream & os) const override;
  virtual ~LpmKey() = default;
  size_t get_prefix_len() const override;
  Type get_type() const override;

 private:
  const size_t prefix_len;
};

class TernaryKey : public MatchKey {
 public:
  TernaryKey(Bytes data, Bytes mask);
  void print(std::ostream & os) const override;
  virtual ~TernaryKey() = default;
  const Bytes & get_mask() const override;
  Type get_type() const override;

 private:
  const Bytes mask;
};

typedef std::unique_ptr<MatchKey>          MatchKeyUPtr;
typedef std::vector<MatchKeyUPtr>          MatchKeyContainer;

// Commands

class CommandProcessor;
class CommandResult;
class ResultProcessor;

// enable_shared_from_this allows the commands to pass a smart pointer to
// themselves to the result that they generate, so that the processor of the
// result has information about the command it came from
class Command : public std::enable_shared_from_this<Command> {
 public:
  virtual void print() = 0;
  virtual std::shared_ptr<CommandResult> process(CommandProcessor *p) = 0;
  virtual void set_table_name(std::string s);
  const std::string & get_table_name() const;
  virtual ~Command() = default;

  // Generate a failure response
  std::shared_ptr<CommandResult> failure_result();

 protected:
  std::string table_name;
};

#define OVERRIDE_PROCESS() \
  std::shared_ptr<CommandResult> process(CommandProcessor *p) override

class InsertCommand : public Command {
 public:
  void print() override;
  OVERRIDE_PROCESS();
  void set_match_keys(MatchKeyContainer * v);
  void set_action(Action * a);
  virtual ~InsertCommand() = default;
  const MatchKeyContainer & get_keys() const;
  const Action & get_action() const;

  // Build a response for a successful insert command. The handle should
  // be some type of unique identifier for the inserted entry.
  std::shared_ptr<CommandResult> success_result(size_t handle);

 private:
  std::unique_ptr<MatchKeyContainer> keys;
  std::unique_ptr<Action> action;
};

class ModifyCommand : public Command {
 public:
  void print() override;
  OVERRIDE_PROCESS();
  void set_handle(size_t h);
  size_t get_handle() const;
  void set_action(Action * a);
  const Action & get_action() const;
  virtual ~ModifyCommand() = default;

  std::shared_ptr<CommandResult> success_result();

 private:
  size_t handle;
  std::unique_ptr<Action> action;
};

class DeleteCommand : public Command {
 public:
  void print() override;
  OVERRIDE_PROCESS();
  void set_handle(size_t h);
  size_t get_handle() const;
  virtual ~DeleteCommand() = default;

  std::shared_ptr<CommandResult> success_result();

 private:
  size_t handle;
};

class BootCompleteCommand : public Command {
 public:
  void print() override;
  OVERRIDE_PROCESS();
  virtual ~BootCompleteCommand() = default;
};

#undef OVERRIDE_PROCESS

// CommandProcessor

#define DECLARE_PROCESS(TYPE)                               \
    friend class TYPE;                                      \
    virtual std::shared_ptr<CommandResult> process(TYPE* t) = 0

class CommandProcessor {
 public:
  std::shared_ptr<CommandResult>
  accept_command(const std::shared_ptr<Command> & cmd);

 protected:
  DECLARE_PROCESS(InsertCommand);
  DECLARE_PROCESS(ModifyCommand);
  DECLARE_PROCESS(DeleteCommand);
  DECLARE_PROCESS(BootCompleteCommand);
};
#undef DECLARE_PROCESS


// Results

#define OVERRIDE_PROCESS() \
  void process(ResultProcessor *p) override

class CommandResult {
 public:
  virtual ~CommandResult() = default;
  virtual void process(ResultProcessor *p) = 0;

  explicit CommandResult(std::shared_ptr<Command> cmd);

 public:
  const std::shared_ptr<Command> command;
};

class InsertResult : public CommandResult {
 public:
  virtual ~InsertResult() = default;
  OVERRIDE_PROCESS();

  InsertResult(std::shared_ptr<Command> cmd, size_t handle);

  const size_t handle;
};

class ModifyResult : public CommandResult {
 public:
  virtual ~ModifyResult() = default;
  OVERRIDE_PROCESS();

  using CommandResult::CommandResult;
};

class DeleteResult : public CommandResult {
 public:
  virtual ~DeleteResult() = default;
  OVERRIDE_PROCESS();

  using CommandResult::CommandResult;
};

class FailedResult : public CommandResult {
 public:
  virtual ~FailedResult() = default;
  OVERRIDE_PROCESS();

  using CommandResult::CommandResult;

  const std::string message;
};
#undef OVERRIDE_PROCESS

#define DECLARE_PROCESS(TYPE)     \
  friend class TYPE;            \
  virtual void process(TYPE* t) = 0

class ResultProcessor {
 public:
  void accept_result(const std::shared_ptr<CommandResult> & res);

 protected:
  DECLARE_PROCESS(ModifyResult);
  DECLARE_PROCESS(InsertResult);
  DECLARE_PROCESS(DeleteResult);
  DECLARE_PROCESS(FailedResult);
};
#undef DECLARE_PROCESS


};  // namespace cp
};  // namespace pfp

std::ostream & operator<< (std::ostream & os, const pfp::cp::Bytes & b);
std::ostream & operator<< (std::ostream & os, const pfp::cp::MatchKey & k);

#endif  // CORE_CP_COMMANDS_H_
