void save(int output /* screen output - on/off */){
 // saves all information related to the network
 int i,j;
 FILE *fp;

 fp=fopen("network.dat","w");
 if(fp==NULL){
  printf("cannot save file network.dat!\nstop.");
  exit(0);
 }

 // saves the description of every single neuron
 fprintf(fp,"%d\n",NNUM); // total number of neurons
 for(i=0;i<NNUM;i++){
  fprintf(fp,"%d\n",i); // neuron index
  fprintf(fp,"%d\n",NEURON[i].nw); // number of input connections (weights)
  for(j=0;j<NEURON[i].nw;j++) fprintf(fp,"%g\n",NEURON[i].w[j]); // weights
  for(j=0;j<NEURON[i].nw;j++) fprintf(fp,"%d\n",NEURON[i].connection[j]); // connections to other neurons
  fprintf(fp,"%d\n",NEURON[i].activation); // activation function
  fprintf(fp,"%d\n",NEURON[i].discriminant); // discriminant function
 }

 // saves the network topology
 fprintf(fp,"%d\n",NETWORK.num_of_layers); // total number of layers (including input and output layers)
 for(i=0;i<NETWORK.num_of_layers;i++){
  fprintf(fp,"%d\n",NETWORK.num_of_neurons[i]); // total number of neurons in the i-th layer
  for(j=0;j<NETWORK.num_of_neurons[i];j++) fprintf(fp,"%d\n",NETWORK.neuron_id[i][j]); // global id neuron of every neuron in the i-th layer
 }

 fclose(fp);

 // screen output
 if(output==ON){
  printf("save(NETWORK);\n");
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
