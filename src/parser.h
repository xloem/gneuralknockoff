/* parser.h -- This belongs to gneural_network

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

// read the input script to define the network and its task

void parser(void){
 int ret;
 char s[180];
 double tmp;

 printf("\n\
=========================\n\
processing the input file\n\
=========================\n");
 do{
  // read the current row
  ret=fscanf(fp,"%s",s);
  // COMMENTS
  if(strcmp(s,"#")==0){
    fgets(s,80,fp);
    printf("COMMENT ---> %s",s);
  }
  // NUMBER OF NEURONS
  else if(strcmp(s,"NUMBER_OF_NEURONS")==0){
   ret=fscanf(fp,"%lf",&tmp);
   int nnum=(int)(tmp);
   if(tmp!=nnum){
    printf("NUMBER_OF_NEURONS must be an integer number!\n");
    exit(0);
   }
   if(nnum<=0){
    printf("NUMBER_OF_NEURONS must be a positive number!\n");
    exit(0);
   }
   printf("TOTAL NUMBER OF NEURONS = %d [OK]\n",nnum);
   NNUM=nnum;
  }
  // DEFINITION OF NEURONS
  else if(strcmp(s,"NEURON")==0){
   // read the identification number (index) of the neuron
   ret=fscanf(fp,"%lf",&tmp);
   int index=(int)(tmp);
   if(index!=tmp){
    printf("neuron id must an integer!\n");
    exit(0);
   }
   if(index<0){
    printf("neuron id must be positive!\n");
    exit(0);
   }
   if(index>(NNUM-1)){
    printf("neuron id out of range!\n");
    exit(0);
   }
   // specify the number of input connections
   // syntax: NEURON ind NUMBER_OF_CONNECTIONS num
   ret=fscanf(fp,"%s",s);
   if(strcmp(s,"NUMBER_OF_CONNECTIONS")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int num_of_connections=(int)(tmp);
    if(num_of_connections!=tmp){
     printf("number of connections must be an integer!\n");
     exit(0);
    }
    if(num_of_connections<0){
     printf("number of connections must be positive!\n");
     exit(0);
    }
    if(num_of_connections>MAX_IN){
     printf("number of connections is too big!\n");
     printf("please increase MAX_IN and recompile!\n");
     exit(0);
    }
    printf("NEURON %d NUMBER_OF_CONNECTIONS = %d [OK]\n",index,num_of_connections);
    NEURON[index].nw=num_of_connections;
   }
   // specify the activation 
   // syntax: NEURON ind ACTIVATION function
   else if(strcmp(s,"ACTIVATION")==0){
    ret=fscanf(fp,"%s",s);
    if(strcmp(s,"TANH")==0){
     NEURON[index].activation=TANH;
     printf("NEURON %d ACTIVATION = TANH\n",index);
    }
    else if(strcmp(s,"EXP")==0){
     NEURON[index].activation=EXP;
     printf("NEURON %d ACTIVATION = EXP\n",index);
    }
    else if(strcmp(s,"ID")==0){
     NEURON[index].activation=ID;
     printf("NEURON %d ACTIVATION = ID\n",index);
    }
    else if(strcmp(s,"POL1")==0){
     NEURON[index].activation=POL1;
     printf("NEURON %d ACTIVATION = POL1\n",index);
    }
    else if(strcmp(s,"POL2")==0){
     NEURON[index].activation=POL2;
     printf("NEURON %d ACTIVATION = POL2\n",index);
    } else {
     printf("the specified activation function is unknown!\n");
     exit(0);
    }
   }
   // specify the discriminant
   // syntax: NEURON ind DISCRIMINANT function
   else if(strcmp(s,"DISCRIMINANT")==0){
    ret=fscanf(fp,"%s",s);
    if(strcmp(s,"LINEAR")==0){
     NEURON[index].discriminant=LINEAR;
     printf("NEURON %d DISCRIMINANT = LINEAR\n",index);
    }
    else if(strcmp(s,"LEGENDRE")==0){
     NEURON[index].discriminant=LEGENDRE;
     printf("NEURON %d DISCRIMINANT = LEGENDRE\n",index);
    }
    else if(strcmp(s,"LAGUERRE")==0){
     NEURON[index].discriminant=LAGUERRE;
     printf("NEURON %d DISCRIMINANT = LAGUERRE\n",index);
    }
    else if(strcmp(s,"FOURIER")==0){
     NEURON[index].discriminant=FOURIER;
     printf("NEURON %d DISCRIMINANT = FOURIER\n",index);
    } else {
     printf("the specified discriminant function is unknown!\n");
     exit(0);
    }
   }
   // specify the connections
   // the neuron ind has its connection_id-th input connection connected to the output of global_neuron_2
   // syntax: NEURON ind CONNECTION connection_id global_neuron_id_2
   else if(strcmp(s,"CONNECTION")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int connection_id=(int)(tmp);
    if(connection_id!=tmp){
     printf("the connection index must be an integer!\n");
     exit(0);
    }
    if(connection_id<0){
     printf("the connection index must be positive!\n");
     exit(0);
    }
    if(connection_id>(NEURON[index].nw-1)){
     printf("the connection index is out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int global_neuron_id_2=(int)(tmp);
    if(global_neuron_id_2!=tmp){
     printf("the global index of neuron #2 must be an integer!\n");
     exit(0);
    }
    if(global_neuron_id_2<0){
     printf("the global index of neuron #2 must be positive!\n");
     exit(0);
    }
    if(global_neuron_id_2>(NNUM-1)){
     printf("the global index of neuron #2 is out of range!\n");
     exit(0);
    }
    printf("NEURON %d CONNECTION %d %d [OK]\n",index,connection_id,global_neuron_id_2);
    NEURON[index].connection[connection_id]=global_neuron_id_2;
   } else {
    printf("the specified neuron feature is unknown!\n");
    exit(0);
   }
  }
  else if(strcmp(s,"NETWORK")==0){
   ret=fscanf(fp,"%s",s);
   // specify the number of layers
   // syntax: NETWORK NUMBER_OF_LAYERS num
   if(strcmp(s,"NUMBER_OF_LAYERS")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int num_layers=(int)(tmp);
    if(num_layers!=tmp){
     printf("NUMBER_OF_LAYERS must be an integer!\n");
     exit(0);
    }
    if(num_layers<0){
     printf("NUMBER_OF_LAYERS must be positive!\n");
     exit(0);
    }
    if(num_layers>MAX_NUM_LAYERS){
     printf("NUMBER_OF_LAYERS is too large!\n");
     printf("modify MAX_NUM_LAYERS and recompile!\n");
     exit(0);
    }
    NETWORK.num_of_layers=num_layers;
    printf("NETWORK NUMBER_OF_LAYERS = %d [OK]\n",num_layers);
   }
   // specify the number of neurons of a layer
   // syntax: NETWORK LAYER ind NUMBER_OF_NEURONS num
   else if(strcmp(s,"LAYER")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int ind=(int)(tmp);
    if(ind!=tmp){
     printf("layer index must be an integer!\n");
     exit(0);
    }
    if(ind<0){
     printf("layer index must be positive!\n");
     exit(0);
    }
    if(ind>(NETWORK.num_of_layers-1)){
     printf("layer index is out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%s",s);
    if(strcmp(s,"NUMBER_OF_NEURONS")!=0){
     printf("syntax error!\n");
     printf("NUMBER_OF_NEURONS expected!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int num=(int)(tmp);
    if(num!=tmp){
     printf("the number of neurons must be an integer!\n");
     exit(0);
    }
    if(num<0){
     printf("the number of neurons must be positive!\n");
     exit(0);
    }
    if(num>MAX_NUM_NEURONS){
     printf("the number of neurons is too large!\n");
     printf("please increase MAX_NUM_NEURONS and recompile!\n");
     exit(0);
    }
    NETWORK.num_of_neurons[ind]=num;
    printf("NETWORK LAYER %d NUMBER_OF_NEURONS %d [OK]\n",ind,num);
   }
   // assigns the neurons to the layers
   // syntax: NETWORK ASSIGN_NEURON_TO_LAYER layer_id local_neuron_id global_neuron_id
   else if(strcmp(s,"ASSIGN_NEURON_TO_LAYER")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int layer_id=(int)(tmp);
    if(tmp!=layer_id){
     printf("layer index must be an integer!\n");
     exit(0);
    }
    if(layer_id<0){
     printf("layer index must be positive!\n");
     exit(0);
    }
    if(layer_id>(NETWORK.num_of_layers-1)){
     printf("layer index out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int local_neuron_id=(int)(tmp);
    if(tmp!=local_neuron_id){
     printf("local neuron index must be an integer!\n");
     exit(0);
    }
    if(local_neuron_id<0){
     printf("local neuron index must be positive!\n");
     exit(0);
    }
    if(local_neuron_id>(NETWORK.num_of_neurons[layer_id]-1)){
     printf("local neuron index out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int global_neuron_id=(int)(tmp);
    if(tmp!=global_neuron_id){
     printf("global neuron index must be an integer!\n");
     exit(0);
    }
    if(global_neuron_id<0){
     printf("global neuron index must be positive!\n");
     exit(0);
    }
    if(global_neuron_id>(NNUM-1)){
     printf("global neuron index out of range!\n");
     exit(0);
    }
    printf("NETWORK ASSIGN_NEURON_TO_LAYER %d %d %d\n [OK]",layer_id,local_neuron_id,global_neuron_id);
    NETWORK.neuron_id[layer_id][local_neuron_id]=global_neuron_id;
   } else {
    printf("the specified network feature is unknown!\n");
    exit(0);
   }
  }
  // specify the number of training points
  // syntax: NUMBER_OF_TRAINING_POINTS num
  else if(strcmp(s,"NUMBER_OF_TRAINING_POINTS")==0){
   ret=fscanf(fp,"%lf",&tmp);
   int ndata=(int)(tmp);
   if(tmp!=ndata){
    printf("NUMBER_OF_TRAINING_POINTS must be an integer!\n");
    exit(0);
   }
   if(ndata<0){
    printf("NUMBER_OF_TRAINING_POINTS must be positive!\n");
    exit(0);
   }
   if(ndata>MAX_TRAINING_POINTS){
    printf("NUMBER_OF_TRAINING_POINTS is too large!\n");
    printf("please increase MAX_TRAINING_POINTS and recompile!\n");
    exit(0);
   }
   printf("NUMBER_OF_TRAINING_POINTS = %d [OK]\n",ndata);
   NDATA=ndata;
  }
  // specify the training points for supervised learning
  // syntax: TRAINING_POINT IN/OUT index value
  else if(strcmp(s,"TRAINING_POINT")==0){
   ret=fscanf(fp,"%s",s);
   if(strcmp(s,"IN")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int ind=(int)(tmp);
    if(ind!=tmp){
     printf("the training data index must be an integer!\n");
     exit(0);
    }
    if(ind<0){
     printf("the training data index must be positive!\n");
     exit(0);
    }
    if(ind>NDATA){
     printf("training data index out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    printf("TRAINING_POINT IN %d %f [OK]\n",ind,tmp);
    X[ind]=tmp;
   }
   else if(strcmp(s,"OUT")==0){
    ret=fscanf(fp,"%lf",&tmp);
    int ind=(int)(tmp);
    if(ind!=tmp){
     printf("the training data index must be an integer!\n");
     exit(0);
    }
    if(ind<0){
     printf("the training data index must be positive!\n");
     exit(0);
    }
    if(ind>NDATA){
     printf("training data index out of range!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    printf("TRAINING_POINT OUT %d %f [OK]\n",ind,tmp);
    Y[ind]=tmp;
   } else {
    printf("unknown feature for TRAINING_POINT!\n");
    exit(0);
   }
  }
  // specify the minimum weight in the search space
  // syntax: WEIGHT_MINIMUM value
  else if(strcmp(s,"WEIGHT_MINIMUM")==0){
    ret=fscanf(fp,"%lf",&tmp);
    printf("WEIGHT_MINIMUM = %f [OK]\n",tmp);
    WMIN=tmp;
  }
  // specify the minimum weight in the search space
  // syntax: WEIGHT_MAXIMUM value
  else if(strcmp(s,"WEIGHT_MAXIMUM")==0){
    ret=fscanf(fp,"%lf",&tmp);
    printf("WEIGHT_MAXIMUM = %f [OK]\n",tmp);
    WMAX=tmp;
  }
  // specify the training method
  // syntax: TRAINING_METHOD method values (see below)..
  else if(strcmp(s,"TRAINING_METHOD")==0){
   ret=fscanf(fp,"%s",s);
   // simulated annealing
   // syntax: verbosity mmax nmax kbtmin kbtmax accuracy
   // where
   // verbosity = ON/OFF
   // mmax      = outer loop - number of effective temperature steps
   // nmax      = inner loop - number of test configurations
   // kbtmin    = effective temperature minimum
   // kbtmax    = effective temperature maximum
   // accuracy  = numerical accuracy
   if(strcmp(s,"SIMULATED_ANNEALING")==0){
    ret=fscanf(fp,"%s",s);
    int verbosity;
    if(strcmp(s,"ON")==0){
     verbosity=ON;
    }
    else if(strcmp(s,"OFF")==0){
     verbosity=OFF;
    } else {
     printf("verbosity can be only ON or OFF!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int mmax=(int)(tmp);
    if(mmax!=tmp){
     printf("MMAX must be an integer!\n");
     exit(0);
    }
    if(mmax<0){
     printf("MMAX must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int nmax=(int)(tmp);
    if(nmax!=tmp){
     printf("NMAX must be an integer!\n");
     exit(0);
    }
    if(nmax<0){
     printf("NMAX must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double kbtmin=tmp;
    ret=fscanf(fp,"%lf",&tmp);
    double kbtmax=tmp;
    if(kbtmin>=kbtmax){
     printf("KBTMIN must be smaller then KBTMAX!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double eps=tmp;
    if(eps<=0){
     printf("ACCURACY must be greater than 0!\n");
     exit(0);
    }
    printf("TRAINING METHOD = SIMULATED ANNEALING %d %d %g %g %g [OK]\n",mmax,nmax,kbtmin,kbtmax,eps);
    VERBOSITY=verbosity;
    OPTIMIZATION_METHOD=SIMULATED_ANNEALING;
    MMAX=mmax;
    NMAX=nmax;
    KBTMIN=kbtmin;
    KBTMAX=kbtmax;
    ACCURACY=eps;
   }
   // random search
   // syntax: verbosity nmax accuracy
   // verbosity = ON/OFF
   // nmax      = maximum number of random attempts
   // accuracy  = numerical accuracy
   else if(strcmp(s,"RANDOM_SEARCH")==0){
    ret=fscanf(fp,"%s",s);
    int verbosity;
    if(strcmp(s,"ON")==0){
     verbosity=ON;
    }
    else if(strcmp(s,"OFF")==0){
     verbosity=OFF;
    } else {
     printf("verbosity can be only ON or OFF!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int nmax=(int)(tmp);
    if(nmax!=tmp){
     printf("NMAX must be an integer!\n");
     exit(0);
    }
    if(nmax<0){
     printf("NMAX must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double eps=tmp;
    if(eps<=0){
     printf("ACCURACY must be greater than 0!\n");
     exit(0);
    }
    printf("OPTIMIZATION METHOD = RANDOM SEARCH %d %g [OK]\n",nmax,eps);
    VERBOSITY=verbosity;
    OPTIMIZATION_METHOD=RANDOM_SEARCH;
    NMAX=nmax;
    ACCURACY=eps;
   }
   // gradient descent
   // syntax: verbosity nxw maxiter gamma accuracy
   // where
   // verbosity = ON/OFF
   // nxw       = number of cells in one direction of the weight space
   // maxiter   = maximum number of iterations
   // gamma     = step size
   // accuracy  = numerical accuracy
   else if(strcmp(s,"GRADIENT_DESCENT")==0){
    ret=fscanf(fp,"%s",s);
    int verbosity;
    if(strcmp(s,"ON")==0){
     verbosity=ON;
    }
    else if(strcmp(s,"OFF")==0){
     verbosity=OFF;
    } else {
     printf("verbosity can be only ON or OFF!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int nxw=(int)(tmp);
    if(nxw!=tmp){
     printf("NXW must be an integer!\n");
     exit(0);
    }
    if(nxw<0){
     printf("NXW must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int maxiter=(int)(tmp);
    if(maxiter!=tmp){
     printf("MAXITER must be an integer!\n");
     exit(0);
    }
    if(maxiter<0){
     printf("MAXITER must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double gamma=tmp;
    if(gamma<=0){
     printf("GAMMA must be greater than 0!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double eps=tmp;
    if(eps<=0){
     printf("ACCURACY must be greater than 0!\n");
     exit(0);
    }
    printf("OPTIMIZATION METHOD = GRADIENT DESCENT %d %d %g %g [OK]\n",nxw,maxiter,gamma,eps);
    VERBOSITY=verbosity;
    NXW=nxw;
    MAXITER=maxiter;
    GAMMA=gamma;
    ACCURACY=eps;
   }
   // genetic algorithm
   // syntax: verbosity nmax npop rate accuracy
   // where:
   // verbosity = ON/OFF
   // nmax      = number of generations
   // npop      = number of individuals per generation
   // rate      = rate of change between one generation and the parent
   // accuracy  = numerical accuracy
   else if(strcmp(s,"GENETIC_ALGORITHM")==0){
    ret=fscanf(fp,"%s",s);
    int verbosity;
    if(strcmp(s,"ON")==0){
     verbosity=ON;
    }
    else if(strcmp(s,"OFF")==0){
     verbosity=OFF;
    } else {
     printf("verbosity can be only ON or OFF!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int nmax=(int)(tmp);
    if(nmax!=tmp){
     printf("NMAX must be an integer!\n");
     exit(0);
    }
    if(nmax<0){
     printf("NMAX must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    int npop=(int)(tmp);
    if(npop!=tmp){
     printf("NPOP must be an integer!\n");
     exit(0);
    }
    if(npop<0){
     printf("NPOP must be positive!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double rate=tmp;
    if(rate<=0){
     printf("RATE must be greater than 0!\n");
     exit(0);
    }
    ret=fscanf(fp,"%lf",&tmp);
    double eps=tmp;
    if(eps<=0){
     printf("ACCURACY must be greater than 0!\n");
     exit(0);
    }
    printf("OPTIMIZATION METHOD = GENETIC ALGORITHM %d %d %g %g [OK]\n",nmax,npop,rate,eps);
    VERBOSITY=verbosity;
    NMAX=nmax;
    NPOP=npop;
    RATE=rate;
    ACCURACY=eps;
   } else {
    printf("unknown training method!\n");
    exit(0);
   }
  }
  // specify if some output has to be saved
  // syntax: SAVE_OUTPUT ON/OFF
  else if(strcmp(s,"SAVE_OUTPUT")==0){
   ret=fscanf(fp,"%s",s);
   if(strcmp(s,"ON")==0){
    SAVE_OUTPUT=ON;
    printf("SAVE_OUTPUT ON [OK]\n");
   }
   else if(strcmp(s,"OFF")==0){
    SAVE_OUTPUT=OFF;
    printf("SAVE_OUTPUT OFF [OK]\n");
   } else {
    printf("SAVE_OUTPUT can be only ON or OFF!\n");
    exit(0);
   }
  }
  // specify the output file name
  // syntax: OUTPUT_FILE_NAME filename
  else if(strcmp(s,"OUTPUT_FILE_NAME")==0){
   ret=fscanf(fp,"%s",OUTPUT_FILENAME);
   printf("OUTPUT FILE NAME = %s [OK]\n",OUTPUT_FILENAME);
  }
  // specify the number of points for the output file
  // syntax: NUMBER_OF_POINTS num
  else if(strcmp(s,"NUMBER_OF_POINTS")==0){
   ret=fscanf(fp,"%lf",&tmp);
   int num=(int)(tmp);
   if(tmp!=num){
    printf("NUMBER_OF_POINTS must be an integer!\n");
    exit(0);
   }
   if(num<0){
    printf("NUMBER_OF_POINTS must be positive!\n");
    exit(0);
   }
   if(num>MAX_NUM_POINTS){
    printf("NUMBER_OF_POINTS is too large!\n");
    printf("please increase MAX_NUM_POINTS and recompile!\n");
    exit(0);
   }
   printf("NUMBER OF POINTS = %d [OK]\n",num);
   NUMBER_OF_POINTS=num;
  }
  // specify the input points for the output file
  // syntax: INPUT_POINT ind val
  else if(strcmp(s,"INPUT_POINT")==0){
   ret=fscanf(fp,"%lf",&tmp);
   int num=(int)(tmp);
   if(tmp!=num){
    printf("INPUT_POINT index must be an integer!\n");
    exit(0);
   }
   if(num<0){
    printf("INPUT_POINT index must be positive!\n");
    exit(0);
   }
   if(num>(NUMBER_OF_POINTS-1)){
    printf("INPUT_POINT index out of range!\n");
    exit(0);
   }
   ret=fscanf(fp,"%lf",&tmp);
   printf("INPUT POINT #%d = %g [OK]\n",num,tmp);
   OUTPUT_X[num]=tmp;
  }
  // save a neural network (structure and weights) in the file network.dat
  // at the end of the training process
  // syntax: SAVE_NEURAL_NETWORK
  else if(strcmp(s,"SAVE_NEURAL_NETWORK")==0){
   SAVE_NEURAL_NETWORK=ON;
   printf("SAVE NEURAL NETWORK [OK]\n");
  }
  // load a neural network (structure and weights) from the file network.dat
  // at the begining of the training process
  // syntax: LOAD_NEURAL_NETWORK
  else if(strcmp(s,"LOAD_NEURAL_NETWORK")==0){
   SAVE_NEURAL_NETWORK=ON;
   printf("LOAD NEURAL NETWORK [OK]\n");
  }
 }while(!feof(fp));
}
