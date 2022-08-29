/* activation.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Ray Dillinger  <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// returns the specified activation function given the input x

#include "includes.h"
#include "activation.h"

inline double
activation (enum activation_function type, double x)
{
  switch (type)
    {
    case ID:
      return x;
      break;
    case TANH:                 // scaled from -1 to 1
      return tanh (x);
      break;
    case ATAN:                 // scaled from -1 to 1
      return atan (x) * M_2_PI;
      break;
    case EXP:                  // scaled from 0 to 1
      return 1. / (1. + exp (-x));
      break;
    case EXP_SIGNED:           // scaled from -1 to 1
      return 2. / (1. + exp (-x)) - 1.;
      break;
    case LOGARITHMIC:
      return -log (1 + exp (-x));
      break;
    case SOFTSIGN:             // scaled from -1 to 1
      return x / abs (x) + 1;
      break;
    case STEP:                 // scaled from 0 to 1
      return x >= 0 ? 1 : 0;
      break;
    case RAMP:
      return (x > 0.) ? x : 0.;
      break;
    case SOFTRAMP:
      return log (1. + exp (x));
      break;
    case LOGRAMP:
      return (x > 0.) ? log (x) : 0.;
      break;
    case PERIODIC:
      return sin (x);
      break;
    case GAUSSIAN:
      return exp (-x * x);
      break;
    case POL1:
      return 1. + x;
      break;
    case POL2:
      return 1. + x + x * x;
      break;
    default:
      printf ("unknown activation function!\n");
      exit (-1);
    }
}
