/* gneural_network.c -- This belongs to gneural_network

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

// =================================================================
// File Name        : gneural_network.c
// Version          : release 0.8.0
// Creation         : 11 Nov. 2012, Cassibile (SR), Italy
// Last Revision    : 01 Apr. 2016, Cassibile (SR), Italy
// =================================================================

/*

   Programmable Neural Network. Every neuron consists of
   n-inputs and only one output. The activation
   and discriminant functions are defined by the user (see below).
   Every network is made of layers which forward propagate
   information from left to right. Every layer can have
   an arbitrary amount of neurons which are connected
   with each other.

   Every neuron has a global ID number and a local ID number
   (inside the layer). This simplifies the description of
   connections in a network.

*/

#include "includes.h"
#include "defines.h"
#include "structures.h"

// constants
const double PI=3.141592654;

struct option longopts[] =
{
  { "version", no_argument, NULL, 'v' },
  { "help", no_argument, NULL, 'h' }
};

neuron NEURON[MAX_NUM_NEURONS];
network NETWORK;

int NNUM;
int NDATA;
int ISEED;
int OPTIMIZATION_METHOD;
int VERBOSITY;
int MMAX;
int NMAX;
int NPOP;
int NXW;
int MAXITER;
int SAVE_OUTPUT;
int ERROR_TYPE;
int NUMBER_OF_POINTS;
int LOAD_NEURAL_NETWORK;
int SAVE_NEURAL_NETWORK;
int INITIAL_WEIGHTS_RANDOMIZATION;

double RATE;
double WMIN;
double WMAX;
double X[MAX_TRAINING_POINTS][MAX_NUM_NEURONS][MAX_IN];
double Y[MAX_TRAINING_POINTS][MAX_NUM_NEURONS];
double KBTMIN;
double KBTMAX;
double GAMMA;
double ACCURACY;
double OUTPUT_X[MAX_NUM_POINTS][MAX_NUM_NEURONS][MAX_IN];

FILE *fp;

static char *progname;
char OUTPUT_FILENAME[256];
char LOAD_NEURAL_NETWORK_FILENAME[256];
char SAVE_NEURAL_NETWORK_FILENAME[256];

#include "rnd.h"
#include "parser.h"
#include "load.h"
#include "save.h"
#include "fact.h"
#include "binom.h"
#include "randomize.h"
#include "activation.h"
#include "feedforward.h"
#include "error.h"
#include "msmco.h"
#include "random_search.h"
#include "simulated_annealing.h"
#include "gradient_descent.h"
#include "genetic_algorithm.h"

int main(int argc,char* argv[])
{
 int optc;
 int h=0,v=0,lose=0,z=0;

 progname=argv[0];

 while((optc=getopt_long(argc,argv,"hv",longopts,(int *) 0))!= EOF)
  switch (optc){
   case 'v':
    v=1;
    break;
   case 'h':
    h=1;
    break;
   default:
    lose=1;
    break;
  }

  if(optind==argc-1) z=1;
  else if(lose || optind < argc){
   // dump an error message and exit.
   if (optind<argc) printf("Too many arguments\n");
   printf("Try `%s --help' for more information.\n",progname);
   exit(1);
  }

  // `help' should come first.  If `help' is requested, ignore the other options.
  if(h){
   /* Print help info and exit.  */
   /* TRANSLATORS: --help output 1
      no-wrap */
   printf("\
Gneural Network, the GNU package for neural networks.\nCopyright (C) 2016 Jean Michel Sellier.\n");
   printf ("\n");
   /* TRANSLATORS: --help output 2
      no-wrap */
   printf ("\
Usage: %s [OPTION] file...\n",progname);

   printf ("\n");
   /* TRANSLATORS: --help output 3 : options 1/2
      no-wrap */
   printf("\
  -h, --help          display this help and exit\n\
  -v, --version       display version information and exit\n");

   printf ("\n");
   /* TRANSLATORS: --help output 5 (end)
      TRANSLATORS, please don't forget to add the contact address for
      your translation!
      no-wrap */
   printf ("\
Report bugs to jeanmichel.sellier@gmail.com\n");
      exit (0);
    }

  if(v){
   /* Print version number.  */
   printf("gneural_network - Gneural Network 0.8.0\n");
   /* xgettext: no-wrap */
   printf("\n");
   printf("\
Copyright (C) %s Jean Michel Sellier.\n\n\
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.\n\
You may redistribute copies of GNU %s under the terms\n\
of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n",
"2016","Gneural Network");
   exit (0);
  }
  else if (z){
   // if the filename is specified then proceed with parsing the script and then run the calculations
   fp=fopen(argv[1],"r");
   // check, just in case the file does not exist...
   if(fp==NULL){
    printf("%s: fatal error in opening the input file %s\n",
           progname,argv[1]);
    exit(EXIT_FAILURE);
   }
   // some defaults are defined before parsing the input script
   SAVE_OUTPUT=OFF;
   LOAD_NEURAL_NETWORK=OFF;
   SAVE_NEURAL_NETWORK=OFF;
   INITIAL_WEIGHTS_RANDOMIZATION=ON;
   ERROR_TYPE=L2;
   parser();
   fclose(fp);

   // initial value for random number generator
   ISEED=38467.;

   if(LOAD_NEURAL_NETWORK==OFF){
    // assigns weights randomly for each neuron
    // before the training process
    if(INITIAL_WEIGHTS_RANDOMIZATION==ON)
     randomize();

    // network training method
    if(OPTIMIZATION_METHOD==SIMULATED_ANNEALING)
     simulated_annealing(VERBOSITY,MMAX,NMAX,KBTMIN,KBTMAX,ACCURACY);
    if(OPTIMIZATION_METHOD==RANDOM_SEARCH)
     random_search(VERBOSITY,NMAX,ACCURACY);
    if(OPTIMIZATION_METHOD==GRADIENT_DESCENT)
     gradient_descent(VERBOSITY,NXW,MAXITER,GAMMA,ACCURACY);
    if(OPTIMIZATION_METHOD==GENETIC_ALGORITHM)
     genetic_algorithm(VERBOSITY,NMAX,NPOP,RATE,ACCURACY);
    if(OPTIMIZATION_METHOD==MSMCO)
     msmco(VERBOSITY,MMAX,NMAX,RATE);
    if(SAVE_NEURAL_NETWORK==ON) save(ON);
   } else {
    // load the neural network
    load(ON);
   }
   if(SAVE_OUTPUT==ON){
    printf("saving %s\n",OUTPUT_FILENAME);
    int n;
    // saves the final curve
    fp=fopen(OUTPUT_FILENAME,"w");
    for(n=0;n<NUMBER_OF_POINTS;n++){
     int i,j;
     double x,y;
     for(i=0;i<NETWORK.num_of_neurons[0];i++){
      int neuron_id;
      neuron_id=NETWORK.neuron_id[0][i];
      for(j=0;j<NEURON[neuron_id].nw;j++){
       x=OUTPUT_X[n][neuron_id][j];
       NEURON[neuron_id].x[j]=NEURON[neuron_id].output=x;
       fprintf(fp,"%g ",x);
      }
     }
     feedforward();
     for(j=0;j<NETWORK.num_of_neurons[NETWORK.num_of_layers-1];j++){
      y=NEURON[NETWORK.neuron_id[NETWORK.num_of_layers-1][j]].output;
      fprintf(fp,"%g ",y);
     }
     fprintf(fp,"\n");
    }
    fclose(fp);
   }
  }
  else{
   // filename not specified
   printf("%s: no input file\n",progname);
   exit(0);
  }
 return(0);
}
