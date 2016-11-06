#include "defines.h"
#include "parser.h"
#include "save.h"

#define HELPSTRING  "usage: nnet <filename> | nnet -v | nnet -h | nnet -H | nnet -l \nOptions:\n\
  -h, -?, --help:  print this help and exit.\n\
  -H, --manpage:   print a manual fully describing nnet.\n\
  -v, --version:   version and copyright information.\n"

int HandleOptions(int argc, char **argv){// name, args, NULL, returnval
    static struct option options[] = {
	{"help", no_argument,NULL,'h'},        {"version", no_argument,NULL,'v'},
	{"manpage", no_argument, NULL, 'H'},   {"language", no_argument, NULL, 'l'},
	{"?", no_argument, NULL, '?'},         {0,0,0,0}};
    int opt;
    opterr = 0;
    while ((opt = getopt_long(argc, argv, "hvHl", options, NULL)) != -1)
	switch(opt){
	case 'v': printf("nnet 0.0.1\nCopyright(C) 2016 gneural_network developers\nLicense LGPLv3+. For information about copying, modifying, "
			 "and distribution see <http://gnu.org/licenses/lgpl.html>.\n"); return 0;
	case 'H': printf(MANPAGE);  break;
	case 'l': printf(LANGMAN); break;
	default: /* includes '?' & 'h' by default.*/
	    printf(HELPSTRING);
	    return 0;
	}
    return 1;
}



int main(int argc, char** argv){
    struct nnet newt;       struct conf netconf;            struct slidingbuffer bf;
    char filename[256]; filename[0] = 0;
    if (!HandleOptions(argc, argv)) exit(0);
    if (argc > 2) fprintf(stderr, "%s does not process more than one nnet script in a single invocation.\n", argv[0]);
    if (argc != 2) {fprintf(stderr, "Usage:  %s [filename] where 'filename.nnet' is the name of a nnet script.\n", argv[0]); exit(1);}
    bzero(&newt, sizeof(struct nnet));  bzero(&bf, sizeof(struct slidingbuffer));
    strncpy(filename, argv[1], 250); strncat(filename, ".nnet", 255);
    if (NULL == (bf.input = fopen(filename, "r"))) {fprintf(stderr, "unable to open %s\n", filename); exit(1);}
    if (NULL == (netconf.savename=malloc(strlen(filename+3)))){fprintf(stderr, "allocation failure attempting to save filename.\n"); exit(1);}
    strcpy(netconf.savename,filename);
    nnetparser( &newt, &netconf, &bf);
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
    nnetwriter( &newt, &netconf, outf);
    fclose(outf);
}
