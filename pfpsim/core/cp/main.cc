#include <cassert>
#include <memory>
#include <iostream>
#include "CommandParser.h"
#include "Commands.h"

using namespace std;
using namespace pfp::cp;

class MiniCPA : public CommandProcessor {
  protected:
    std::shared_ptr<CommandResult> process(InsertCommand * cmd) override {
      cout << "CPA Received an insert command:" << endl;
      cmd->debug_print();
      return std::shared_ptr<CommandResult>(new InsertResult(cmd->shared_from_this(), 1));
    }

    std::shared_ptr<CommandResult> process(ModifyCommand * cmd) override {
      cout << "CPA Received a modify command:" << endl;
      cmd->debug_print();
      return std::shared_ptr<CommandResult>(new ModifyResult(cmd->shared_from_this()));;
    }

    std::shared_ptr<CommandResult> process(DeleteCommand * cmd) override {
      cout << "CPA Received a delete command:" << endl;
      cmd->debug_print();
      return std::shared_ptr<CommandResult>(new DeleteResult(cmd->shared_from_this()));
    }

    std::shared_ptr<CommandResult> process(BootCompleteCommand * cmd) override {
      cout << "CPA Received a boot-complete command:" << endl;
      cmd->debug_print();
      return nullptr;
    }

};


int main(int argc, const char *argv[])
{
  pfp::cp::CommandParser parser;
  MiniCPA cpa;

  std::string line;
  do {
    std::getline(std::cin, line);
    const auto cmd = parser.parse_line(line);

    if (cmd.get()) {
      cpa.accept_command(cmd);
    } else {
      cout << "Skipping comment line" << endl;
    }

  } while( ! std::cin.eof() );

  cpa.accept_command(std::make_shared<BootCompleteCommand>());

  return 0;
}
