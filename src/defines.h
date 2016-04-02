/* defines.h -- This belongs to gneural_network

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

// maximum number of training point
#define MAX_TRAINING_POINTS 8

// maximum allowed number of input connections per neuron
#define MAX_IN 16

// maximum total number of neurons
#define MAX_NUM_NEURONS 32

// maximum number of layers
#define MAX_NUM_LAYERS 16

// maximum number of points in output file
#define MAX_NUM_POINTS 64

// definition of various internal types
#define OFF 0
#define ON  1
// activation functions
#define TANH     0
#define EXP      1
#define ID       2
#define POL1     3
#define POL2     4
// discriminant functions
#define LINEAR   0
#define LEGENDRE 1
#define LAGUERRE 2
#define FOURIER  3
// error/cost/target functions
#define L2   0
#define LINF 1
// optimization methods
#define SIMULATED_ANNEALING 0
#define RANDOM_SEARCH       1
#define GRADIENT_DESCENT    2
#define GENETIC_ALGORITHM   3
#define MSMCO               4
