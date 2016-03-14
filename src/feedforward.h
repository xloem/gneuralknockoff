/* feedforward.h -- This belongs to gneural_network

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

// feedforward propagation of information through the neural network

void feedforward(void){
 register int i,j;
 register int l,n;

 for(l=1;l<NETWORK.num_of_layers;l++){
  for(n=0;n<NETWORK.num_of_neurons[l];n++){
   int id;
   double x;
   x=0.;
   id=NETWORK.neuron_id[l][n];
   for(i=0;i<NEURON[id].nw;i++) NEURON[id].x[i]=NEURON[NEURON[id].connection[i]].output;
   if(NEURON[id].discriminant==LINEAR){
    for(i=0;i<NEURON[id].nw;i++)
     x+=NEURON[id].x[i]*NEURON[id].w[i]; // linear product between w[] and x[]
   } else if(NEURON[id].discriminant==LEGENDRE){
    double a,tmp;
    for(i=0;i<NEURON[id].nw;i++){
     a=pow(2,i);
     tmp=0.;
     for(j=0;j<=i;j++){
      tmp+=pow(NEURON[id].x[i],j)*binom(i,j)*binom((i+j-1)/2,j);
     }
     tmp*=a*NEURON[id].w[i];
     x+=tmp;
    }
   } else if(NEURON[id].discriminant==LAGUERRE){
    double tmp;
    for(i=0;i<NEURON[id].nw;i++){
     tmp=0.;
     for(j=0;j<=i;j++) tmp+=binom(i,j)*pow(NEURON[id].x[i],j)*pow(-1,j)/fact(j);
     tmp*=NEURON[id].w[i];
     x+=tmp;
    }
   } else if(NEURON[id].discriminant==FOURIER){
    double tmp;
    for(i=0;i<NEURON[id].nw;i++){
     tmp=0.;
     for(j=0;j<=i;j++) tmp+=sin(2.*j*PI*NEURON[id].x[i]);
     tmp*=NEURON[id].w[i];
     x+=tmp;
    }

   }
   NEURON[id].output=activation(NEURON[id].activation,x);
  }
 }

}
