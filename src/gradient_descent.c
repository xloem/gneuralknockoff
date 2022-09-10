/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Jean Michel Sellier  <jeanmichel.sellier@gmail.com>
   Copyright (C) 2016 Francesco Maria Virlinzi  <francesco.virlinzi@gmail.com>

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

// performs a gradient descent search of the best weights during the training process

#include "includes.h"
#include "gradient_descent.h"


void
gradient_descent (network * nn, network_config * config)
{
  int output = config->verbosity;       /* screen output - on/off */
  int nxw = config->nxw;        /* number of cells in one direction of the weight space */
  int maxiter = config->maxiter;        /* maximum number of iterations */
  double gamma = config->gamma; /* step size */
  double eps = config->accuracy;        /* numerical accuracy */
  register int i, j, k;
  int n;
  double delta;
  double err;
  double *wbackup;
  double *diff;

  wbackup = malloc ((nn->num_of_neurons * MAX_IN + 1) * sizeof (*wbackup));
  if (wbackup == NULL)
    {
      printf ("GD: Not enough memory to allocate\ndouble *wbackup\n");
      exit (-1);
    }

  diff = malloc ((nn->num_of_neurons * MAX_IN + 1) * sizeof (*diff));
  if (diff == NULL)
    {
      printf ("GD: Not enough memory to allocate\ndouble *diff\n");
      exit (-1);
    }


  err = 1.e8;                   // just a big number
  delta = (config->wmax - config->wmin) / nxw;

  for (n = 0; (n < maxiter) && (err > eps); n++)
    {
      // backup the weights before anything else
      for (k = 0, i = 0; i < nn->num_of_neurons; i++)
        for (j = 0; j < nn->neurons[i].num_input; ++j, ++k)
          wbackup[k] = nn->neurons[i].w[j];

      // computes the derivatives in all directions (central difference - second order)
      double err_minus;
      double err_plus;
      // computes the derivative for every single direction
      for (k = 0, i = 0; i < nn->num_of_neurons; i++)
        for (j = 0; j < nn->neurons[i].num_input; ++j, ++k)
          {
            nn->neurons[i].w[j] = wbackup[k] - delta;
            err_minus = error (nn, config);
            nn->neurons[i].w[j] = wbackup[k] + delta;
            err_plus = error (nn, config);
            diff[k] = 0.5 * (err_plus - err_minus) / delta;
          }

      // updates the weights according to the gradient
      for (k = 0, i = 0; i < nn->num_of_neurons; i++)
        for (j = 0; j < nn->neurons[i].num_input; ++j, ++k)
          nn->neurons[i].w[j] = wbackup[k] - gamma * diff[k];

      // updates the error of the NN
      err = error (nn, config);
      if (output == ON)
        printf ("GD: %d %g\n", n, err);
    }
  if (output == ON)
    printf ("\n");
  free (wbackup);
  free (diff);
}
