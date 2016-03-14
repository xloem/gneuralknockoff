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
// Version          : release 0.0.1
// Creation         : 11 Nov. 2012, Cassibile (SR), Italy
// Last Revision    : 04 Mar. 2016, Cassibile (SR), Italy
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

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<memory.h>
#include<string.h>
#include<time.h>

// maximum number of training point
#define MAX_TRAINING_POINTS 8

// maximum allowed number of inputs per neuron
#define MAX_IN 16

// maximum total number of neurons
#define MAX_NUM_NEURONS 32

// maximum number of layers
#define MAX_NUM_LAYERS 16

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

neuron NEURON[MAX_NUM_NEURONS];
network NETWORK;

int NNUM;
int NDATA;
int ISEED;

double WMIN;
double WMAX;
double X[MAX_TRAINING_POINTS];
double Y[MAX_TRAINING_POINTS];

FILE *fp;

#include "rnd.h"
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
 // initial value for random number generator
 ISEED=38467.;

 // ###############################################
 // The purpose of the following example is to fit
 // a curve given by three points. The points
 // are represented by the arrays below X[] and Y[]
 // ###############################################

 // data for supervised learning
 NDATA=3; // number of training points

 // training point coordinates
/* X[0]=0.1; Y[0]=pow(X[0],2.);
 X[1]=0.5; Y[1]=pow(X[1],2.);
 X[2]=0.9; Y[2]=pow(X[2],2.);*/
 X[0]=0.15; Y[0]=sqrt(X[0]);
 X[1]=0.60; Y[1]=sqrt(X[1]);
 X[2]=0.80; Y[2]=sqrt(X[2]);

 // interval for the search of weights
/* WMIN=-12.;
 WMAX=+12.;*/
 WMIN=-1.;
 WMAX=+1.;

 // total number of neurons in the network
 NNUM=6;

 // defines the number of inputs for every single neuron
 NEURON[0].nw=1;
 NEURON[1].nw=1;
 NEURON[2].nw=1;
 NEURON[3].nw=1;
 NEURON[4].nw=1;
 NEURON[5].nw=4;

 // defines the activation function for every neuron
 // first layer does not need to be specified as it has always an identity activation function
// NEURON[0].activation=ID;
 NEURON[1].activation=TANH;
 NEURON[2].activation=TANH;
 NEURON[3].activation=TANH;
 NEURON[4].activation=TANH;
 NEURON[5].activation=TANH;

 // defines the discriminant function for every neuron
 // first layer does need to be speicified as it is the identity function
 NEURON[1].discriminant=LINEAR;
 NEURON[2].discriminant=LINEAR;
 NEURON[3].discriminant=LINEAR;
 NEURON[4].discriminant=LINEAR;
 NEURON[5].discriminant=LINEAR;

 // defines the topology of the network
 // in this specific case, the first (input) layer has 1 neurons,
 // the (only) hidden layer has four layers
 // the last (output) layer has only one neuron
 NETWORK.num_of_layers=3;
 NETWORK.num_of_neurons[0]=1;
 NETWORK.num_of_neurons[1]=4;
 NETWORK.num_of_neurons[2]=1;
 // first layer
 NETWORK.neuron_id[0][0]=0;
 // second layer
 NETWORK.neuron_id[1][0]=1;
 NETWORK.neuron_id[1][1]=2;
 NETWORK.neuron_id[1][2]=3;
 NETWORK.neuron_id[1][3]=4;
 // third layer
 NETWORK.neuron_id[2][0]=5;

 // defines connections between neurons
 // the first layer [0] does not need anything
 // as its neurons are not connected to any neuron
 NEURON[1].connection[0]=0;
 NEURON[2].connection[0]=0;
 NEURON[3].connection[0]=0;
 NEURON[4].connection[0]=0;
 NEURON[5].connection[0]=1;
 NEURON[5].connection[1]=2;
 NEURON[5].connection[2]=3;
 NEURON[5].connection[3]=4;

 // assigns weights randomly for each neuron
 // before the training process
 randomize();

 // network training method
 simulated_annealing(ON,25,25000,1.e-4,8.,1.e-2); // - simulated annealing
// random_search(ON,500,1.e-3); // totally random search
// gradient_descent(ON,32,5000,0.01,1.e-6); // gradient descent method
// genetic_algorithm(ON,2048,1024,0.1,1.e-4); // home made genetic algorithm
// hybrid_optimization(ON,32,50,25000,512,512,500,1.e-4,8.,0.1,1.e-6); // hybrid method

 int n;
 int nx=32;
 double min=0.;
 double max=1.;

 // saves the final curve
 fp=fopen("final_result.dat","w");
 for(n=0;n<nx;n++){
  double x,y,r;
  x=min+(n+0.5)*(max-min)/nx;
  y=sqrt(x);
//  y=pow(x,2.);
  NEURON[0].x[0]=NEURON[0].output=x;
  feedforward();
  r=NEURON[5].output;
  fprintf(fp,"%g %g %g\n",x,y,r);
 }
 fclose(fp);

 return(0);
}
