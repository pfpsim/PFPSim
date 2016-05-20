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

%namespace pfp_cp

%scanner Scanner.h
%scanner-token-function d_scanner.lex()

/* There are 2 shift-reduce conflict created by allowing trailing whitespace */
/* these are for the productions ending in an action spec, because they end  */
/* with a whitespace-seperated list of bytes, so it's not immediately clear  */
/* if there will be another argument following whitespace, or if it is just  */
/* extra. */
/* */
/* There's a final shift-reduce conflict in the rule for comments and blank  */
/* lines, since actually every symbol is is nullable (I think that's why) */
%expect 3
             /* Basic data types*/
%polymorphic INTEGER           : unsigned long long;
             BYTES             : Bytes;
             STRING            : std::string;

             /* Action type, defined in Commands.h */
             ACTION_SPEC       : Action *;

             /* Match Key Types, defined in Commands.h */
             MATCH_KEY         : MatchKey  *;
             EXACT_KEY_PARAM   : ExactKey *;
             LPM_KEY_PARAM     : LpmKey *;
             TERNARY_KEY_PARAM : TernaryKey *;

             /* Group of Match Keys */
             MATCH_KEYS        : MatchKeyContainer *;

             /* Command Types, defined in Commands.h */
             COMMAND           : Command    *;
             INSERT_COMMAND    : InsertCommand *;
             MODIFY_COMMAND    : ModifyCommand *;
             DELETE_COMMAND    : DeleteCommand *;


/***** Nonterminal types *****/
/* Keys */
%type <EXACT_KEY_PARAM>   exact_match_key
%type <LPM_KEY_PARAM>     lpm_match_key
%type <TERNARY_KEY_PARAM> ternary_match_key
%type <MATCH_KEY>      match_key
%type <MATCH_KEYS>     match_keys

/* Actions */
%type <ACTION_SPEC>    action_spec
%type <BYTES>          action_param

/* Commands */
%type <INSERT_COMMAND> insert_entry_command
%type <MODIFY_COMMAND> modify_entry_command
%type <DELETE_COMMAND> delete_entry_command
%type <COMMAND>        command

/* Data */
%type <BYTES>          bytes
%type <BYTES>          raw_bytes
%type <BYTES>          ip_addr


/***** Terminal types *****/
/* Table or action names */
%token                 IDENTIFIER
%type <STRING>         IDENTIFIER

/* Reserved words */
%token                 INSERT_ENTRY
%type <INSERT_COMMAND> INSERT_ENTRY

%token                 MODIFY_ENTRY
%type <MODIFY_COMMAND> MODIFY_ENTRY

%token                 DELETE_ENTRY
%type <DELETE_COMMAND> DELETE_ENTRY

/* Literal values */
%token                 DECIMAL
%type <INTEGER>        DECIMAL

%token                 HEXADECIMAL
%type <BYTES>          HEXADECIMAL

/* Whitespace tokens */
%token __ EOL
%token COMMENT


%%

/* A sentence in our language is just a single command.
 * We will also allow leading and trailing spaces and
 * a trailing newline, to avoid being overly sensitive.
 */
startrule:
  optional_space command optional_EOL {
    returnCommand($2);
  }|
  optional_space optional_comment optional_EOL {
    returnCommand(nullptr);
  };

/* The list of the commands we recognize */
/* TODO: there must be a more elegant way of dealing with this casting */
/* in bisonc++ 4.09 this is unnecassary but in 4.05 (ubuntu 14.04's version)*/
/* we have to do this... :( */
command:
  insert_entry_command{
    $$ = static_cast<Command*>($1);
  }|
  modify_entry_command{
    $$ = static_cast<Command*>($1);
  }|
  delete_entry_command{
    $$ = static_cast<Command*>($1);
  };

/* Command Formats */
insert_entry_command:
  INSERT_ENTRY __ IDENTIFIER __ match_keys __ action_spec {
    $1 ->set_table_name($3);
    $1 ->set_match_keys($5);
    $1 ->set_action($7);
    $$ = $1;
  };

modify_entry_command:
  MODIFY_ENTRY __ IDENTIFIER __ DECIMAL __ action_spec {
    $1 -> set_table_name($3);
    $1 -> set_handle($5);
    $1 -> set_action($7);
    $$ = $1;
  };

delete_entry_command:
  DELETE_ENTRY __ IDENTIFIER __ DECIMAL {
    $1 -> set_table_name($3);
    $1 -> set_handle($5);
    $$ = $1;
  };

/* match_keys is a collection of zero or more match keys */
match_keys:
  /* Create an empty list */
  empty {
    $$ = new MatchKeyContainer();
  }|
  /* Create a list with one key in it */
  match_key {
    $$ = new MatchKeyContainer();
    $$ -> push_back(MatchKeyUPtr($1));
  }|
  /* Add a key to the existing list */
  match_keys __ match_key {
    $$ = $1;
    $$ -> push_back(MatchKeyUPtr($3));
  };

/* The types of match keys we can represent */
/* TODO same problem as Commands above, there must be a better way */
match_key:
  exact_match_key{
    $$ = static_cast<MatchKey*>($1);
  }|
  lpm_match_key{
    $$ = static_cast<MatchKey*>($1);
  }|
  ternary_match_key{
    $$ = static_cast<MatchKey*>($1);
  };

/* An exact match key is just any chunk of bytes */
exact_match_key:
  bytes {
    $$ = new ExactKey($1);
  };

/* An LPM key is a chunk of bytes a slash and then a decimal prefix length */
/* Note that no spaces are permitted */
lpm_match_key:
  bytes '/' DECIMAL {
    $$ = new LpmKey($1, $3);
  };

/* A ternary key is a chunk of bytes an '&' and another chunk of bytes */
/* Note that no spaces are permitted */
ternary_match_key:
  bytes '&' bytes {
    $$ = new TernaryKey($1, $3);
  };

/* An action spec is an action name, followed by a list of action parameters */
action_spec:
  /* An identifier of the action name creates a new Action */
  IDENTIFIER {
    $$ = new Action($1);
  }|
  /* Add a new parameter to the existing action spec */
  action_spec __ action_param {
    $$ = $1;
    $$ ->add_param($3);
  };

/* for now an action param is just any sequence of bytes */
action_param: bytes;

/* bytes is any literal, with an optional resizing */
bytes:
  /* bytes without resizing */
  raw_bytes|
  /* bytes, resized to a specified length */
  raw_bytes '\'' DECIMAL {
    auto newsize = $3;
    auto bytes   = $1;

    // Resize used to work because the important part of the data was
    // at the beginning, now it's at the end so we need to copy it forward

    auto delta = std::abs((int)bytes.size() - (int)newsize);

    if (newsize < bytes.size()) {
      // we're shrinking the data so we need to copy the range
      // [delta, max_index] to [0, max_index-delta]

      // copy data forward
      std::copy(bytes.begin() + delta, bytes.end(), bytes.begin());
      // shrink to the new size
      bytes.resize(newsize);
    } else if(newsize > bytes.size()) {
      // Here we're growing the data so we need to copy the range
      // [0, max_index] to [delta, max_index+delta]

      // TODO - there's some problem with this that comes up under valgrind
      // shows up only for size >= 16 ... :S

      // expand to the new size
      bytes.resize(newsize);
      // copy data to the end
      std::copy(bytes.begin(), bytes.end()+1-delta, bytes.begin()+delta);
      // Zero out the beginning (where the data was before
      std::fill(bytes.begin(), bytes.end()+1-delta, 0x00);
    }

    //$1 .resize($3, 0x00);
    $$ = bytes;
  };

/* Any literal representation which can be converted to a sequence of bytes */
raw_bytes:
  DECIMAL {
    Bytes ret_val;
    // We cast the unsigned long long to a buffer of bytes
    uint8_t * buf = (uint8_t*) & $1;
    size_t len    = sizeof $1;

    // We then just copy that buffer into our vector
    std::reverse_copy(buf, buf+len, back_inserter(ret_val));

    $$ = ret_val;
  }|
  HEXADECIMAL|
  ip_addr;

/* An ipv4 address in the usual format */
/* We parse it in the grammar instead of as a terminal token for the sake
   of simplicity */
ip_addr:
  DECIMAL '.' DECIMAL '.' DECIMAL '.' DECIMAL
  {
    // Simply create a vector of the four decimal values
    $$ = Bytes{
                (uint8_t)$1,
                (uint8_t)$3,
                (uint8_t)$5,
                (uint8_t)$7};
  };

/* Utility productions */
optional_space: __ | empty;
optional_EOL: optional_space EOL | optional_space;
optional_comment: COMMENT | empty;
empty: ;
