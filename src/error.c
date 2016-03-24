/* error.c -- This belongs to gneural_network

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

// compute the specified error of a (training) neural network

#include "error.h"

double error(int type){
 register int n;
 double err;
 double y;

 err=0.;

 switch(type){
  // Linf norm
  case LINF:
   err=-1.e8;
   for(n=0;n<NDATA;n++){
    int i,j;
    // assign training input
    for(i=0;i<NETWORK.num_of_neurons[0];i++){
     int neuron_id;
     neuron_id=NETWORK.neuron_id[0][i];
     for(j=0;j<NEURON[neuron_id].nw;j++){
      NEURON[neuron_id].x[j]=NEURON[neuron_id].output=X[n][neuron_id][j];
     }
    }
//    NEURON[0].x[0]=NEURON[0].output=X[n];
    feedforward();
    // compute the L_inf error comparing with training output
    double tmp;
    tmp=0.;
    for(j=0;j<NETWORK.num_of_neurons[NETWORK.num_of_layers-1];j++){
     y=NEURON[NETWORK.neuron_id[NETWORK.num_of_layers-1][j]].output;
     tmp+=y-Y[n][NETWORK.neuron_id[NETWORK.num_of_layers-1][j]];
    }
    err+=fabs(tmp);
   }
   return(err);
   break;
  // L2 norm
  case L2:
   for(n=0;n<NDATA;n++){
    int i,j;
    // assign training input
    for(i=0;i<NETWORK.num_of_neurons[0];i++){
     int neuron_id;
     neuron_id=NETWORK.neuron_id[0][i];
     for(j=0;j<NEURON[neuron_id].nw;j++){
      NEURON[neuron_id].x[j]=NEURON[neuron_id].output=X[n][neuron_id][j];
     }
    }
//    NEURON[0].x[0]=NEURON[0].output=X[n];
    feedforward();
    // compute the L2 error comparing with the training output
    double tmp;
    tmp=0.;
    for(j=0;j<NETWORK.num_of_neurons[NETWORK.num_of_layers-1];j++){
     y=NEURON[NETWORK.neuron_id[NETWORK.num_of_layers-1][j]].output;
     tmp+=y-Y[n][NETWORK.neuron_id[NETWORK.num_of_layers-1][j]];
    }
    err+=pow(tmp,2.);
//    err+=pow(Y[n]-NEURON[5].output,2.);
   }
   return(sqrt(err));
   break;
  default:
   return(0.);
   break;
 }
}
