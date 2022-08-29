/* network. -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>             // for uint32_t macro etc.
#include <stdio.h>              // for size_t macro, FILE macro, etc.
#include "defines.h"            // for frickin everything that isn't a macro.

typedef struct _neuron
{
  unsigned int global_id;       // a unique global id for each neuron
  unsigned int num_input;       // how many inputs the neuron has
  // connection[i] is the identification number of the neuron which
  // output is connected to the i-th input branch of the neuron
  struct _neuron **connection;  // which neurons is connected
  enum activation_function activation;  // type of activation function
  enum accumulator_function accumulator;        // type of accumulator function
  // double x[MAX_IN]; // n inputs FMV: no more required...
  double *w;                    // n weights
  double output;                // one output
} neuron;

typedef struct _layer
{
  unsigned int num_of_neurons;  // number of neurons in the layers;
  neuron *neurons;              // pointer to the first neuron of the layers;
} layer;

typedef struct _network
{
  unsigned int num_of_neurons;  // total number of neurons
  unsigned int num_of_layers;   // total number of layers
  layer *layers;
  neuron *neurons;
} network;

// struct added by Ray Dillinger, Aug 2016
struct nnet
{
  unsigned int inputcount;      // nodes 1 to this number (inclusive) are inputs (node zero is reserved for bias).
  unsigned int outputcount;     // nodes (nodecount minus this number) to nodecount (exclusive) are outputs.
  unsigned int nodecount;       // this many total nodes.
  int *transfer;                // each node has its own transfer function.
  int *accum;                   // each node has its own accumulation function. (addition assumed if NULL)
  unsigned int *transferwidths; // One record per node of how many nodes are to be processed by the transfer fn when
  // processing this node.  transfer width is assumed to be one if this is left NULL.
  // In wide transfers only the entry corresponding to the first node will be read.
  unsigned int synapsecount;    // this many connections (including bias) are used during one 'cycle' of the network.
  flotype *weights;             // each synapse has its own weight.
  unsigned int *sources;        // each synapse has its own source (bias nodes are source zero).
  unsigned int *dests;          // each synapse has its own destination.
  struct cases *data;
  struct plans *plan;
};

// struct added by Ray Dillinger, Nov 2016
struct conf
{
  char *openingcomment;         // opening comment is saved and used in writeback.
  unsigned int flags;
  unsigned int serialnum;       // serial number of next save to output if serializing.
  unsigned int savecount;       // number of saves remaining to be made in the current plan
  char *savename;               // filename for script writeback
};

// struct added by Ray Dillinger, Jan 2017
struct plans
{
  unsigned int planflags;       // train/test/validate/deploy, endOnAccuracy/endOnDivergence/
  flotype goal;                 // goal accuracy if defined
  unsigned int epochmin;        // zero - end instantly if goal reached.
  unsigned int epochmax;        // zero - no maximum - continue until goal reached.
  unsigned int batchsize;
  unsigned int epochsize;
  flotype trainrate;
  flotype momentum;

  char *outputdest;             // filename to send individual results to case by case; may be NULL if output is not desired.
  char *reportdest;             // filename to send summary report (accuracy, use statistics, time, etc to).
  char *inputsrc;               // filename to read input from (this in addition to whatever's in data statements);
  struct plans *next;
};

// struct added by Ray Dillinger, Dec 2016
struct cases
{
  uint32_t flags;               // 1 test 2 train 4 validate 8 deploy 10 sequential 20 seekable 40 randomize 80 no_output .....
  size_t entrycount;            // number of cases in allocated data buffer.
  size_t inputcount;            // width (number of nodes) of input data for this data source. usually nnet.inputcount.
  size_t outputcount;           // width (number of nodes) of output for this data source. usually nnet.outputcount.
  char *inname;                 // filename for example data.  NULL for examples in script data.
  FILE *inpipe;                 // input pipe. NULL if pipe is not presently open.
  char *outname;                // output filename.  NULL if output is not to be written to a file.
  FILE *outpipe;                // NULL if pipe is not presently open.
  flotype *data;                // The buffer which contains the actual data.
  struct cases *next;           // yup, it's a linked list.  nnetwork scripts can ask for more than one 
};


typedef struct _network_config
{
  unsigned char verbosity;
  unsigned char initial_weights_randomization;
  enum optimization_method optimization_type;
  enum error_function error_type;

  unsigned char load_neural_network;
  char *load_network_file_name;

  unsigned char save_neural_network;
  char *save_network_file_name;

  unsigned char save_output;
  char *output_file_name;

  unsigned int num_of_cases;
  double output_x[MAX_NUM_CASES][MAX_NUM_NEURONS][MAX_IN];

  double rate;
  int nmax, mmax;
  int npop;
  int nxw;
  int maxiter;
  double accuracy;
  double gamma;
  double kbtmin, kbtmax;
  double wmin, wmax;

  /* training fields */
  unsigned int num_cases;
  double cases_x[MAX_TRAINING_CASES][MAX_NUM_NEURONS][MAX_IN];
  double cases_y[MAX_TRAINING_CASES][MAX_NUM_NEURONS];
} network_config;

struct nnet *convertnetwork (struct _network *);
int AddHiddenNodes (struct nnet *, int, int, int, unsigned int);
int AddInputNodes (struct nnet *, int, int, int, unsigned int);
int AddOutputNodes (struct nnet *, int, int, int, unsigned int);
void AddConnections (struct nnet *, int, int, int, int, flotype *);
void AddRandomizedConnections (struct nnet *, int, int, int, int);


/*
 * network* API
 */

/*
 * network_alloc:
 * - alloc a network object
 */
network *network_alloc ();
/*
 * network_free:
 * - free a network object
 */
void network_free (network *);

/*
 * network_run_algorithm:
 * - run a training algorith on a network
 */
void network_run_algorithm (network *, network_config *);

/*
 * network_set_neuron_number:
 * - set the number of neuron the network has and alloc the required memory
 */
int network_set_neuron_number (network *, unsigned long);
/*
 * network_set_layer_number:
 * - set the number of layers the network has and alloc the required memory
 */
int network_set_layer_number (network *, unsigned long);
/*
 * network_set_neuron_connection_number
 * -  set the the number of input a neuron has
 *    and alloc the required memory
 */
int network_neuron_set_connection_number (neuron * ne, unsigned int nr);

/*
 * network_set_neuron_connection_number
 * -  show the network topology
 */
void network_print (network *);

/*
 * network_config* API
 */

/*
 * network_config_alloc:
 * - alloc a network_config object
 */
network_config *network_config_alloc ();

void network_config_set_default (network_config *);

network_config *network_config_alloc_default ();
/*
 * network_config_free:
 * - free a network_config object
 */
void network_config_free (network_config *);
#endif
