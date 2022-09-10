/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

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

// compute the specified error of a (training) neural network

#include "includes.h"
#include "feedforward.h"

double
error (network * nn, network_config * config)
{
  register int n;
  double err;
  double y;

  err = 0.;

  switch (config->error_type)
    {
      // Mean Error
    case ME:
      err = -1.e8;
      for (n = 0; n < config->num_cases; n++)
        {
          int i, j;
          // assign training input
          for (i = 0; i < nn->layers[0].num_of_neurons; i++)
            {
              neuron *ne = &nn->layers[0].neurons[i];
#if 1
              ne->output = config->cases_x[n][ne->global_id][0];
#else
              for (j = 0; j < ne->num_input; j++)
                ne->output = config->cases_x[n][i][j];
#endif
            }
          feedforward (nn);
          // compute the mean error comparing with training output
          double tmp = 0.;
          for (j = 0; j < nn->layers[nn->num_of_layers - 1].num_of_neurons;
               j++)
            {
              neuron *ne = &nn->layers[nn->num_of_layers - 1].neurons[j];
              y = ne->output;
              tmp += fabs (y - config->cases_y[n][ne->global_id]);
            }
          err += tmp;
        }
      return err;

      break;
      // Mean Squared Error
    case MSE:
      for (n = 0; n < config->num_cases; n++)
        {
          int i, j;
          // assign training input
          for (i = 0; i < nn->layers[0].num_of_neurons; i++)
            {
              neuron *ne = &nn->layers[0].neurons[i];
#if 1
              ne->output = config->cases_x[n][ne->global_id][0];
#else
              for (j = 0; j < ne->num_input; j++)
                ne->output = config->cases_x[n][i][j];
#endif
            }
          feedforward (nn);
          // compute the squared error comparing with the training output
          double tmp = 0.;
          for (j = 0; j < nn->layers[nn->num_of_layers - 1].num_of_neurons;
               j++)
            {
              neuron *ne = &nn->layers[nn->num_of_layers - 1].neurons[j];
              y = ne->output;;
              tmp += pow (y - config->cases_y[n][ne->global_id], 2);
            }
          err += tmp;
        }
      return sqrt (err);
      break;
    default:
      return 0.;
      break;
    }
}
