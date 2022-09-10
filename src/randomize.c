/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Jean Michel Sellier <jeanmichel.sellier@gmail.com> & Ray Dillinger <bear@sonic.net>

   This file is part of Gneural Knockoff.

   Gneural Knockoff is free software; you can redistribute it and/or modify it
   under the terms of the GNU Affero General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any later
   version.

   Gneural Knockoff is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
   License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with Gneural Knockoff.  If not, see <http://www.gnu.org/licenses/>.
*/

// assigns weights randomly for each neuron

#include "includes.h"
#include "randomize.h"
#include "rnd.h"

void
randomize (network * nn, network_config * config)
{
  register int i, n;

  /* for each neuron in the network */
  for (n = 0; n < nn->num_of_neurons; n++)
    /* for each input in the neuron */
    for (i = 0; i < nn->neurons[n].num_input; i++)
      {
        nn->neurons[n].w[i] =
          config->wmin + rnd () * (config->wmax - config->wmin);
      }
}

// returns a random float (using random()) between min and max.
double
randomfloat (const double min, const double max)
{
  if (max < min)
    return randomfloat (max, min);
  return (min + ((double) random () / RAND_MAX) * (max - min));
}
