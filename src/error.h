/* error.h -- This belongs to gneural_network

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

#ifndef ERROR_H
#define ERROR_H

#include "includes.h"
#include "defines.h"
#include "structures.h"
#include "feedforward.h"

extern int NNUM;
extern int NDATA;
extern double X[MAX_TRAINING_POINTS][MAX_NUM_NEURONS][MAX_IN];
extern double Y[MAX_TRAINING_POINTS][MAX_NUM_NEURONS];
extern neuron NEURON[];
extern network NETWORK;

double error(int);

#endif
