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
// Version          : release 0.5.0
// Creation         : 11 Nov. 2012, Cassibile (SR), Italy
// Last Revision    : 18 Mar. 2016, Cassibile (SR), Italy
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

#include<getopt.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<memory.h>
#include<time.h>
#ifdef  HAVE_STRING_H
#include<string.h>
#else
#include<strings.h>
#endif

// maximum number of training point
#define MAX_TRAINING_POINTS 8

// maximum allowed number of inputs per neuron
#define MAX_IN 16

// maximum total number of neurons
#define MAX_NUM_NEURONS 32

// maximum number of layers
#define MAX_NUM_LAYERS 16

// maximum number of points in output file
#define MAX_NUM_POINTS 64

// definition of various internal types
#define OFF 0
#define ON  1
// activation functions
#define TANH     0
#define EXP      1
#define ID       2
#define POL1     3
#define POL2     4
// discriminant functions
#define LINEAR   0
#define LEGENDRE 1
#define LAGUERRE 2
#define FOURIER  3
// error/cost/target functions
#define L2   0
#define LINF 1
#define COS  2
// optimization methods
#define SIMULATED_ANNEALING 0
#define RANDOM_SEARCH       1
#define GRADIENT_DESCENT    2
#define GENETIC_ALGORITHM   3

// constants
const double PI=3.141592654;

typedef struct _neuron{
 int nw;
 // connection[i] is the identification number of the neuron which
 // output is connected to the i-th input branch of the neuron
 int connection[MAX_IN];
 int activation; // type of activation function
 int discriminant; // type of discriminant function
 double x[MAX_IN]; // n inputs
 double w[MAX_IN]; // n weights
 double output;    // one output
} neuron;

typedef struct _network{
 int num_of_layers; // total number of layers
 int num_of_neurons[MAX_NUM_LAYERS]; // number of neurons per layer
 // neuron_id[i][j] is the global identification number of the j-th neuron in the i-th layer
 int neuron_id[MAX_NUM_LAYERS][MAX_NUM_NEURONS];
} network;

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
int RATE;
int NXW;
int MAXITER;
int SAVE_OUTPUT;
int NUMBER_OF_POINTS;
int LOAD_NEURAL_NETWORK;
int SAVE_NEURAL_NETWORK;

double WMIN;
double WMAX;
double X[MAX_TRAINING_POINTS];
double Y[MAX_TRAINING_POINTS];
double KBTMIN;
double KBTMAX;
double GAMMA;
double ACCURACY;
double OUTPUT_X[MAX_NUM_POINTS];

FILE *fp;

static char *progname;
char OUTPUT_FILENAME[256];

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
   printf("gneural_network - Gneural Network 0.5.0\n");
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
   parser();
   fclose(fp);

   // initial value for random number generator
   ISEED=38467.;

   if(LOAD_NEURAL_NETWORK==OFF){
    // assigns weights randomly for each neuron
    // before the training process
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
     double x,y;
     x=OUTPUT_X[n];
     NEURON[0].x[0]=NEURON[0].output=x;
     feedforward();
     y=NEURON[NNUM-1].output;
     fprintf(fp,"%g %g\n",x,y);
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
