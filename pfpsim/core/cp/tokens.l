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

%%

insert_entry {
  // Reserved word, creates an empty command object
  returnValue(new InsertCommand());
  returnToken(INSERT_ENTRY);
}

modify_entry {
  // Reserved word, creates an empty command object
  returnValue(new ModifyCommand());
  returnToken(MODIFY_ENTRY);
}

delete_entry {
  // Reserved word, creates an empty command object
  returnValue(new DeleteCommand());
  returnToken(DELETE_ENTRY);
}

begin\ transaction {
  // reserved phrase, creates an empty command object
  returnValue(new BeginTransactionCommand());
  returnToken(BEGIN_TRANSACTION);
}

end\ transaction {
  // reserved phrase, creates an empty command object
  returnValue(new EndTransactionCommand());
  returnToken(END_TRANSACTION);
}

#.* {
  returnToken(COMMENT);
}

[[:digit:]]+ {
  // Matches a positive decimal literal

  // We attempt to convert the literal to an unsigned long long
  // since its the biggest type we can easily use without having
  // to mess around with bignum libraries, and probably no one
  // will need to specify a number that big anyways.
  unsigned long long int_val;
  try {
    int_val = std::stoull(matched());
  }catch(std::out_of_range){
    // If the value is too big, print an error (TODO npulog?) and
    // return a NULL to cause a syntax error
    std::cerr << "Decimal literal " << matched() << " is too big!" << std::endl;
    return '\0';
  }

  // Return the value as a DECIMAL token
  returnValue( int_val );
  returnToken(DECIMAL);
}

[[:xdigit:]]{2}(:[[:xdigit:]]{2})+ {
  // Matches a string of bytes specified as hexadecimal with bytes
  // seperated from eachother with colons. This is to match the typical
  // format for MAC addresses, but is more general, allowing longer
  // and shorter bytes strings to be specified as well.

  // We parse the matched string into a vector of uint8_t (Bytes)
  std::string s = matched();
  Bytes ret_val;

  // We will parse bytes starting from the first one, so that we
  // end up with an array in MSB first order.

  // This is the position of the first byte in the string
  size_t pos = 0;

  // Now we loop over, incrementing by three characters (two hex digits
  // and a colon), converting each pair of hex digits to an integer and
  // pushing it back into our Bytes vector.
  while (pos < s.size()) {
    ret_val.push_back( (uint8_t) std::stoi( s.substr(pos, 2), nullptr, 16 ));
    pos += 3;
  }

  // Return the Bytes as a HEXADECIMAL token
  returnValue(ret_val);
  returnToken(HEXADECIMAL);
}

[[:alpha:]_][[:alpha:][:digit:]_]* {
  // Matches an identifier, which could be the name of a table
  // or an action. Standard regex for a c-style variable name
  returnValue(matched());
  returnToken(IDENTIFIER);
}

[ \t]+ {
  // Spaces are returned as tokens so that we can have constructs
  // in the grammar which are built out of multiple tokens, but in
  // which we don't want to allow spaces. One example is LPM keys
  // such as 192.168.0.1/23 where we do not allow spaces between
  // data, '/' character, and prefix. Another example is any data
  // where we explicitly specify a length, for example 123'4, where
  // again we don't allow spaces.
  returnToken(__);
}

\n {
  // Explicitly handle newlines, because we don't want to allow them.
  returnToken(EOL);
}

. {
  // This token is just here for debugging purposes, it simply does
  // what would already be done automatically, that is literally
  // returning any character not otherwise matched. The reason it's
  // here is so that we can optionally print out the returned character
  // for debugging. Unfortunately we can't use our usual returnToken
  // macro, since it expects the name of a token.
  #ifdef SCANNER_DEBUG
  std::cout << '\'' << matched()[0] << '\'' << std::endl;
  #endif
  return matched()[0];
}
