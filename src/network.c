/* network.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>
   Copyright (C) 2016 Francesco Maria Virlinzi <francesco.virlinzi@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   Programmable Neural Network. Every neuron consists of n-inputs and only one output. The activation and accumulator
   functions are defined by the user (see below).  Every network is made of layers which forward propagate information
   from left to right. Every layer can have an arbitrary amount of neurons which are connected with each other.

   Every neuron has a global ID number and a local ID number (inside the layer). This simplifies the description of
   connections in a network.

*/

#ifdef _DEBUG
# define d_print(fmt, args...)  printf("%s: " fmt, __func__ , ## args)
#else
# define d_print(fmt, args...)
#endif

#include "includes.h"
#include "network.h"
#include "defines.h"
#include "msmco.h"
#include "randomize.h"
#include "random_search.h"
#include "simulated_annealing.h"
#include "gradient_descent.h"
#include "genetic_algorithm.h"

/*
 * network* API
 */
network *network_alloc()
{
  network * nn = (network *)malloc(sizeof(*nn));
  if (nn)
	memset(nn, 0, sizeof(*nn));
  return nn;
};

static void _free_neuron_data(neuron* ne)
{
  if (!ne)
	return;

  if (ne->connection)
	free(ne->connection);

  if (ne->w)
	free(ne->w);
}

void network_free(network *nn)
{
 if (!nn)
	return;

 if (nn->neurons) {
	int i;

	for (i = 0; i < nn->num_of_neurons; ++i)
		_free_neuron_data(&nn->neurons[i]);

	free(nn->neurons);
	}

 if (nn->layers)
	free(nn->layers);

 free(nn);
}

int network_set_neuron_number(network *nn, unsigned long nr)
{
  int i;

  if (!nn || !nr)
	return -1;

  if (nr == nn->num_of_neurons) /* do nothing... */
	return 0;

  if (nn->neurons)
        /* free the previous array if any */
	free(nn->neurons);

  nn->neurons = (neuron *)malloc(sizeof(neuron) * nr);
  if (!nn->neurons) {
	printf("No memory available to allocate the neurons array!\n");
	exit(-1);
  }

  memset(nn->neurons, 0, sizeof(neuron) * nr);
  nn->num_of_neurons = nr;

  for (i = 0; i < nr; ++i) {
	nn->neurons[i].global_id = i; /* set the global is once */
	nn->neurons[i].output = 0.0;
  }

  d_print("neurons array allocated: size for %d elements\n", nr);

  return 0;
}

int network_set_layer_number(network *nn, unsigned long nr)
{
  if (!nn || !nr)
	return -1;

  if (nr == nn->num_of_layers) /* do nothing... */
	return 0;

  if (nn->layers)
	/* free the previous array if any */
	free(nn->layers);

  nn->layers = (layer *) malloc(sizeof(layer) * nr);
  if (!nn->layers) {
	printf("No memory available to allocate the layers array!\n");
	exit(-1);
  }
  memset(nn->layers, 0, sizeof(layer) * nr);

  nn->num_of_layers = nr;
  d_print("layers array allocated: size for %d elements\n", nr);

  return 0;
}

int network_neuron_set_connection_number(neuron* ne, unsigned int nr)
{
  int i;
  if (!ne || !nr)
	return -1;

  if (ne->num_input == nr)
        /* do nothing */
        return 0;

  if (ne->connection)
        free(ne->connection);
  if (ne->w)
	free(ne->w);

  ne->connection = (neuron **)malloc(sizeof(neuron *) * nr);
  if (!ne->connection) {
	printf("No memory available to allocate the connetcion array!\n");
	exit(-1);
  }

  memset(ne->connection, 0, sizeof(neuron *) * nr);

  ne->w = (double *)malloc(sizeof(double) * nr);
  if (!ne->w) {
	printf("No memory available to allocate the y array!\n");
	exit(-1);
  }

  for (i = 0; i < nr; ++i)
    ne->w[i] = 0.0;

  ne->num_input = nr;
  return 0;
}

/*
 * __network_check:
 * - check network consistensy
 */
static int __network_check(network *nn)
{
  int i, j, k;

  if (!nn) {
    printf("Network can not be null");
    return -1;
  }
  if (!nn->num_of_neurons) {
    printf("Error: Network with No neurons!!");
    return -1;
  }
  if (!nn->num_of_layers) {
    printf("Error: Network with No layers!!");
    return -1;
  }

  /* 1. check the total neurons in the layers are equal
   *    to the neuron in the network
   */
  for (j = 0, i = 0; i < nn->num_of_layers; ++i)
	j += nn->layers[i].num_of_neurons;
  if (j != nn->num_of_neurons) {
	printf("Error: The total neuron in the layers is not equal "
	    "to the total neurons the network has (%d vs %d)\n", j, nn->num_of_neurons);
	return -1;
  }

  /* 2. check the layers do not overlap and there is no hole
   */
  for (i = 0; i < nn->num_of_layers -1; ++i)
    if ((nn->layers[i].neurons + nn->layers[i].num_of_neurons) != nn->layers[i + 1].neurons) {
	printf("Error: There is layers error setting between layer %d and %d\n",
	    i, i + 1);
	return -1;
    }

  /*
   * 3. neuron connetction can not be NULL
   */
  for (i = 1; i < nn->num_of_layers; ++i)
   for (j = 0; j < nn->layers[i].num_of_neurons; ++j)
    for (k = 0; k < nn->layers[i].neurons[j].num_input; ++k)
     if (nn->layers[i].neurons[j].connection[k] == NULL) {
       printf("Error: Neuron %d has a NULL connection %d\n", nn->layers[i].neurons[j].global_id, j);
       return -1;
     }

  return 0;
}

void network_print(network *nn)
{
  int i, j, k;

  if (!nn)
    return;

  printf("========\nNetwork:\n");
  printf("-> Number of neurons: %d\n-> Number of layers: %d\n",
      nn->num_of_neurons, nn->num_of_layers);
  for (i = 0; i < nn->num_of_layers; ++i) {
	printf("-> Layer: #%d has %d neurons\n", i, nn->layers[i].num_of_neurons);
	if (i == 0) {
	  printf("    ");
	  for (j = 0; j < nn->layers[i].num_of_neurons; ++j)
	    printf("n[%d]\t", nn->layers[i].neurons[j].global_id);
	} else {
	  for (j = 0; j < nn->layers[i].num_of_neurons; ++j) {
	    printf("    ");
	    printf("n[%d] (%d inputs)\n",
		nn->layers[i].neurons[j].global_id, nn->layers[i].neurons[j].num_input);
	    for (k = 0; k < nn->layers[i].neurons[j].num_input; ++k)
		printf("\tn[%d] (%.2f)",
		  nn->layers[i].neurons[j].connection[k]->global_id,
		  nn->layers[i].neurons[j].w[k]);
	    printf("\n");
	  }
	}
	printf("\n");
  }
  printf("========\n");
}

// Prints a network of the new format.  --added by Ray D. 29 Aug 2016
void nnet_print(struct nnet *net){
    printf("=========\nnetwork (new format)\n");
    printf("network has %d nodes\n", net->nodecount);
    printf("network has %d connections including bias connections \n", net->synapsecount);
    printf("node 0 is the bias input\n");
    printf("nodes 1 through %d are input\n", net->inputcount);
    if (net->outputcount == 1) printf("node %d is the output node.\n", net->nodecount-1);
    else printf("nodes %d through %d are output\n", net->nodecount - net->outputcount, net->nodecount-1);
    printf("connections:\n");
    printf("index || src-->dest|| weight\n");
    for (unsigned int concount = 0; concount < net->synapsecount; concount++)
	printf("%5d || %3d-->%-3d || %6.2f\n", concount, net->sources[concount], net->dests[concount], net->weights[concount]);
    printf("\n============\n");
}


void network_run_algorithm(network *nn, network_config *config)
{
  static const
  void (*supported_optimization_methods[])(network *, network_config *) = {
	[SIMULATED_ANNEALING]	= simulated_annealing,
	[RANDOM_SEARCH]		= random_search,
	[GRADIENT_DESCENT]	= gradient_descent,
	[GENETIC_ALGORITHM]	= genetic_algorithm,
	[MSMCO]			= msmco,
  };

  /*
   * Before the algorith... we check the network topology
   */
  if (__network_check(nn) < 0) {
    /* print the network topology in case of error... */
        network_print(nn);
	exit(-1);
  }

  /* network_print(nn); */

  supported_optimization_methods[config->optimization_type](nn, config);
}

/*
 * network_config*  API
 */
network_config *network_config_alloc()
{
  network_config *config = (network_config *)malloc(sizeof(*config));
  if (config)
	memset(config, 0, sizeof(*config));
  return config;
}

void network_config_free(network_config *config)
{
  if (!config)
	return;

  if (config->load_network_file_name)
	free(config->load_network_file_name);

  if (config->save_network_file_name)
	free(config->save_network_file_name);

  free(config);
}

void network_config_set_default(network_config *config)
{
  // some defaults are defined before parsing the input script
  config->save_output = OFF;
  config->load_neural_network = OFF;
  config->save_neural_network = OFF;
  config->initial_weights_randomization = ON;
  config->error_type = MSE;
}

network_config *network_config_alloc_default()
{
  network_config *config = network_config_alloc();
  if (config)
	network_config_set_default(config);
  return config;
}


struct nnet *nnet_alloc_default(){
    struct nnet *retval = (struct nnet *)calloc(1, sizeof(struct nnet));
    if (retval == NULL) {printf("Unable to allocate (in nnet_alloc_default).\n"); exit(1);}
    //calloc does the right thing here: all pointers are NULL, all counts are zero.
    return(retval);
}

// swaps a set of nodes with another equal-size set of nodes and patches up the connections.
void SwapRange(struct nnet *net, int start, int star2, int len){
    int count; int swap; unsigned int wsap;
    if (start > star2) SwapRange(net, star2, start, len);
    if (net == NULL || len < 0 || start < 0 || star2 < 0 || star2 + len >= net->nodecount){printf("improper call to SwapRange.\n"); exit(1);}
    if (start + len >= star2){printf("SwapRange cannot swap overlapping ranges.\n");exit(1);}
    if (start < net->inputcount && star2+len >= net->inputcount){printf("SwapRange cannot swap between input and non-input nodes.\n"); exit(1);}
    if (start < (net->nodecount - net->outputcount) && star2+len >= (net->nodecount - net->outputcount))
	{printf("SwapRange cannot swap between output and non-output nodes.\n"); exit(1);}
    for (count = 0; count < len; count++){
	swap = net->transfer[start+count]; net->transfer[start+count] = net->transfer[star2+count]; net->transfer[star2+count] = swap;
	swap = net->accum[start+count]; net->accum[start+count] = net->accum[star2+count]; net->accum[star2+count] = swap;
	wsap=net->transferwidths[start+count];net->transferwidths[start+count]=net->transferwidths[star2+count];net->transferwidths[star2+count]=wsap;
    }
    for (count = 0; count < net->synapsecount; count++){
	if (net->sources[count] > start && net->sources[count] < start+len) net->sources[count] += (star2-start);
	else if (net->sources[count] > star2 && net->sources[count] < star2+len)net->sources[count] += (start-star2);
	if (net->dests[count] > start && net->dests[count] < start+len) net->dests[count] += (star2-start);
	else if (net->dests[count] > star2 && net->dests[count] < star2+len)net->dests[count] += (start-star2);
    }
}

// add (newcount) new nodes starting at (startloc). update synapses to maintain pre-existing connectivity. new nodes
// should have the specified accumulator and transfer functions and transfer size - but being new will have no incoming
// or outgoing connections.  This is NOT public - it could insert new nodes only part of which are in the input or
// output ranges accidentally breaking input/output to connectivity mappings even though it preserves connectivity.
int InsertNodes(struct nnet *net, int startloc, int newcount, int transferfn, int accumfn, unsigned int xfersize){
    if (net == NULL || newcount < 0) {fprintf(stderr, "Program Error: improper call to InsertNodes.\n");exit(1);}
    if (net->nodecount == 0) {net->nodecount++; net->inputcount++; startloc++;}
    int *newtrans = (int *)malloc((net->nodecount + newcount)*sizeof(int));
    int *newacc = (int *)malloc((net->nodecount + newcount)*sizeof(int));
    unsigned int *xwidth = (unsigned int *)malloc((net->nodecount + newcount)*sizeof(int));
    int count;
    if (newtrans == NULL || newacc == NULL || xwidth == NULL) {
	free(newtrans); free(newacc); free (xwidth); fprintf(stderr,"Runtime error: allocation failure while inserting nodes.\n");exit(1);}
    newtrans[0] = newacc[0] = 0; xwidth[0] = 1; // reserve bias node
    if (net->transfer != NULL && net->accum != NULL && net->transferwidths != NULL)
	for (count = 1; count < startloc; count++){
	    newtrans[count] = net->transfer[count]; newacc[count] = net->accum[count]; xwidth[count] = net->transferwidths[count];}
    for (count = startloc; count < startloc + newcount; count++){ newtrans[count] = transferfn; newacc[count] = accumfn; xwidth[count] = xfersize;}
    if (net->transfer != NULL && net->accum != NULL && net->transferwidths != NULL)
	for (count = startloc; count < net->nodecount; count++){
	    newtrans[count+newcount]=net->transfer[count];newacc[count+newcount]=net->accum[count];xwidth[count+newcount]=net->transferwidths[count];}
    free(net->transfer); free (net->accum); free(net->transferwidths);
    net->transfer = newtrans; net->accum = newacc; net->transferwidths = xwidth; net->nodecount += newcount;
    for (count = 0; count < net->synapsecount; count++){
	net->sources[count] += net->sources[count] < startloc ? 0 : newcount;
	net->dests[count] += net->dests[count] < startloc ? 0 : newcount;
    }
    return(startloc);
}

// inserts non-input, non-output nodes.  New nodes will be at end of non-output nodes (but you can swapRange them within other nodes if you want).
int AddHiddenNodes(struct nnet *net, int newnodes, int transferfn, int accumfn, unsigned int xfersize){
    return (InsertNodes(net, net->nodecount - net->outputcount, newnodes, transferfn, accumfn, xfersize));
}


// inserts input nodes to existing network.  New nodes will be at end of current input (but you can SwapRange them within input if you want).
int AddInputNodes(struct nnet *net, int newnodes, int transferfn, int accumfn, unsigned int xfersize){
    net->inputcount += newnodes;
    return (InsertNodes(net, net->inputcount-newnodes, newnodes, transferfn, accumfn, xfersize));
}


// inserts output nodes into existing network.  New nodes will be at end of current output (but you can SwapRange them within output if you want).
int AddOutputNodes(struct nnet *net, int newnodes, int transferfn, int accumfn, unsigned int xfersize){
    net->outputcount += newnodes;
    return (InsertNodes(net, net->nodecount, newnodes, transferfn, accumfn, xfersize));
}

// inserts connections into an existing network.
void AddConnections(struct nnet *net, int fromstart, int fromend, int tostart, int toend, flotype *weight){
    int wt = 0;
    int newcount = net->synapsecount + (1+fromend-fromstart) * (1+toend-tostart);
    net->weights = (flotype *)realloc(net->weights, newcount * sizeof(flotype));
    net->sources = (unsigned int *)realloc(net->sources, newcount * sizeof(int));
    net->dests = (unsigned int *)realloc(net->dests, newcount * sizeof(int));
    if (net->weights == NULL || net->sources == NULL || net->dests == NULL) {printf("Runtime error: allocation failure in AddConnections.\n"); exit(1);}
    for (int from = fromstart; from <= fromend; from++)
	for (int to = tostart; to <= toend; to++){
	    net->sources[net->synapsecount] = from;
	    net->dests[net->synapsecount] = to;
	    net->weights[net->synapsecount++] = weight[wt++];
	}
}

// insert randomized connections into an existing network.
void AddRandomizedConnections(struct nnet *net, int fromstart, int fromend, int tostart, int toend){
    int wt = 0;
    int newcount = net->synapsecount + (1+fromend-fromstart) * (1+toend-tostart);
    net->weights = (flotype *)realloc(net->weights, newcount * sizeof(flotype));
    net->sources = (unsigned int *)realloc(net->sources, newcount * sizeof(int));
    net->dests = (unsigned int *)realloc(net->dests, newcount * sizeof(int));
    if (net->weights == NULL || net->sources == NULL || net->dests == NULL) {printf("Runtime error: Allocation failure in AddRandomizedConnections.\n"); exit(1);}
    for (int from = fromstart; from <= fromend; from++)
	for (int to = tostart; to <= toend; to++){
	    net->sources[net->synapsecount] = from;
	    net->dests[net->synapsecount] = to;
	    // FIXME: at some future stage of development auto-optimize the random ranges per Bengio & Glorot's paper, and Hinton's addendum to that paper.
	    // And/or punt to the user and ask/allow THEM to specify a range for initialization.
	    net->weights[net->synapsecount++] = randomfloat(-0.5, +0.5);
	}
}


//  produce a new-format network given an old-format network.  -- added by Ray D. 29 Aug 2016. The 'nnet' format has a
//  single population of nodes (neurons) and a single sequence of connections (synapses).  The accumulation and transfer
//  functions are called the first time in the sequence that the node is used as the source for any synapse. They are
//  user definable on a per-node basis, as they are in the old network format.  Each node and connection has a global ID
//  - which is the array index at which which its information is recorded. In the case of synapses the global ID is also
//  the synapse's number in the sequence of synapse operations.

// The advantages of the nnet format are: First, it permits bias weights.  Second, it does not depend on a layered
//  structure. It allows backward connections so it can be used for recurrent networks and it allows self connections,
//  so it can be used for Boltzmann networks or LSTM's. In general it permits recurrent and topologically varied
//  networks. Third, neither the number of nodes nor the number of connections is limited.  Fourth, it allows transfer
//  functions of arbitrary width (producing and consuming more than one node, so multi-argument and/or multi-output
//  functions can be used).  Other than bias weights (initialized to zero) converted networks do not have these
//  features.

struct nnet *convertnetwork(struct _network *oldnet){
    struct nnet *newval;    unsigned int layercount, neuroncount, connectioncount, nodenum;
    unsigned int synapsecount = 0;
    if (oldnet == NULL || NULL == (newval = (struct nnet *)calloc(1, sizeof(struct nnet)))) return (NULL); // unable to allocate
    newval->nodecount = oldnet->num_of_neurons + 1; // add one to get space for reserved zero node.
    newval->inputcount = oldnet->layers[0].num_of_neurons;  // old structure assumed that all of layer zero is input.
    newval->outputcount = oldnet->layers[oldnet->num_of_layers - 1].num_of_neurons; // old structure assumed that all of last layer is output.
    newval->synapsecount = oldnet->num_of_neurons - oldnet->layers[0].num_of_neurons;  // old structure didn't have bias weights; this makes space for one per non-input neuron.
    for (layercount = 1; layercount < oldnet->num_of_layers; layercount++) // for loop counts non-bias connections in old network
	for (neuroncount = 0; neuroncount < oldnet->layers[layercount].num_of_neurons; neuroncount++)
	    newval->synapsecount += oldnet->layers[layercount].neurons[neuroncount].num_input;
    newval->weights = (flotype *)malloc(newval->synapsecount * sizeof(flotype));
    newval->sources = (unsigned int *)malloc(newval->synapsecount * sizeof(unsigned int));
    newval->dests = (unsigned int *)malloc(newval->synapsecount  * sizeof(unsigned int));
    newval->transfer = (enum activation_function *)malloc(sizeof(enum activation_function) * newval->nodecount);
    newval->accum = (enum accumulator_function *)malloc(sizeof(enum accumulator_function) * newval->nodecount);
    if (newval->weights == NULL || newval->sources == NULL || newval->dests == NULL || newval->transfer == NULL || newval->accum == NULL){  // if unable to allocate
	free(newval->weights); free(newval->sources); free(newval->weights); free(newval->transfer); free(newval->accum); free(newval); return (NULL);
    }
    for (layercount = 1; layercount < oldnet->num_of_layers; layercount++){
	for (neuroncount = 0; neuroncount < oldnet->layers[layercount].num_of_neurons; neuroncount++){
	    nodenum = oldnet->layers[layercount].neurons[neuroncount].global_id + 1;  // adding one to make room for zero node
	    newval->transfer[nodenum] = oldnet->layers[layercount].neurons[neuroncount].activation;
	    newval->accum[nodenum] = oldnet->layers[layercount].neurons[neuroncount].accumulator;
	    newval->dests[synapsecount] = nodenum; 	    // add dest/source/weight for bias connection
	    newval->sources[synapsecount] = 0;              // (old network format lacks bias connections)
	    newval->weights[synapsecount++] = ZERO;         // initial bias weight is zero
	    for (connectioncount = 0; connectioncount < oldnet->layers[layercount].neurons[neuroncount].num_input; connectioncount++){
		newval->dests[synapsecount] = nodenum;
		newval->sources[synapsecount] = oldnet->layers[layercount].neurons[neuroncount].connection[connectioncount]->global_id + 1;
		newval->weights[synapsecount++] = (flotype)(oldnet->layers[layercount].neurons[neuroncount].w[connectioncount]);
	    }
	}
    }
    // Traversal of old network gave us connections in sequence by destination; new format needs them in sequence by origin. so we sort.
    flotype wtmp; unsigned int srctmp; unsigned int desttmp; unsigned int skipsize; unsigned int i1; unsigned int i2;
    for (skipsize = (2 * --synapsecount) / 3; skipsize >= 1; skipsize = (2 * skipsize) / 3)
	for (connectioncount = 0; connectioncount < synapsecount; connectioncount++){
	    i1 = MIN (connectioncount, (connectioncount + skipsize) % synapsecount);
	    i2 = MAX (connectioncount, (connectioncount + skipsize) % synapsecount);
	    if (newval->sources[i1] > newval->sources[i2] || (newval->sources[i1] == newval->sources[i2] && newval->dests[i1] > newval->dests[i2])){
		wtmp = newval->weights[i1];    newval->weights[i1] = newval->weights[i2];   newval->weights[i2] = wtmp;
		srctmp = newval->sources[i1];  newval->sources[i1] = newval->sources[i2];   newval->sources[i2] = srctmp;
		desttmp = newval->dests[i1];   newval->dests[i1]   = newval->dests[i2];     newval->dests[i2] = desttmp;
	    }
	}
    return(newval);
}
