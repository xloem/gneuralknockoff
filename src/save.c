/* save.c -- This belongs to gneural_network

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

#include "includes.h"
#include "save.h"
#include "feedforward.h"
#include "parser.h" // for acctokens and outtokens

void network_save(network *nn, network_config *config) {
 int output = config->verbosity; /* screen output - on/off */
 // saves all information related to the network
 int i,j;
 FILE *fp;

 fp = fopen(config->save_network_file_name,"w");
 if (fp == NULL) {
  printf("cannot save file %s!\n", config->save_network_file_name);
  exit(-1);
 }

 // saves the description of every single neuron
 fprintf(fp,"%d\n", nn->num_of_neurons); // total number of neurons
 for (i = 0; i < nn->num_of_neurons; i++){
  fprintf(fp,"%d\n",i); // neuron index
  fprintf(fp,"%d\n", nn->neurons[i].num_input); // number of input connections (weights)
  for (j = 0; j < nn->neurons[i].num_input; j++)
    fprintf(fp,"%g\n",nn->neurons[i].w[j]); // weights
  for (j = 0; j < nn->neurons[i].num_input; j++)
    fprintf(fp,"%d\n", nn->neurons[i].connection[j]->global_id); // connections to other neurons
  fprintf(fp,"%d\n", nn->neurons[i].activation); // activation function
  fprintf(fp,"%d\n", nn->neurons[i].accumulator); // accumulator function
 }

 // saves the network topology
 fprintf(fp,"%d\n", nn->num_of_layers); // total number of layers (including input and output layers)
 for (i = 0; i < nn->num_of_layers; i++){
  fprintf(fp,"%d\n", nn->layers[i].num_of_neurons); // total number of neurons in the i-th layer
  for (j = 0; j < nn->layers[i].num_of_neurons;j++)
   fprintf(fp,"%d\n", nn->layers[i].neurons[j].global_id); // global id neuron of every neuron in the i-th layer
 }

 fclose(fp);

 // screen output
 if(output==ON){
  printf("save(NETWORK);\n");
  // neuron descriptions
  printf("\nNEURONS\n=======\n");
  printf("NNUM = %d\n", nn->num_of_neurons); // total number of neurons
  for (i = 0; i < nn->num_of_neurons; i++){
   printf("\n=======\n");
   printf("NEURON[%d].nw = %d\n",i, nn->neurons[i].num_input); // number of input connections (weights)
   for (j = 0; j < nn->neurons[i].num_input; j++)
     printf("NEURON[%d].w[%d] = %g\n",i, j, nn->neurons[i].w[j]); // weights
   for (j = 0; j < nn->neurons[i].num_input; j++)
     printf("NEURON[%d].connection[%d] = %d\n",
	 i, j, nn->neurons[i].connection[j]->global_id); // connections to other neurons
   printf("NEURON[%d].activation = %d\n",
       i, nn->neurons[i].activation); // activation function
   printf("NEURON[%d].accumulator = %d\n",
       i, nn->neurons[i].accumulator); // accumulator function
   printf("=======\n");
  }
  // network topology
  printf("\nNETWORK\n=======\n");
  printf("NETWORK.num_of_layers = %d\n", nn->num_of_layers); // total number of layers (including input and output layers)
  for (i = 0; i < nn->num_of_layers; i++){
   printf("NETWORK.num_of_neurons[%d] = %d\n",
       i, nn->layers[i].num_of_neurons); // total number of neurons in the i-th layer
   for (j = 0; j < nn->layers[i].num_of_neurons; j++)
    printf("NETWORK.neuron_id[%d][%d] = %d\n",
	i, j, nn->layers[i].neurons[j].global_id); // global id neuron of every neuron in the i-th layer
  }
  printf("\n");
 }
}

void network_save_final_curve(network *nn, network_config *config)
{
  int n;
  FILE* fp = fopen(config->output_file_name, "w");

  if (fp == NULL) {
	printf("cannot save file %s!\n", config->output_file_name);
	exit(-1);
  }

  for (n = 0; n < config->num_of_cases; n++) {
	int i, j;
	double y;
	/* for each neuron in layers[0] i.e: the input layer */
	for (i = 0; i < nn->layers[0].num_of_neurons; ++i) {
#if 0
	  nn->layers[0].neurons[i].output = config->output_x[n][i][0];
	  fprintf(fp,"%g ", nn->layers[0].neurons[i].output);
#else
	  for (j = 0; j < nn->layers[0].neurons[i].num_input; ++j) {
			nn->layers[0].neurons[i].output = config->output_x[n][i][j];
			fprintf(fp,"%g ", nn->layers[0].neurons[i].output);
		}
#endif
	}

	feedforward(nn);
	/* for each neuron in the last layer */
	for (i = 0; i < nn->layers[nn->num_of_layers -1].num_of_neurons; ++i) {
		y = nn->layers[nn->num_of_layers -1].neurons[i].output;
		fprintf(fp, "%g ", y);
	}
	fprintf(fp,"\n");
  }
  fclose(fp);
}



static const char* acctokens[ACCUMCOUNT] = {ACCTOKENS};
static const char* outtokens[OUTPUTCOUNT] = {OUTTOKENS};

void WriteImmediateCases(FILE *out, const struct cases *current){
    assert(current != NULL); assert(out != NULL);
    int entry; int datum; int casesize = current->inputcount + current->outputcount; int bracecontrol = MIN(current->inputcount, current->outputcount);
    assert(casesize > 0);
    for (entry = 0; entry < current->entrycount; entry++){
        fprintf(out, "\n        ["); if (bracecontrol) fprintf(out, "[");
        for (datum = 0; datum < current->inputcount; datum++) fprintf( out, FLOFMT" ", current->data[entry*casesize + datum]);
        if (bracecontrol) fprintf(out,"][");
        for (datum = current->inputcount; datum < casesize; datum++) fprintf( out, FLOFMT" ", current->data[entry*casesize + datum]);
        fprintf(out, "]"); if (bracecontrol) fprintf(out, "]");
    }
}



// Nnetwriter produces a script that, when read by the parser, produces a network with the same configuration, topology, weights, and firing sequence of the
// network given as an argument.  All three of these things can be changed by various kinds of training, and there are different ways of expressing even the
// same topology, so this may be different from the way the configfile originally wrote it.
void nnetwriter(struct nnet *net, struct conf *config, FILE *out){
    assert(net != NULL); assert (out != NULL);
    int start, end, width, acc, xfer;
    unsigned int maxbit;
    struct cases *currentcase;
    uint32_t currentmask;
    if (config->openingcomment != NULL) fprintf(out, "%s", config->openingcomment);
    if (net->nodecount < 2) return;
    currentmask = (SILENCE_BIAS | SILENCE_DEBUG | SILENCE_ECHO | SILENCE_INPUT | SILENCE_OUTPUT | SILENCE_NODEINPUT |
                   SILENCE_NODEOUTPUT | SILENCE_MULTIACTIVATION | SILENCE_RECURRENCE | SILENCE_RENUMBER);
    if ((config->flags & currentmask) != 0 || (config->flags & SAVE_DEFAULT) != 0){
        fprintf(out, "StartConfig\n");
        if ((config->flags & currentmask) != 0){
            fprintf(out, "   Silence( "); // Bias, Debug, Echo, Input, Output, NodeInput, NodeOutput, MultiActivation, Renumber
            for (maxbit = 0x1; (config->flags & maxbit) == 0 && maxbit != 0; maxbit <<= 1);
            if ((config->flags & SILENCE_BIAS) != 0)             fprintf(out, "Bias ");
            if ((config->flags & SILENCE_DEBUG) != 0)            fprintf(out, "Debug ");
            if ((config->flags & SILENCE_ECHO) != 0)             fprintf(out, "Echo ");
            if ((config->flags & SILENCE_INPUT) != 0)            fprintf(out, "Input ");
            if ((config->flags & SILENCE_OUTPUT) != 0)           fprintf(out, "Output ");
            if ((config->flags & SILENCE_NODEINPUT) != 0)        fprintf(out, "NodeInput ");
            if ((config->flags & SILENCE_NODEOUTPUT) != 0)       fprintf(out, "NodeOutput ");
            if ((config->flags & SILENCE_MULTIACTIVATION) != 0)  fprintf(out, "MultiActivation ");
            if ((config->flags & SILENCE_RENUMBER) != 0)         fprintf(out, "Renumber ");
            fprintf(out, ")\n");
        }
        if ((config->flags & SAVE_DEFAULT) == 0){
            fprintf(out,"    Save(\"%s\"", config->savename);
            if ((config->flags & SAVE_SERIALIZE) !=0) fprintf(out, " Serialize");
            if (config->savecount != 0)fprintf(out, " %d", config->savecount);
            fprintf(out, ")\n");
        }
        fprintf(out, "\"%s\")\nEndConfig\n", config->savename);
    }
    if (net->data != NULL){
        fprintf(out, "StartData\n");
        for (currentcase = net->data; currentcase != NULL; currentcase = currentcase->next){
            fprintf(out, "   Data(");
            currentmask = (DATA_IMMEDIATE | DATA_FROMFILE | DATA_FROMDIRECTORY | DATA_FROMPIPE);
            switch(currentcase->flags & currentmask){
            case DATA_IMMEDIATE :     fprintf(out, "Immediate ");      break;
            case DATA_FROMFILE  :     fprintf(out, "FromFile ");       break;
            case DATA_FROMDIRECTORY:  fprintf(out, "FromDirectory ");  break;
            case DATA_FROMPIPE:       fprintf(out, "FromPipe ");       break;
            default: fprintf(stderr, "Program Error: Network being saved has unrecognizable data source.\n"); exit(1);
            }
            if (currentcase->flags & currentmask != DATA_IMMEDIATE) fprintf(out, "\"%s\" ", currentcase->inname);
            if ((currentcase->flags & DATA_TRAINING) != 0)     fprintf(out, "Training ");
            if ((currentcase->flags & DATA_TESTING) != 0)      fprintf(out, "Testing ");
            if ((currentcase->flags & DATA_VALIDATION) != 0)   fprintf(out, "Validation ");
            if ((currentcase->flags & DATA_DEPLOYMENT) != 0)   fprintf(out, "Deployment ");
            if ((currentcase->flags & DATA_NOINPUT) != 0)      fprintf(out, "NoInput ");
            if ((currentcase->flags & DATA_NOOUTPUT) !=0)      fprintf(out, "NoOutput ");
            if ((currentcase->flags & DATA_NOWRITEINPUT) != 0) fprintf(out, "WriteNoInput ");
            if ((currentcase->flags & DATA_NOWRITEOUTPUT) != 0)fprintf(out, "WriteNoOutput ");
            if ((currentcase->flags & DATA_WRITEFILE) != 0)    fprintf(out, "ToFile ");
            if ((currentcase->flags & DATA_WRITEPIPE) != 0)    fprintf(out, "ToPipe ");
            if ((currentcase->flags & (DATA_WRITEFILE | DATA_WRITEPIPE)) != 0) fprintf(out, "\"%s  \"", currentcase->outname);
            if ((currentcase->flags & DATA_IMMEDIATE) != 0)     WriteImmediateCases(out, currentcase);
            fprintf(out, ")\n");
        }
        fprintf(out, "EndData\n");
    }
    end = start = 1;
    fprintf(out, "StartNodes\n");
    for (start = end=1; end < net->inputcount && end+1 < net->nodecount; start=++end){
	acc = net->accum[start]; xfer = net->transfer[start]; width = net->transferwidths[start];
	while(end<net->inputcount && 1+end<net->nodecount && net->accum[end+1]==acc && net->transfer[end+1]==xfer && net->transferwidths[end+1]==width) end++;
	if (width == 1) fprintf(out,"    CreateInput( %d, %s, %s)\n",1+end-start, acctokens[acc], outtokens[xfer] );
	else fprintf(out,"    CreateInput( %d, %s, %s, %d)\n",1+end-start, acctokens[acc], outtokens[xfer], width);
    }
    for (; end < net->nodecount - net->outputcount && end+1 < net->nodecount; start=++end){
	acc = net->accum[start]; xfer = net->transfer[start]; width = net->transferwidths[start];
	while(end+1 < net->nodecount - net->outputcount && end + 1 < net->nodecount && net->accum[end+1]==acc && net->transfer[end+1]==xfer &&
	      net->transferwidths[end+1]==width) end++;
	if (width == 1) fprintf(out,"    CreateHidden( %d, %s, %s)\n",1+end-start, acctokens[acc], outtokens[xfer] );
	else fprintf(out,"    CreateHidden( %d, %s, %s, %d)\n",1+end-start, acctokens[acc], outtokens[xfer], width);
    }
    for (start=end; end < net->nodecount; start=++end){
	acc = net->accum[start]; xfer = net->transfer[start]; width = net->transferwidths[start];
	while(end+1 < net->nodecount && net->accum[end+1]==acc && net->transfer[end+1]==xfer && net->transferwidths[end+1]==width) end++;
	if (width == 1) fprintf(out,"    CreateOutput( %d, %s, %s)\n", 1+end-start, acctokens[acc], outtokens[xfer]);
	else fprintf(out,"    CreateOutput( %d, %s, %s, %d)\n",1+end-start, acctokens[acc], outtokens[xfer], width);
    }
    fprintf(out, "EndNodes\n");
    if (net->synapsecount == 0) return;
    fprintf(out, "StartConnections\n");
    // The logic in this while/switch construction is excessively intricate. Be careful and test a lot if you need to screw with it. - RD
    int state, backtrack, conn, firstfrom, firstto, lastfrom, lastto, nex;
    start = end = state = conn = firstfrom = firstto = lastfrom = lastto = nex = 0;
    while (state != 4)
	switch(state){
	case 0: // ready to start on new expression
	    end = start = conn; lastfrom=firstfrom=net->sources[conn]; lastto=firstto=net->dests[conn]; backtrack = nex = conn+1;
	    state = conn >= net->synapsecount ? 4 : 1; break;
	case 1: if (net->sources[nex] == net->sources[conn] && net->dests[nex] == 1+net->dests[conn]){// check for advancing in a row
		if (net->sources[nex]==firstfrom){ end = conn = nex; lastto = net->dests[conn];
		    if (nex+1 == net->synapsecount) state=3; else backtrack = ++nex;
		} else if (net->dests[nex] >= firstto && net->dests[nex] <= lastto){
		    if (net->dests[nex] == lastto){conn = end = nex;lastfrom = net->sources[nex];backtrack = ++nex;state = nex+1 == net->synapsecount ? 3 : 2;}
		    else{if (nex+1==net->synapsecount){ conn = backtrack; state = 3;} else conn=nex++; }
		} else { conn = backtrack; state = 3;}
	    }else state=2; break;
	case 2:
	    if (net->dests[nex]==firstto && net->dests[conn]==lastto && net->sources[nex]==1+net->sources[conn]){// check for advancing in a column
		if (firstto == lastto){end = conn = nex; lastfrom = net->sources[nex]; if (nex+1 < net->synapsecount) nex++; else state = 3;}
		else {if (nex+1 == net->synapsecount){conn = backtrack; state = 3;} else {conn = nex; nex++; state = 1;}}
	    } else {conn = backtrack; state = 3;} break;
	case 3: // we've detected a complete expression.  print the connect statement and look for another.
	    fprintf(out, "    Connect(" );
	    if (firstfrom == lastfrom) fprintf(out,"%d ", firstfrom);    else fprintf(out, "{%d %d} ", firstfrom, lastfrom);
	    if (firstto == lastto) fprintf(out,"%d ", firstto);          else fprintf(out, "{%d %d} ", firstto, lastto);
	    if (start != end) {
		fprintf(out, "[");
		for (nex = start; nex <= end; nex++) fprintf(out, FLOFMT" ", net->weights[nex]); fprintf(out, "])\n");}
	    else fprintf(out, FLOFMT"\n", net->weights[start] );
	    if (end == net->synapsecount) state = 4;                     else {state = 0; conn = end+1;}
	case 4: break;
	default: {fprintf(stderr, "Program error in while/switch stmt of netwriter.\n"); exit(1);}
	}
    fprintf(out, "EndConnections\n");
}
