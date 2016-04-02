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

// initially implemented by : Jean Michel Sellier
// drastically improved by  : Nan

#include "genetic_algorithm.h"

typedef struct{
 double error;
 double* weights;
} individual_t;

int actual_weight_count(void){
 int k=0;
 int i,j;
 for(i=0;i<NNUM;++i){
  for(j=0;j<NEURON[i].nw;++j){
    k++;
  }
 }
 return k;
}

void crossover(double w1,double w2,double* n1,double* n2){
 static int POSS=1;
 double average=(w1+w2)/2;
 double mid=(WMAX+WMIN)/2;
 *n1=average;
 if(average>mid){
  *n2=average-(WMAX-WMIN)/2;
 }
 else if(average<mid){
  *n2=average+(WMAX-WMIN)/2;
 } else /* == mid*/{
  if(POSS){
   *n2=WMAX;
   POSS=0;
  }
  else {
   *n2=WMIN;
   POSS=1;
  }
 }
}

void mutation(double* weight,double rate){
 if(rnd()>rate) return;
 if(rnd()>0.5){
  /* go plus */
  *weight+=(rnd()*(WMAX-WMIN)/2);
  if(*weight>WMAX) *weight-=(WMAX-WMIN);
 } else {
  /* go minus */
  *weight-=(rnd()*(WMAX-WMIN)/2);
  if(*weight<WMIN) *weight+=(WMAX-WMIN);
 }
}

int individual_compare(const void* a,const void* b){
 individual_t* ia=*(individual_t**)a;
 individual_t* ib=*(individual_t**)b;
 if(ia->error>ib->error) return 1;
 if(ia->error<ib->error) return -1;
 return 0;
}

void init_individuals(individual_t** individuals,int size){
 int n;
 for(n=0;n<size;++n){
  int k=0;
  int i,j;
  for(i=0;i<NNUM;i++){
   for(j=0;j<NEURON[i].nw;j++){
    individuals[n]->weights[k]=rnd();
    k++;
   }
  }
 }
}

void reproduce_next_generation(individual_t** individuals,int size,int weight_cout,double rate){
 int pos=size;
 int i,j,k;
 for(i=0;i<size;++i){
  for(j=i+1;j<size;++j){
   for(k=0;k<weight_cout;++k){
    double w1=individuals[i]->weights[k];
    double w2=individuals[j]->weights[k];
    crossover(w1,w2,individuals[pos]->weights+k,individuals[pos+1]->weights+k);
    mutation(individuals[pos]->weights+k,rate);
    mutation(individuals[pos+1]->weights+k,rate);
   }
   pos+=2;
  }
 }
}

void selection(individual_t** individuals,int size){
 int pool_size=size*size;
 int n;
 for(n=0;n<pool_size;++n){
  int k=0;
  int i,j;
  for(i=0;i<NNUM;i++){
   for(j=0;j<NEURON[i].nw;j++){
    NEURON[i].w[j]=individuals[n]->weights[k];
    k++;
   }
  }
  individuals[n]->error=error(ERROR_TYPE);
 }
 qsort(individuals,pool_size,sizeof(individual_t*),individual_compare);
}

void genetic_algorithm(int output /* screen output - on/off */,
                       int nmax /* number of generations */,
                       int npop /* number of individuals per generation */,
                       double rate /* rate of change between one generation and the parent */,
                       double eps /* numerical accuracy */) {
 int i,j,k,n;

 int pool_size=npop*npop;
 individual_t** individuals=malloc(pool_size*sizeof(individual_t*));
 if(individuals==NULL){
  printf("GA: Not enough memory to allocate individual index table\n");
  exit(0);
 }

 for(i=0;i<pool_size;++i){
  individuals[i]=malloc(sizeof(individual_t));
  if(individuals[i]==NULL){
   printf("GA: Not enough memory to allocate individuals\n");
   exit(0);
  }
  individuals[i]->weights=malloc(NNUM*MAX_IN*sizeof(double));
  if(individuals[i]->weights==NULL){
   printf("GA: Not enough memory to allocate weights\n");
   exit(0);
  }
 }
 init_individuals(individuals,npop);
 int weight_cout=actual_weight_count();

 for(n=0;n<nmax;++n){
  reproduce_next_generation(individuals,npop,weight_cout,rate);
  selection(individuals,npop);
  if(output==ON) printf("GA2: %d %.12g\n",n,individuals[0]->error);
  if(individuals[0]->error<eps) break;
 }

 if(individuals[0]->error>eps){
  if(output==ON) printf("GA2: error still greater than %g after %d iterations",eps,nmax);
 }

 k=0;
 for(i=0;i<NNUM;i++){
  for(j=0;j<NEURON[i].nw;j++){
   NEURON[i].w[j]=individuals[0]->weights[k];
   k++;
  }
 }

 for(i=0;i<pool_size;++i){
  free(individuals[i]->weights);
  free(individuals[i]);
 }
 free(individuals);
}


//void genetic_algorithm(int output /* screen output - on/off */,
//                       int nmax /* number of generations */,
//                       int npop /* number of individuals per generation */,
//                       double rate /* rate of change between one generation and the parent */,
//                       double eps /* numerical accuracy */) {
//    int m, n;
//    int m_best;
//    register int i, j, k;
//    double* err;
//    double err_tmp;
//    double err_best;
//    double* w;
//    double** wbest;

//    err_tmp = err_best = 1.e8; // just a big number

//    err = malloc((npop + 1) * sizeof(*err));
//    if (err == NULL) {
//        printf("GA: Not enough memory to allocate\ndouble *err\n");
//        exit(0);
//    }

//    w = malloc((NNUM * MAX_IN + 1) * sizeof(*w));
//    if (w == NULL) {
//        printf("GA: Not enough memory to allocate\ndouble *w\n");
//        exit(0);
//    }

//    wbest = (double**)malloc((npop + 1) * sizeof(double*));
//    if (wbest == NULL) {
//        printf("GA: Not enough memory to allocate\ndouble *wbest[npop+1]\n");
//        exit(0);
//    }
//    for (i = 0; i <= npop; i++) {
//        wbest[i] = (double*)malloc((NNUM * MAX_IN + 1) * sizeof(double));
//        if (wbest[i] == NULL) {
//            printf("GA: Not enough memory to allocate\ndouble wbest[%d][NNUM*<AX_IN+1]\n", i);
//            exit(0);
//        }
//    }

// starts the evolution from a first (random) generation
//    for (m = 0; m < npop; m++) {
//        k = 0;
//        for (i = 0; i < NNUM; i++) {
//            for (j = 0; j < NEURON[i].nw; j++) {
//                if (m == 0) wbest[m][k] = NEURON[i].w[j];
//                else wbest[m][k] = NEURON[i].w[j] + rate * rnd() * (WMAX - WMIN);
//                k++;
//            }
//        }
//    }
// main loop over generations
//    for (n = 0; (n < nmax) && (err_tmp > eps); n++) {
        // computes the error for every element
//        for (m = 0; m < npop; m++) {
//            k = 0;
//            for (i = 0; i < NNUM; i++) {
//                for (j = 0; j < NEURON[i].nw; j++) {
//                    NEURON[i].w[j] = wbest[m][k];
//                    k++;
//                }
//            }
//            err[m] = error(ERROR_TYPE);
//        }
        // checks for the best element
//        m_best = -1000;
//        for (m = 0; m < npop; m++) {
//            if (err[m] < err_best) {
//                m_best = m;
//                err_best = err[m];
//            }
//        }
        // updates the weights
//        if (m_best != -1000) {
//            k = 0;
//            for (i = 0; i < NNUM; i++) {
//                for (j = 0; j < NEURON[i].nw; j++) {
//                    NEURON[i].w[j] = w[k] = wbest[m_best][k];
//                    k++;
//                }
//            }
//            err_tmp = err_best;
//        }
        // creates a new generation from the best breed
//        for (m = 0; m < npop; m++) {
//            k = 0;
//            for (i = 0; i < NNUM; i++) {
//                for (j = 0; j < NEURON[i].nw; j++) {
//                    wbest[m][k] = NEURON[i].w[j] + rate * rnd() * (WMAX - WMIN);
//                    k++;
//                }
//            }
//        }
//        if (output == ON) printf("GA: %d %g\n", n, err_best);
//    }
// final update of the weights
//    k = 0;
//    for (i = 0; i < NNUM; i++) {
//        for (j = 0; j < NEURON[i].nw; j++) {
//            NEURON[i].w[j] = w[k];
//            k++;
//        }
//    }
//    if (output == ON) printf("\n");
//    free(err);
//    free(w);
//    for (i = 0; i <= npop; i++) {
//        free(wbest[i]);
//    }
//    free(wbest);
//}
