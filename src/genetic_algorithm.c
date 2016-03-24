/* genetic_algorithm.c -- This belongs to gneural_network

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

// performs a genetic search of the best weights during the training process

#include "genetic_algorithm.h"

void genetic_algorithm(int output /* screen output - on/off */,
                       int nmax /* number of generations */,
                       int npop /* number of individuals per generation */,
                       double rate /* rate of change between one generation and the parent */,
                       double eps /* numerical accuracy */){
 int m,n;
 int m_best;
 register int i,j,k;
 double *err;
 double err_tmp;
 double err_best;
 double *w;
 double **wbest;

 err_tmp=err_best=1.e8; // just a big number

 err=malloc((npop+1)*sizeof(*err));
 if(err==NULL){
  printf("GA: Not enough memory to allocate\ndouble *err\n");
  exit(0);
 }

 w=malloc((NNUM*MAX_IN+1)*sizeof(*w));
 if(w==NULL){
  printf("GA: Not enough memory to allocate\ndouble *w\n");
  exit(0);
 }

 wbest=(double **)malloc((npop+1)*sizeof(double*));
 if(wbest==NULL){
  printf("GA: Not enough memory to allocate\ndouble *wbest[npop+1]\n");
  exit(0);
 }
 for(i=0;i<=npop;i++){
  wbest[i]=(double *)malloc((NNUM*MAX_IN+1)*sizeof(double));
  if(wbest[i]==NULL){
   printf("GA: Not enough memory to allocate\ndouble wbest[%d][NNUM*<AX_IN+1]\n",i);
   exit(0);
  }
 }

 // starts the evolution from a first (random) generation
 for(m=0;m<npop;m++){
  k=0;
  for(i=0;i<NNUM;i++){
   for(j=0;j<NEURON[i].nw;j++){
    if(m==0) wbest[m][k]=NEURON[i].w[j];
    else wbest[m][k]=NEURON[i].w[j]+rate*rnd()*(WMAX-WMIN);
    k++;
   }
  }
 }
 // main loop over generations
 for(n=0;(n<nmax) && (err_tmp>eps);n++){
  // computes the error for every element
  for(m=0;m<npop;m++){
   k=0;
   for(i=0;i<NNUM;i++){
    for(j=0;j<NEURON[i].nw;j++){
     NEURON[i].w[j]=wbest[m][k];
     k++;
    }
   }
   err[m]=error(ERROR_TYPE);
  }
  // checks for the best element
  m_best=-1000;
  for(m=0;m<npop;m++){
   if(err[m]<err_best){
    m_best=m;
    err_best=err[m];
   }
  }
  // updates the weights
  if(m_best!=-1000){
   k=0;
   for(i=0;i<NNUM;i++){
    for(j=0;j<NEURON[i].nw;j++){
     NEURON[i].w[j]=w[k]=wbest[m_best][k];
     k++;
    }
   }
   err_tmp=err_best;
  }
  // creates a new generation from the best breed
  for(m=0;m<npop;m++){
   k=0;
   for(i=0;i<NNUM;i++){
    for(j=0;j<NEURON[i].nw;j++){
     wbest[m][k]=NEURON[i].w[j]+rate*rnd()*(WMAX-WMIN);
     k++;
    }
   }
  }
  if(output==ON) printf("GA: %d %g\n",n,err_best);
 }
 // final update of the weights
 k=0;
 for(i=0;i<NNUM;i++){
  for(j=0;j<NEURON[i].nw;j++){
   NEURON[i].w[j]=w[k];
   k++;
  }
 }
 if(output==ON) printf("\n");
 free(err);
 free(w);
 for(i=0;i<=npop;i++){
  free(wbest[i]);
 }
 free(wbest);
}
