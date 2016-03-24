/* simulated_annealing.h -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016 Jean Michel Sellier
   <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "includes.h"
#include "defines.h"
#include "structures.h"
#include "randomize.h"
#include "error.h"
#include "rnd.h"

extern int NNUM;
extern int ERROR_TYPE;
extern neuron NEURON[];

void simulated_annealing(int,int,int,double,double,double);

#endif
