
#include "parser.h"
#include "save.h"

int main(int argc, char** argv){
    struct nnet newt;                   struct slidingbuffer bf;
    char filename[256]; filename[0] = 0;
    if (argc == 0) {fprintf(stderr, "Your system does not handle command line arguments in a way that this program understands.\n"); exit(1);}
    if (argc > 2) fprintf(stderr, "%s does not process more than one gnetwork file in a single invocation.\n", argv[0]);
    if (argc != 2) {fprintf(stderr, "Usage:  %s [filename] where 'filename.gnw' is the name of a gnetwork file.\n", argv[0]); exit(1);}
    bzero(&newt, sizeof(struct nnet));  bzero(&bf, sizeof(struct slidingbuffer));
    strncpy(filename, argv[1], 250); strncat(filename, ".gnw", 255);
    if (NULL == (bf.input = fopen(filename, "r"))) {printf("unable to open %s\n", filename); exit(1);}
    nnetparser( &newt, &bf);
    fflush(stdout);
    PrintWarnings( &bf );
    printf("\n\nParse successful.");
    printf("\n%d nodes created. Node 0 is a bias node.  ", newt.nodecount);
    if (newt.inputcount > 0) printf("Nodes {1 %d} are input nodes.  ", newt.inputcount-1);
    if (newt.nodecount - newt.inputcount - newt.outputcount > 0)
	printf(" Nodes {%d %d} are hidden nodes.", newt.inputcount, (newt.nodecount-1) - newt.outputcount);
    if (newt.outputcount > 0) printf(" Nodes {%d %d} are output nodes. ", newt.nodecount - newt.outputcount, (newt.nodecount-1));
    printf("\n%d Connections created. ", newt.synapsecount);
    for (int syn = 0; syn < newt.synapsecount; syn++) printf("%d=%d->%d,",syn,newt.sources[syn],newt.dests[syn]); printf("\b \n");
    fclose(bf.input); bf.input = NULL;
    strcpy(&(filename[strlen(filename)-3]), "out");
    FILE *outf = fopen(filename, "w");
    if (outf == NULL) {printf("unable to open %s", filename);exit(1);}
    debugnnet(&newt);
    nnetwriter( &newt, outf);
    fclose(outf);
}
