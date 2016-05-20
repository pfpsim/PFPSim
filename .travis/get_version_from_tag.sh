#!/usr/bin/env bash
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


# If a tag is not set, return 0
if [ -z "$TRAVIS_TAG" ]
then
  complete=0.0.0
  major=0
  minor=0
  patch=0
else
  # Split the version number on dots, storing it in an array
  complete=$(echo $TRAVIS_TAG | tr -d v )
  major=$(echo $complete | awk 'BEGIN {FS="."} {print $1}')
  minor=$(echo $complete | awk 'BEGIN {FS="."} {print $2}')
  patch=$(echo $complete | awk 'BEGIN {FS="."} {print $3}')
fi


case $1 in
  major)    echo -n $major;;
  minor)    echo -n $minor;;
  patch)    echo -n $patch;;
  complete) echo -n $complete;;
  *)        echo -n $complete;;
esac
