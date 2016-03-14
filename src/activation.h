/* activation.h -- This belongs to gneural_network

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

// returns the specified activation function given the input x

inline double activation(int type,double x){
 if(type==TANH)      return(tanh(x));
 else if(type==EXP)  return(1./(1.+exp(-x)));
 else if(type==ID)   return(x);
 else if(type==POL1) return(1.+x);
 else if(type==POL2) return(1.+x+x*x);
 else {
  printf("unknown activation function!\n");
  exit(0);
 }
}
