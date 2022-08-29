/* load.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "includes.h"
#include "load.h"

#include <stdarg.h>

intptr_t exit_if (intptr_t return_value, intptr_t error_value, char const * fmt, ...)
{
  if (return_value == error_value)
    {
      char message[512];
      va_list args;

      va_start (args, fmt);
      vsnprintf (message, sizeof(message), fmt, args);
      va_end (args);

      perror (message);

      exit (-1);
    }
  return return_value;
}


void
network_load (network * nn, network_config * config)
{
  int output = config->verbosity;       /* screen output - on/off */
  // load a network that has been previously saved
  int i, j;
  double tmp;
  FILE *fp;

  fp = (FILE*) exit_if (
    (intptr_t) fopen (config->load_network_file_name, "r"),
    (intptr_t) NULL,
    "cannot open file %s!", config->load_network_file_name
  );

  // saves the description of every single neuron
  exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "total number of neurons");

  /* set the neuron number and alloc the required memory */
  network_set_neuron_number (nn, (unsigned int) tmp);

  for (i = 0; i < nn->num_of_neurons; i++)
    {
      neuron *ne = &nn->neurons[i];
      exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "neuron index - useless");
      exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "number of input connections (weights");

      ne->num_input = (int) (tmp);

      for (j = 0; j < ne->num_input; j++)
        {
          exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "neuron i=%d j=%d", i, j);
          ne->w[j] = tmp;       // set the 'j-th'weight in the 'i-th' neuron
        }

      for (j = 0; j < ne->num_input; j++)
        {
          exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "connections to other neurons i=%d j=%d", i, j);
          ne->connection[j] = &nn->neurons[(int) (tmp)];
//   NEURON[i].connection[j]=(int)(tmp);
        }
      exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "activation function %d", i);
      ne->activation = (int) (tmp);

      exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "accumulator function %d", i);
      ne->accumulator = (int) (tmp);
    }

  // saves the network topology
  exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "total number of layers (including input and output layers)");
  network_set_layer_number (nn, (int) (tmp));

  for (i = 0; i < nn->num_of_layers; i++)
    {
      layer *le = &nn->layers[i];
      exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "total number of neurons in the %d-th layer", i);
      le->num_of_neurons = (unsigned long) tmp;

      for (j = 0; j < le->num_of_neurons; j++)
        {
          exit_if (fscanf (fp, "%lf\n", &tmp), EOF, "global id neuron of %d-th neuron in the %d-th layer", j, i);
          if (!j)               /* set only the first neuron in the layer... the following neuron are contigues... */ /* TODO: file contains ignored topology */
            le->neurons = &nn->neurons[(int) tmp];
//   NETWORK.neuron_id[i][j]=(int)(tmp);
        }
    }

  fclose (fp);

  // screen output
  if (output == ON)
    {
      printf ("load(NETWORK);\n");
      // neuron descriptions
      printf ("\nNEURONS\n=======\n");
      printf ("NNUM = %d\n", nn->num_of_neurons);       // total number of neurons
      for (i = 0; i < nn->num_of_neurons; i++)
        {
          neuron *ne = &nn->neurons[i];
          printf ("\n=======\n");
          printf ("NEURON[%d].nw = %d\n", i, ne->num_input);    // number of input connections (weights)
          for (j = 0; j < ne->num_input; j++)
            printf ("NEURON[%d].w[%d] = %g\n", i, j, ne->w[j]); // weights

          for (j = 0; j < ne->num_input; j++)
            printf ("NEURON[%d].connection[%d] = %d\n", i, j, ne->connection[j]->global_id);    // connections to other neurons
          printf ("NEURON[%d].activation = %d\n", i, ne->activation);   // activation function
          printf ("NEURON[%d].accumulator = %d\n", i, ne->accumulator); // accumulator function
          printf ("=======\n");
        }
      // network topology
      printf ("\nNETWORK\n=======\n");
      printf ("NETWORK.num_of_layers = %d\n", nn->num_of_layers);       // total number of layers (including input and output layers)
      for (i = 0; i < nn->num_of_layers; i++)
        {
          layer *le = &nn->layers[i];
          printf ("NETWORK.num_of_neurons[%d] = %d\n", i, le->num_of_neurons);  // total number of neurons in the i-th layer
          for (j = 0; j < le->num_of_neurons; j++)
            printf ("NETWORK.neuron_id[%d][%d] = %d\n", i, j, le->neurons[j].global_id);        // global id neuron of every neuron in the i-th layer
        }
      printf ("\n");
    }
}
