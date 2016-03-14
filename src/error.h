/* error.h -- This belongs to gneural_network

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

double error(int type){
 register int n;
 double err;

 err=0.;

 // Linf norm
 if(type==LINF){
  err=-1.e8;
  for(n=0;n<NDATA;n++){
   NEURON[0].x[0]=NEURON[0].output=X[n];
   feedforward();
   if(err<fabs(NEURON[5].output-Y[n])) err=fabs(NEURON[5].output-Y[n]);
  }
  return(err);
 }
 // L2 norm
 if(type==L2){
  for(n=0;n<NDATA;n++){
   NEURON[0].x[0]=NEURON[0].output=X[n];
   feedforward();
   err+=pow(Y[n]-NEURON[5].output,2.);
  }
  return(sqrt(err));
 }
 // cosine similarity
 if(type==COS){
  double s,x,y;
  s=0.;
  x=0.;
  y=0.;
  for(n=0;n<NDATA;n++){
   NEURON[0].x[0]=NEURON[0].output=X[n];
   feedforward();
   x+=pow(NEURON[5].output,2.);
   y+=pow(Y[n],2.);
   s+=NEURON[5].output*Y[n];
  }
  x=sqrt(x);
  y=sqrt(y);
  err=s/(x*y);
  return(err);
 }
 return(0.);
}
