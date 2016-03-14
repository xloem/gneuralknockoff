/* random_search.h -- This belongs to gneural_network

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

// perform a random search of the best weights of a network during the training process

void random_search(int output /* screen output - on/off */,
                   int nmax /* maximum number of random attempts */,
                   double eps /* accuracy of the method */){
 register int i,j,k;
 int n;
 double err,e0;
 double *wbackup;

 wbackup=malloc((NNUM*MAX_IN+1)*sizeof(*wbackup));
 if(wbackup==NULL){
  printf("RND: Not enough memory to allocate\ndouble *wbackup\n");
  exit(0);
 }


 err=1.e8; // just a big number
 e0=error(L2);

 for(n=0;(n<nmax) && (e0>eps);n++){
  // backup of the weights
  k=0;
  for(i=0;i<NNUM;i++){
   for(j=0;j<NEURON[i].nw;j++){
    wbackup[k]=NEURON[i].w[j];
    k++;
   }
  }
  // random weights
  randomize();
  // update error
  err=error(L2);
  if(err<e0){
   // keep the new state
   e0=err;
  } else {
   // restore the old state
   k=0;
   for(i=0;i<NNUM;i++){
    for(j=0;j<NEURON[i].nw;j++){
     NEURON[i].w[j]=wbackup[k];
     k++;
    }
   }
  }
 if(output==ON) printf("RND: %d %g\n",n,e0);
 }
 free(wbackup);
}
