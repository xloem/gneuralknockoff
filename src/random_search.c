/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Jean Michel Sellier <jeanmichel.sellier@gmail.com>
   Copyright (C) 2016 Francesco Maria Virlinzi <francesco.virlinzi@gmail.com>

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

// perform a random search of the best weights of a network during the training process

#include "includes.h"
#include "random_search.h"
#include "randomize.h"


void
random_search (network * nn, network_config * config)
{
  int output = config->verbosity;       /* screen output - on/off */
  int nmax = config->nmax;      /* maximum number of random attempts */
  double eps = config->accuracy;        /* accuracy of the method */
  register int i, j, k;
  int n;
  double err, e0;
  double *wbackup;

  wbackup = malloc ((nn->num_of_neurons * MAX_IN + 1) * sizeof (*wbackup));
  if (wbackup == NULL)
    {
      printf ("RND: Not enough memory to allocate\ndouble *wbackup\n");
      exit (0);
    }


  err = 1.e8;                   // just a big number
  e0 = error (nn, config);

  for (n = 0; (n < nmax) && (e0 > eps); n++)
    {
      // backup of the weights
      for (k = 0, i = 0; i < nn->num_of_neurons; ++i)
        for (j = 0; j < nn->neurons[i].num_input; ++j, ++k)
          wbackup[k] = nn->neurons[i].w[j];

      // random weights
      randomize (nn, config);
      // update error
      err = error (nn, config);
      if (err < e0)
        {
          // keep the new state
          e0 = err;
        }
      else
        {
          // restore the old state
          for (k = 0, i = 0; i < nn->num_of_neurons; i++)
            for (j = 0; j < nn->neurons[i].num_input; j++, ++k)
              nn->neurons[i].w[j] = wbackup[k];
        }
      if (output == ON)
        printf ("RND: %d %g\n", n, e0);
    }
  free (wbackup);
}
