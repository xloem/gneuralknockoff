/* rnd.h -- This belongs to gneural_network

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

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "defines.h"

typedef struct _neuron{
 int nw;
 // connection[i] is the identification number of the neuron which
 // output is connected to the i-th input branch of the neuron
 int connection[MAX_IN];
 int activation; // type of activation function
 int discriminant; // type of discriminant function
 double x[MAX_IN]; // n inputs
 double w[MAX_IN]; // n weights
 double output;    // one output
} neuron;

typedef struct _network{
 int num_of_layers; // total number of layers
 int num_of_neurons[MAX_NUM_LAYERS]; // number of neurons per layer
 // neuron_id[i][j] is the global identification number of the j-th neuron in the i-th layer
 int neuron_id[MAX_NUM_LAYERS][MAX_NUM_NEURONS];
} network;

#endif
