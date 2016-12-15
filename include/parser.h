/* parser.h -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef PARSER_H
#define PARSER_H

#include "includes.h"
#include "network.h"

// The number of different accumulator functions
#define ACCUMCOUNT 8
// the names of the accumulator functions
#define ACCTOKENS "None","Add","Multiply","SoftLimit","Magnitude","SoftLog","Maximum","Onesided"

// the number of different output function names
#define OUTPUTCOUNT 16
// the first SINGLEOUTPUTS function names are for output functions that operate on exactly one node.
#define SINGLEOUTPUTS 14
#define OUTTOKENS "Identity","Tanh","Arctan","Logistic","SignedLogistic","Softsign","Logarithmic","StepFunction", "ReLU","SoftPlus",\
"LogRectifier","Periodic","Gaussian","Spline","ParallelMult","ParallelMath"


#define BFLEN 128
struct slidingbuffer {
    FILE *input;
    int head;
    int end;
    int line;
    int col;
    char *warnings;
    char buffer[BFLEN];
};


void parser(network *, network_config*, FILE *);
void PrintWarnings(struct slidingbuffer *);
void nnetparser(struct nnet *, struct conf *, struct slidingbuffer *);
void debugnnet(struct nnet *net);

#endif
