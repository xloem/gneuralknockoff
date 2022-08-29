/* rnd.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// returns a number between 0. and 1.

#include "includes.h"
#include "rnd.h"

inline double
rnd (void)
{
  static int ISEED = 38467.;

  ISEED = fmod (1027. * ISEED, 1048576.);

  return ISEED / 1048576.;
}
