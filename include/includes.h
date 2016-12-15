/* includes.h -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>


   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// This is supposed to prevent us from having to parse a whole bunch of library header files more than once.  unfortunately macros and typedefs from included
// headers are not included transitively, so for many definitions it doesn't work.  Could be fixed by echoing important typedefs and macro defns here, but we're
// nowhere near the knee of the curve where parsing recursive includes starts to occupy 90+% of the compile time.  Unless this gets to be a
// ten-million-line-plus program, headers including headers ad infinitum is not likely to occupy more than half our compile time and 'extra' includes for macros
// & typedefs shouldn't be a problem. The usual suspects for redundant parsing are stdint (uint32_t & friends) and stdio (FILE, size_t, etc).

#ifndef INCLUDES_H
#define INCLUDES_H

#include<assert.h>
#include<ctype.h>
#include<error.h>
#include<getopt.h>
#include<limits.h>
#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<stdint.h>
#include<math.h>
#include<memory.h>
#include<time.h>
#ifdef  HAVE_STRING_H
#include<string.h>
#else
#include<strings.h>
#endif
#include<omp.h>

#endif
