/* simulated_annealing.c -- This belongs to gneural_network

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

// train the neural network by means of simulated annealing to optimize the weights

#include "simulated_annealing.h"

void simulated_annealing(int output /* screen output - on/off */,
                         int mmax /* outer loop - number of effective temperature steps*/,
                         int nmax /* inner loop - number of test configurations */,
                         double kbtmin /* effective temperature minimum */,
                         double kbtmax /* effective temperature maximum */,
                         double eps /* numerical accuracy */){
 register int m,n;
 int i,j,k;
 double err;
 double e0;
 double de;
 double kbt;
 double e_best;
 double *wbackup;
 double *wbest;

 wbackup=malloc((NNUM*MAX_IN+1)*sizeof(*wbackup));
 if(wbackup==NULL){
  printf("RND: Not enough memory to allocate\ndouble *wbackup\n");
  exit(0);
 }

 wbest=malloc((NNUM*MAX_IN+1)*sizeof(*wbest));
 if(wbest==NULL){
  printf("SA: Not enough memory to allocate\ndouble *wbest\n");
  exit(0);
 }


 e_best=err=e0=1.e8; // just a big number

 for(m=0;(m<mmax) && (e0>eps);m++){

  kbt=kbtmax-m*(kbtmax-kbtmin)/(mmax-1);

  for(n=0;(n<nmax) && (e0>eps);n++){
   // backup the old weights
   k=0;
   for(i=0;i<NNUM;i++){
    for(j=0;j<NEURON[i].nw;j++){
     wbackup[k]=NEURON[i].w[j];
     k++;
    }
   }
   // new random configuration
   randomize();
   // compute the error
   err=error(ERROR_TYPE);
   // update energy landscape
   de=err-e0;
   // decides what configuration to keep
   if(de>0.){
    // if(de<=0.) just accept the new configuration
    // otherwise treat it probabilistically
    double p;
    p=exp(-e0/kbt);
    if(rnd()<p){
     // accept the new configuration
     e0=err;
    } else {
     // reject the new configuration
     k=0;
     for(i=0;i<NNUM;i++){
      for(j=0;j<NEURON[i].nw;j++){
       NEURON[i].w[j]=wbackup[k];
       k++;
      }
     }
    }
   } else {
    // accept the new configuration
    e0=err;
   }
   if(e_best>e0){
    k=0;
    for(i=0;i<NNUM;i++){
     for(j=0;j<NEURON[i].nw;j++){
      wbest[k]=NEURON[i].w[j];
      k++;
     }
    }
    e_best=e0;
   }
  }
  if(output==ON) printf("SA: %d %g %g\n",m,kbt,e_best);
 }
 // keep the best solution found
 k=0;
 for(i=0;i<NNUM;i++){
  for(j=0;j<NEURON[i].nw;j++){
   NEURON[i].w[j]=wbest[k];
   k++;
  }
 }
 if(output==ON) printf("\n");
 free(wbackup);
 free(wbest);
}
