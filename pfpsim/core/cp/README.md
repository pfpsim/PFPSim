Control Plane Command Parser
-------------------

This is a parser which accepts control-plane commands, for example insert_entry
which should be executed by the switch. It's a complete rewrite of the old
`MatchActionConfig.cpp` hand-written parser to be easier to understand and
to extend, and less brittle.


Adding new commands
----------------------
Adding new commands should be relatively straightforward.

  - Create a new subclass of Command to represent your command
    - Don't forget to override the `process` function. For convenience this can
      be done with the `OVERRIDE_PROCESS` macro in the class declaration, and by
      the `PROCESS` macro in the `.cpp` file
  - Register your command with the ControlPlaneAgent interface by adding a
    `DECLARE_PROCESS` line.
  - In `grammar.y`:
    - Create a polymorphic type for your command in the main `%polymorphic`
      section.
    - Create a token for your command and associate it to your type in the
      `Terminal types` section.
    - Create a production for your command. Follow the examples of `insert_entry_command`
      and the other pre-existing commands.
    - Add your command grammar rule to the overall `command` production.
  - In `tokens.l` create a new reserved word for the command. Look at the
    insert_entry for an example.
  - That's it.

