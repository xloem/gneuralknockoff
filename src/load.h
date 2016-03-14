void load(int output /* screen output - on/off */){
 // load a network that has been previously saved
 int i,j;
 int ret;
 double tmp;
 FILE *fp;

 fp=fopen("network.dat","r");
 if(fp==NULL){
  printf("cannot open file network.dat!\nstop.");
  exit(0);
 }

 // saves the description of every single neuron
 ret=fscanf(fp,"%lf\n",&tmp); // total number of neurons
 NNUM=(int)(tmp);
 for(i=0;i<NNUM;i++){
  ret=fscanf(fp,"%lf\n",&tmp); // neuron index - useless
  ret=fscanf(fp,"%lf\n",&tmp); // number of input connections (weights)
  NEURON[i].nw=(int)(tmp);
  for(j=0;j<NEURON[i].nw;j++){
   ret=fscanf(fp,"%lf\n",&tmp);
   NEURON[i].w[j]=tmp; // weights
  }
  for(j=0;j<NEURON[i].nw;j++){
   ret=fscanf(fp,"%lf\n",&tmp); // connections to other neurons
   NEURON[i].connection[j]=(int)(tmp);
  }
  ret=fscanf(fp,"%lf\n",&tmp); // activation function
  NEURON[i].activation=(int)(tmp);
  ret=fscanf(fp,"%lf\n",&tmp); // discriminant function
  NEURON[i].discriminant=(int)(tmp);
 }

 // saves the network topology
 ret=fscanf(fp,"%lf\n",&tmp); // total number of layers (including input and output layers)
 NETWORK.num_of_layers=(int)(tmp);
 for(i=0;i<NETWORK.num_of_layers;i++){
  ret=fscanf(fp,"%lf\n",&tmp); // total number of neurons in the i-th layer
  NETWORK.num_of_neurons[i]=(int)(tmp);
  for(j=0;j<NETWORK.num_of_neurons[i];j++){
   ret=fscanf(fp,"%lf\n",&tmp); // global id neuron of every neuron in the i-th layer
   NETWORK.neuron_id[i][j]=(int)(tmp);
  }
 }

 fclose(fp);

 // screen output
 if(output==ON){
  printf("load(NETWORK);\n");
  // neuron descriptions
  printf("\nNEURONS\n=======\n");
  printf("NNUM = %d\n",NNUM); // total number of neurons
  for(i=0;i<NNUM;i++){
   printf("\n=======\n");
   printf("NEURON[%d].nw = %d\n",i,NEURON[i].nw); // number of input connections (weights)
   for(j=0;j<NEURON[i].nw;j++) printf("NEURON[%d].w[%d] = %g\n",i,j,NEURON[i].w[j]); // weights
   for(j=0;j<NEURON[i].nw;j++) printf("NEURON[%d].connection[%d] = %d\n",i,j,NEURON[i].connection[j]); // connections to other neurons
   printf("NEURON[%d].activation = %d\n",i,NEURON[i].activation); // activation function
   printf("NEURON[%d].discriminant = %d\n",i,NEURON[i].discriminant); // discriminant function
   printf("=======\n");
  }
  // network topology
  printf("\nNETWORK\n=======\n");
  printf("NETWORK.num_of_layers = %d\n",NETWORK.num_of_layers); // total number of layers (including input and output layers)
  for(i=0;i<NETWORK.num_of_layers;i++){
   printf("NETWORK.num_of_neurons[%d] = %d\n",i,NETWORK.num_of_neurons[i]); // total number of neurons in the i-th layer
   for(j=0;j<NETWORK.num_of_neurons[i];j++)
    printf("NETWORK.neuron_id[%d][%d] = %d\n",i,j,NETWORK.neuron_id[i][j]); // global id neuron of every neuron in the i-th layer
  }
  printf("\n");
 }
}
