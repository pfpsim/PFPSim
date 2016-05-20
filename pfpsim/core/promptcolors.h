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

/*
 * promptcolors.h
 *
 *  Created on: Nov 13, 2015
 *      Author: Lemniscate Snickets
 */

#ifndef CORE_PROMPTCOLORS_H_
#define CORE_PROMPTCOLORS_H_

/*
 * Adapted from https://wiki.archlinux.org/index.php/Color_Bash_Prompt
 */

#define txtblk "\e[0;30m"  // Black - Regular
#define txtred "\e[0;31m"  // Red
#define txtgrn "\e[0;32m"  // Green
#define txtylw "\e[0;33m"  // Yellow
#define txtblu "\e[0;34m"  // Blue
#define txtpur "\e[0;35m"  // Purple
#define txtcyn "\e[0;36m"  // Cyan
#define txtwht "\e[0;37m"  // White
#define bldblk "\e[1;30m"  // Black - Bold
#define bldred "\e[1;31m"  // Red
#define bldgrn "\e[1;32m"  // Green
#define bldylw "\e[1;33m"  // Yellow
#define bldblu "\e[1;34m"  // Blue
#define bldpur "\e[1;35m"  // Purple
#define bldcyn "\e[1;36m"  // Cyan
#define bldwht "\e[1;37m"  // White
#define unkblk "\e[4;30m"  // Black - Underline
#define undred "\e[4;31m"  // Red
#define undgrn "\e[4;32m"  // Green
#define undylw "\e[4;33m"  // Yellow
#define undblu "\e[4;34m"  // Blue
#define undpur "\e[4;35m"  // Purple
#define undcyn "\e[4;36m"  // Cyan
#define undwht "\e[4;37m"  // White
#define bakblk "\e[40m"    // Black - Background
#define bakred "\e[41m"    // Red
#define bakgrn "\e[42m"    // Green
#define bakylw "\e[43m"    // Yellow
#define bakblu "\e[44m"    // Blue
#define bakpur "\e[45m"    // Purple
#define bakcyn "\e[46m"    // Cyan
#define bakwht "\e[47m"    // White
#define txtrst "\e[0m"     // Text Reset

// Reset
#define Color_Off "\e[0m"       // Text Reset

// Regular Colors
#define Black "\e[0;30m"        // Black
#define Red "\e[0;31m"          // Red
#define Green "\e[0;32m"        // Green
#define Yellow "\e[0;33m"       // Yellow
#define Blue "\e[0;34m"         // Blue
#define Purple "\e[0;35m"       // Purple
#define Cyan "\e[0;36m"         // Cyan
#define White "\e[0;37m"        // White

// Bold
#define BBlack "\e[1;30m"       // Black
#define BRed "\e[1;31m"         // Red
#define BGreen "\e[1;32m"       // Green
#define BYellow "\e[1;33m"      // Yellow
#define BBlue "\e[1;34m"        // Blue
#define BPurple "\e[1;35m"      // Purple
#define BCyan "\e[1;36m"        // Cyan
#define BWhite "\e[1;37m"       // White

// Underline
#define UBlack "\e[4;30m"       // Black
#define URed "\e[4;31m"         // Red
#define UGreen "\e[4;32m"       // Green
#define UYellow "\e[4;33m"      // Yellow
#define UBlue "\e[4;34m"        // Blue
#define UPurple "\e[4;35m"      // Purple
#define UCyan "\e[4;36m"        // Cyan
#define UWhite "\e[4;37m"       // White

// Background
#define On_Black "\e[40m"       // Black
#define On_Red "\e[41m"         // Red
#define On_Green "\e[42m"       // Green
#define On_Yellow "\e[43m"      // Yellow
#define On_Blue "\e[44m"        // Blue
#define On_Purple "\e[45m"      // Purple
#define On_Cyan "\e[46m"        // Cyan
#define On_White "\e[47m"       // White

// High Intensity
#define IBlack "\e[0;90m"       // Black
#define IRed "\e[0;91m"         // Red
#define IGreen "\e[0;92m"       // Green
#define IYellow "\e[0;93m"      // Yellow
#define IBlue "\e[0;94m"        // Blue
#define IPurple "\e[0;95m"      // Purple
#define ICyan "\e[0;96m"        // Cyan
#define IWhite "\e[0;97m"       // White

// Bold High Intensity
#define BIBlack "\e[1;90m"      // Black
#define BIRed "\e[1;91m"        // Red
#define BIGreen "\e[1;92m"      // Green
#define BIYellow "\e[1;93m"     // Yellow
#define BIBlue "\e[1;94m"       // Blue
#define BIPurple "\e[1;95m"     // Purple
#define BICyan "\e[1;96m"       // Cyan
#define BIWhite "\e[1;97m"      // White

// High Intensity backgrounds
#define On_IBlack "\e[0;100m"   // Black
#define On_IRed "\e[0;101m"     // Red
#define On_IGreen "\e[0;102m"   // Green
#define On_IYellow "\e[0;103m"  // Yellow
#define On_IBlue "\e[0;104m"    // Blue
#define On_IPurple "\e[0;105m"  // Purple
#define On_ICyan "\e[0;106m"    // Cyan
#define On_IWhite "\e[0;107m"   // White




#endif  // CORE_PROMPTCOLORS_H_
