#!/bin/bash
#
# PFPSim: Library for the Programmable Forwarding Plane Simulation Framework
#
# Copyright (C) 2016 Concordia Univ., Montreal
#     Samar Abdi
#     Umair Aftab
#     Gordon Bailey
#     Faras Dewal
#     Shafigh Parsazad
#     Eric Tremblay
#
# Copyright (C) 2016 Ericsson
#     Bochra Boughzala
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#


# cpplint
# Script for running cpplint on a directory of code (.h and .cpp files only)
#
# Author: Eric Tremblay
# Created on: April 25th, 2016
#
# Usage:  ./run_lint.sh [pattern]
#      The optional pattern is used to grep the output. Useful if looking to fix a particular category of errors.

# Filters:  -x --> ignore category x
#      +y --> don't ignore category y
#      Separate each by a comma with no space
#      Example: -whitespace,+whitespace/braces  --> ignores all 'whitespace' errors except the 'whitespace/braces' errors.
filters="-legal/copyright,-runtime/references,-build/include_subdir,-build/c++11"

# Root Directory:  Used to determine appropriate header guards
#          The path must be relative to the location of the directory containing the .git file
#          Do not put a / at the end
#          Example: rmt/src  -->  Old header guards: RMT_SRC_BEHAVIOURAL_LOGGER_H_, New header guards: BEHAVIOURAL_LOGGER_H_
root_dir="pfpsim"

# Lint Directory:  Directory in which to start linting. The path is relative to where this script is run.
#          Examples: ../src or .
lint_dir="pfpsim"

# Exclude Directories:  Directories to exclude. Directories should be specified relative to the Lint Directory.
#            Use regex syntax. In particular, must escape all . and all /. Use | to separate multiple directories
#            Use NO_EXCLUDE_DIRS if there is nothing to exclude.
#            Example: \.\.\/build\/|\.\.\/src\/tries\/  --> excludes ../build and ../src/tries/
exclude_dirs="pfpsim\/doxygen\/|pfpsim\/core\/proto\/|pfpsim\/core\/cp\/CommandParser.cpp|pfpsim\/core\/cp\/CommandScanner.cpp|pfpsim\/core\/cp\/CommandParserbase.h|pfpsim\/core\/cp\/CommandScannerbase.h|pfpsim\/core\/cmake\/"

# Lint Command:    Command to run cpplint. Should not have to be modified.
lint_command="cpplint --root="${root_dir}" --counting=detailed --filter="${filters}" $( find "${lint_dir}" -name \*.h -or -name \*.cpp | grep -vE "${exclude_dirs}" )"

if [ $# -eq 0 ]; then
  ${lint_command}
else
  ${lint_command} 2>&1 | grep --color "$1"
fi
