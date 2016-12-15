/* defines.h -- This belongs to gneural_network

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


#ifndef _DEFINE_H
#define _DEFINE_H

#include "includes.h"

#define MAX(x,  y)   (((x) > (y)) ? (x) : (y))
#define MIN(x,  y)   (((x) < (y)) ? (x) : (y))

// maximum number of training cases
#define MAX_TRAINING_CASES 8

// maximum allowed number of input connections per neuron
#define MAX_IN 16

// maximum total number of neurons
#define MAX_NUM_NEURONS 32

// maximum number of layers
#define MAX_NUM_LAYERS 16

// maximum number of cases in output file
#define MAX_NUM_CASES 64

// low limit for the genetic algorithm
#define PAR_QSORT_LOW_LIMIT 1024

// specifically for datafiles, weights, and activations. We want to be able to compile correctly for different size floats, on account of OMP and GPU
// restrictions.
typedef double flotype;

// FLOFMT is for scripts (replace with %#.15g if all bits are needed; FLOFMT3 is for columnar human-readable (output).
#define FLOFMT " %#6g"
#define FLOFMT3 " %#6.3g"

#define ZERO ((flotype)0.0)
#define ONE  ((flotype)1.0)
#define TWO  ((flotype)2.0)

#define absolute(n)    ((flotype)(fabs((double)(n))))
#define logarithm(n)   ((flotype)(log((double)(n))))
#define cosine(n)      ((flotype)(cos((double)(n))))
#define hypertan(n)    ((flotype)(tanh((double)(n))))
#define arctan(n)      ((flotype)(atan((double)(n))))
#define exponential(n) ((flotype)(exp((double)(n))))

// definition of various internal types
enum switch_flag {
	OFF = (0 == 1),
	ON = (1 == 1),
};

// activation functions
enum activation_function {
	TANH,
	EXP,
	EXP_SIGNED,
	SOFTSIGN,
	RAMP,
	SOFTRAMP,
	ID,
	POL1,
	POL2,
};

// accumulators
enum accumulator_function {
	LINEAR,
	LEGENDRE,
	LAGUERRE,
	FOURIER,
};

// error/cost/target functions
enum error_function {
	MSE,
	ME,
};

// optimization methods
enum optimization_method {
	SIMULATED_ANNEALING,
	RANDOM_SEARCH,
	GRADIENT_DESCENT,
	GENETIC_ALGORITHM,
	MSMCO,
};

// flags for configuration to silence various outputs and warnings. (struct nnet ->flags)
#define SILENCE_BIAS              0x1
#define SILENCE_DEBUG             0x2
#define SILENCE_ECHO              0x4
#define SILENCE_INPUT             0x8
#define SILENCE_OUTPUT           0x10
#define SILENCE_NODEINPUT        0x20
#define SILENCE_NODEOUTPUT       0x40
#define SILENCE_MULTIACTIVATION  0x80
#define SILENCE_RECURRENCE      0x100
#define SILENCE_RENUMBER        0x200
// flags for save configuration  (struct nnet ->flags)
#define SAVE_SERIALIZE          0x400
#define SAVE_DEFAULT            0x800

// flags for datasets  (struct cases ->flags)
#define DATA_IMMEDIATE        0x1
#define DATA_SEEKABLE         0x2
#define DATA_FROMFILE         0x4
#define DATA_FROMDIRECTORY    0x8
#define DATA_FROMPIPE        0x10
#define DATA_TRAINING        0x20
#define DATA_TESTING         0x40
#define DATA_VALIDATION      0x80
#define DATA_DEPLOYMENT     0x100
#define DATA_NOINPUT        0x200
#define DATA_NOOUTPUT       0x400
#define DATA_NOWRITEINPUT   0x800
#define DATA_NOWRITEOUTPUT 0x1000
#define DATA_WRITEPIPE     0x2000
#define DATA_WRITEFILE     0x4000

#define MANPAGE "\n"\
"NNET(1)                         User Manuals                         NNET(1)\n"\
"\n"\
"NAME\n"\
"       nnet - define, train, and deploy feedforward, recurrent, or deep neu‐\n"\
"       ral networks.\n"\
"\n"\
"SYNOPSIS\n"\
"       nnet filename\n"\
"\n"\
"       nnet -v, nnet --version\n"\
"\n"\
"       nnet -h, nnet --help\n"\
"\n"\
"       nnet -l, nnet --language\n"\
"\n"\
"DESCRIPTION\n"\
"       nnet is a utility for general development of  neural  networks.   The\n"\
"       neural  networks  it defines are not limited as to topology or depth,\n"\
"       and many unusual node types such as LSTMs, pooling  nodes,  etc,  are\n"\
"       supported.   When  invoked  with  a filename argument, nnet processes\n"\
"       filename.nnet which must be a nnet script.  See  nnet_language(1)  or\n"\
"       invoke  nnet  -l  for  information  about  the  script language. Nnet\n"\
"       scripts specify the topology, weights,  and  firing  sequence  of  an\n"\
"       artificial neural network.  Optionally they may also specify training\n"\
"       and/or testing instructions and data (or locations  of  data  sources\n"\
"       and destinations) for training, testing, validation, or deployment.\n"\
"\n"\
"       When  finished  processing, or at intervals during training as speci‐\n"\
"       fied by any training instructions in the script,  nnet  writes  file‐\n"\
"       name.out,  a nnet script that specifies the network including changes\n"\
"       made during processing or training.  If filename.out is  saved  while\n"\
"       training  is  not  yet  complete,  it will include a reduced training\n"\
"       schedule corresponding to the amount of training that remains  to  be\n"\
"       done.   If  training  is  interrupted, renaming filename.out to file‐\n"\
"       name.nnet and restarting with nnet filename will resume training from\n"\
"       the most recent save point.\n"\
"\n"\
"       Nnet  scripts  may  be  edited in any text editor.  Users may use the\n"\
"       script language to add new training, testing, or deployment  instruc‐\n"\
"       tions,  to change training parameters or switch to a different train‐\n"\
"       ing method, or to directly modify the network definition  and  topol‐\n"\
"       ogy,  eg  by  changing  the number of input, output, or hidden nodes,\n"\
"       adding layers while building deep networks, adding new connections or\n"\
"       removing existing connections, etc. The edited file may then be saved\n"\
"       as filename.nnet and used to restart nnet for  continued  processing.\n"\
"       Topology  or weight edits will usually make the network less accurate\n"\
"       until retraining has taken place, but whatever training  has  already\n"\
"       been done need not be entirely lost.\n"\
"\n"\
"OPTIONS\n"\
"       -v,--version\n"\
"              Print the current version information of nnet and exit.\n"\
"\n"\
"       -h,--help\n"\
"              Print this man page and exit.\n"\
"\n"\
"       -l,--language\n"\
"              Print the nnet_language man page and exit.\n"\
"\n"\
"REPORTING BUGS\n"\
"       nnet is part of the gneural_network project. You may report any bugs,\n"\
"       or review the bug database, at\n"\
"\n"\
"       <https://savannah.gnu.org/bugs/?group=gneuralnetwork>.\n"\
"\n"\
"       nnet contains numerous  self-diagnostic  and  constraint  checks  for\n"\
"       things  which  will  never  happen unless the code contains an unsus‐\n"\
"       pected bug.  Please notify us immediately if it ever exits printing a\n"\
"       diagnostic  message starting with the words 'Program Error.' Any such\n"\
"       message would be reporting a bug in nnet itself, not a  problem  with\n"\
"       your script.\n"\
"\n"\
"AUTHOR\n"\
"       Man page written by Ray Dillinger <bear at sonic dot net>\n"\
"\n"\
"COPYRIGHT\n"\
"       nnet  and  its  documentation  are  Copyright (C) 2016 Ray Dillinger,\n"\
"       Jean-Michel Sellier, and the Gneural_Network project.\n"\
"\n"\
"       Permission is granted to modify and distribute the software  and  its\n"\
"       sources  subject to the conditions of the GNU Public License, version\n"\
"       3 or later.  Permission is granted to distribute  the  documentation,\n"\
"       including  this  man  page, subject to the conditions of the GNU Free\n"\
"       Documentation License, version 1.3 or later.\n"\
"\n"\
"SEE ALSO\n"\
"       nnet_language(1)\n"\
"\n"\
"Linux                           NOVEMBER 2016                        NNET(1)\n"

#define LANGMAN ""\
"NNET_LANGUAGE(1)                User Manuals               NNET_LANGUAGE(1)\n"\
"\n"\
"NAME\n"\
"       nnet_language - neural net scripting language reference manual\n"\
"\n"\
"SYNOPSIS\n"\
"       nnet filename\n"\
"\n"\
"DESCRIPTION\n"\
"       nnet  is  a  utility for general development of neural networks.  It\n"\
"       reads and writes files written in the  nnet  script  language.   See\n"\
"       nnet(1) for instructions on using the executable.\n"\
"\n"\
"       Nnet  scripts  completely  describe neural networks, along with such\n"\
"       ancillary information as needed to train,  test,  and  deploy  them.\n"\
"       nnet  reads  the  scripts, builds the neural networks, does whatever\n"\
"       processing or training or deployment  the  script  calls  for,  then\n"\
"       writes  a  script which describes the the network as changed by that\n"\
"       processing.  The developer can edit nnet script in  a  text  editor,\n"\
"       making changes to the network, adding a new training plan, directing\n"\
"       it to access new sources of data, etc, and then run  nnet  again  on\n"\
"       the modified script, enabling progressive staged development or mid‐\n"\
"       stream alterations to training parameters.\n"\
"\n"\
"   Comments and whitespace:\n"\
"       NNet script is whitespace insensitive.  You can  insert  tabs,  com‐\n"\
"       ments,  line  breaks,  or  spaces  anywhere  except in the middle of\n"\
"       tokens, without altering the meaning of the script.\n"\
"\n"\
"       A nnet script is divided into sections of several types.  These sec‐\n"\
"       tions are:\n"\
"\n"\
"       Initial Comment\n"\
"\n"\
"       Nnet Configuration\n"\
"\n"\
"       Node Definition\n"\
"\n"\
"       Connection Definition\n"\
"\n"\
"       Modification\n"\
"\n"\
"       Data\n"\
"\n"\
"       Training\n"\
"\n"\
"       To take them one at a time:\n"\
"\n"\
"   Initial Comment\n"\
"       Comments may occur anywhere in a nnet script file. They begin with a\n"\
"       “#” character and end with a newline.  Initial Comments are comments\n"\
"       at the very start of a script file.  Any non-comment character, even\n"\
"       whitespace characters, terminates Initial Comments. Initial Comments\n"\
"       will  be copied verbatim into the output file. Comments appearing at\n"\
"       other places in the script will not appear in the  generated  output\n"\
"       file.  This allows project notes, including an interpreter ('crunch‐\n"\
"       bang' or 'shebang') line, to be propagated  through  nnet  to  later\n"\
"       versions of the network file.\n"\
"\n"\
"   Nnet Configuration Section\n"\
"       Nnet  Configuration  Sections customize the way nnet works with this\n"\
"       particular script.  They start with the keyword StartConfig and  end\n"\
"       with the keyword EndConfig.  In between there are various statements\n"\
"       that alter the behavior of nnet  while  processing  that  particular\n"\
"       script.\n"\
"\n"\
"       Silence  commands  tell nnet to turn off some of its default output.\n"\
"       Silence may be called with multiple arguments.  Each  argument  must\n"\
"       be one of the following keywords.\n"\
"\n"\
"       Silence(Echo)  suppresses the usual printing of the script to stdout\n"\
"       as it is read.\n"\
"\n"\
"       Silence(Recurrence) suppresses a warning about  a  recurrent  neural\n"\
"       network.\n"\
"\n"\
"       Silence(NoOutput) supresses a warning about a network with no output\n"\
"       nodes.\n"\
"\n"\
"       Silence(NoInput) suppresses a warning about a network with no  input\n"\
"       nodes.\n"\
"\n"\
"       Silence(NodeInput)  and  Silence(NodeOutput) suppress warnings about\n"\
"       nodes that have no incoming or outgoing connections.\n"\
"\n"\
"       Silence(MultiActivation) suppresses  a  warning  about  nodes  whose\n"\
"       activation function will be run more than once in the defined firing\n"\
"       sequence.  This happens  when  processing  connections  in  sequence\n"\
"       means  that  propagating signals to and from a node will switch more\n"\
"       than once in the firing sequence.\n"\
"\n"\
"       Save commands change the way the script output is saved.  nnet  nor‐\n"\
"       mally  writes  filename.out  once  immediately  after  loading file‐\n"\
"       name.nnet and performing any simple modifications.  Then if perform‐\n"\
"       ing  modifications  that  take a significant amount of time, such as\n"\
"       training, nnet writes filename.out again when those actions are fin‐\n"\
"       ished,  overwriting the previous save.  Save may be called with mul‐\n"\
"       tiple arguments.\n"\
"\n"\
"       Save(“String”) will change the save  filename.   The  file  will  be\n"\
"       saved as String instead of as filename.out.\n"\
"\n"\
"       Save(Serialize)  Is  a command to insert a serial number in the name\n"\
"       of each savefile, which avoids overwriting  the  earlier  savefiles.\n"\
"       If  the script being run has a serial number in its name, the serial\n"\
"       numbers in the output file names will start from that  number.   For\n"\
"       example  if  you  ran  nnet  foobar.20  to process a file named foo‐\n"\
"       bar.20.nnet, and there was a Save(Serialize) command in  that  file,\n"\
"       then  the  output  file will be named foobar.21.out rather than foo‐\n"\
"       bar.20.out.  If an explicit output filename was given along  with  a\n"\
"       Serialize  argument,  the  last  '.'  in  the  savefile name will be\n"\
"       changed into a number between two '.' characters.   For  example  if\n"\
"       you  ran  nnet  foobar.20 to process a file named foobar.20.nnet and\n"\
"       there was a command Save(Serialize “foobar.nnet”)  the  output  file\n"\
"       would be named foobar.21.nnet rather than foobar.20.out.  If a spec‐\n"\
"       ified savefilename has no '.' character in it, then a '.'  character\n"\
"       and the serial number will be appended at the end.\n"\
"\n"\
"       Save(n)  where n is an integer causes up to n periodic saves to take\n"\
"       place during training.  Each save will take place at the end  of  an\n"\
"       epoch  of  training.   If n evenly divides the number of epochs then\n"\
"       the intervals between saves will be equal.  Otherwise they'll be  as\n"\
"       close  to equal as possible.  If n is less than the number of epochs\n"\
"       of training then one save will be made after each epoch.   If  given\n"\
"       along  with  a Serialize keyword argument, each save will have a new\n"\
"       incremental serial number.\n"\
"\n"\
"   Node Definition Section\n"\
"       Node Definition Sections define nodes.  They start with the  keyword\n"\
"       StartNodes and end with EndNodes.  In between there are CreateInput,\n"\
"       CreateHidden, and CreateOutput statements. Each node gets an ID num‐\n"\
"       ber  when  it is created.  You need to know how these ID numbers are\n"\
"       assigned in order to make connections.  They are assigned in ascend‐\n"\
"       ing  sequence,  starting with 1, as the Node-creating statements are\n"\
"       processed.  But there is an exception to this  sequence  rule.   All\n"\
"       input  nodes have lower ID numbers than all non-input nodes, and all\n"\
"       output nodes have ID numbers higher than all non-output  nodes.   So\n"\
"       if  a  CreateInput  statement is processed after a different kind of\n"\
"       node-creating statement, it can force the ID numbers of  hidden  and\n"\
"       output  nodes which already exist to be changed to make room for the\n"\
"       new nodes.  The same thing happens if a  CreateHidden  statement  is\n"\
"       processed  after  a  CreateOutput  statement.  If the ID number of a\n"\
"       node changes as a result of defining additional nodes,  any  connec‐\n"\
"       tions  already defined using the earlier ID number are not affected.\n"\
"       This can be confusing because different ID numbers then refer to the\n"\
"       same node in different connection statements.  It's usually clearest\n"\
"       to avoid renumbering entirely by always defining input  nodes  first\n"\
"       and  output  nodes  last,  and defining all nodes before all connec‐\n"\
"       tions.\n"\
"\n"\
"       CreateInput statements create Input nodes.  Input nodes are  differ‐\n"\
"       ent from other nodes in that an input value is added its signal, and\n"\
"       its activation function is run, before the  connection  sequence  is\n"\
"       run.  CreateOutput statements create Output nodes.  Output nodes are\n"\
"       different from other nodes in that, when the connection sequence has\n"\
"       been run, any output nodes with nonzero signal have their activation\n"\
"       function run.  The activation of the output nodes is copied  to  the\n"\
"       network  output.  CreateHidden statements create hidden nodes.  Sig‐\n"\
"       nals arrive at nodes when connections having those nodes as destina‐\n"\
"       tion are processed, and are accumulated into the node's signal level\n"\
"       using the node's accumulator function.  Whenever a connection having\n"\
"       a  given  node as source is processed, if that node has nonzero sig‐\n"\
"       nal, its activation function is run, establishing a  new  activation\n"\
"       value  for  that function.  This sets the signal level back to zero.\n"\
"       All this applies equally to Input and Output nodes, but in a typical\n"\
"       feedforward  network input nodes are not the destination of any con‐\n"\
"       nections and output nodes are not the source of any connections.\n"\
"\n"\
"       Every node-creating statement takes three arguments; the  number  of\n"\
"       nodes  to create, the name of the accumulator function, and the name\n"\
"       of the activation function.\n"\
"\n"\
"       For example here is the Node Definition Section of the  XOR  network\n"\
"       (the  smallest  network  of  conventional nodes capable of computing\n"\
"       XOR):\n"\
"\n"\
"            StartNodes\n"\
"                 CreateInput(2, None, Identity)\n"\
"                 CreateHidden(1, Add, StepFunction)\n"\
"                 CreateOutput(1, Add, StepFunction)\n"\
"            EndNodes\n"\
"\n"\
"   Connection Definition Section\n"\
"       Connection definition sections start with the  keyword  StartConnec‐\n"\
"       tions  and  end  with  the keyword EndConnections.  In between are a\n"\
"       series of Connect statements.\n"\
"\n"\
"       Connect statements have three arguments: the source node  or  nodes,\n"\
"       the  destination  node  or  nodes, and the weight or weights of each\n"\
"       connection.  Each node may be an ID number, or an integer span of ID\n"\
"       numbers.   The  weights  may be a real number, an array of real num‐\n"\
"       bers, or (when initializing) the keyword 'Randomize.'   For  example\n"\
"       here  is  a  way to write the connection definition statement of the\n"\
"       XOR network:\n"\
"\n"\
"            StartConnections\n"\
"                 Connect(0 {3 4} -1.0)\n"\
"                 Connect({1 2} {3 4} 1.0)\n"\
"                 Connect(3 4 -2.0)\n"\
"            EndConnections\n"\
"\n"\
"       The first Connect statement defines bias connections to  the  hidden\n"\
"       node  (3)  and the output node (4). Both connections have weight -1.\n"\
"       The second connects both of the input nodes to the hidden  node  and\n"\
"       the  output  node  (4),  also with weight 1.  And the last defines a\n"\
"       connection from the hidden node to the output node, with weight -2.\n"\
"\n"\
"       The same network can also be written this way:\n"\
"\n"\
"            StartConnections\n"\
"                 Connect({0 2} {3 4} [-1.0 -1.0\n"\
"                                       1.0  1.0\n"\
"                                       1.0  1.0])\n"\
"                 Connect(3 4 -2.0)\n"\
"            EndConnections\n"\
"\n"\
"       Here the first two statements have been combined into one, using  an\n"\
"       array  of  weights instead of two different shared weights.  It con‐\n"\
"       nects the bias node and both the input nodes to the hidden node  and\n"\
"       the  output  node  - and gives the six weights as an array, with one\n"\
"       source per line and one destination per column.\n"\
"\n"\
"       It could also be written with the whole array on  one  line;  white‐\n"\
"       space is not significant.\n"\
"\n"\
"       Whenever a different sequence of connection processing within a Con‐\n"\
"       nect statement could give different results (for example in  a  non‐\n"\
"       spiking  recurrent  network when the sources and destinations in the\n"\
"       same 'connect' statement overlap) this sequence of  connection  pro‐\n"\
"       cessing  within  connect statements that define multiple connections\n"\
"       (in ascending sequence by source) is considered canonical; the  net‐\n"\
"       work  must produce results as if this were the actual order in which\n"\
"       the connections were processed.\n"\
"\n"\
"       The sequence in which connections are processed  is  otherwise  con‐\n"\
"       strained by the sequence of the Connect statements defining them.\n"\
"\n"\
"       Connection Semantics:\n"\
"\n"\
"       A  node  has  two  values significant for connection processing: its\n"\
"       signal and its activation.  Whenever its activation function is run,\n"\
"       its  signal  is  used  as  an argument to its activation function to\n"\
"       determine its activation, and the signal is reset to zero.\n"\
"\n"\
"       In spiking networks, the activation function is run the  first  time\n"\
"       in the firing sequence a node is used as the source of a connection,\n"\
"       and not run again during the same firing  sequence.   In  nonspiking\n"\
"       networks,  each  time  a connection is processed, the source node is\n"\
"       checked to see if it has nonzero signal; if so, the activation func‐\n"\
"       tion  is  run and the new activation is propagated by the connection\n"\
"       (the product of the weight and the activation is added to the signal\n"\
"       of  the  destination node).  If the source node has zero signal, the\n"\
"       activation function is not run, and the existing activation is prop‐\n"\
"       agated  by  the connection.  Both signal and activation are initial‐\n"\
"       ized to zero when the network is started.\n"\
"\n"\
"       For purposes of this determination a node's signal is nonzero  when‐\n"\
"       ever  connections  leading  to it have been processed since the last\n"\
"       time its activation function was run, even if the  sum  of  all  the\n"\
"       propagated values is in fact zero.\n"\
"\n"\
"   Modification Sections:\n"\
"       nnet writes back script files as output in order to facilitate modi‐\n"\
"       fying networks by editing, and it's reasonable to add a node defini‐\n"\
"       tion  or  connection  definition section to the end of a script, but\n"\
"       otherwise  modifying  scripts  by  altering  definition   statements\n"\
"       directly  is  prone  to  error because it's hard to keep connections\n"\
"       consistent when the number (and ID numbers) of nodes are changed  by\n"\
"       the modification.  Additional utilities for network modification may\n"\
"       be added to the end of a script in a modification section, and nodes\n"\
"       and  connections  already existing will be kept synchronized through\n"\
"       all changes made by the alterations.\n"\
"\n"\
"       Modification sections start with the keyword StartMods and end  with\n"\
"       EndMods.   They  contain  Delnodes, Dupenodes, and Disconnect state‐\n"\
"       ments.  nnet will execute the  modifications  immediately  on  load,\n"\
"       creating  an  output  file  that describes the new network structure\n"\
"       directly.  Each of these instructions inflicts some degree of 'brain\n"\
"       damage'  on  trained  networks,  but  may be steps that make further\n"\
"       training or wider application possible or faster.\n"\
"\n"\
"       Delnodes takes an ID number or ID number span  as  an  argument  and\n"\
"       deletes  the  nodes having those ID numbers.  All connections to and\n"\
"       from those nodes are also deleted.  Other connections  will  not  be\n"\
"       changed,  although  the ID numbers of all nodes higher-numbered than\n"\
"       the nodes deleted will decrease.\n"\
"\n"\
"       Disconnect statements take two arguments. Each argument may be an ID\n"\
"       number  or an ID number span.  Their effect is to delete all connec‐\n"\
"       tions from the node or nodes in the first argument to  the  node  or\n"\
"       nodes in the second argument.\n"\
"\n"\
"       Dupenodes takes an integer and an ID number, and creates that number\n"\
"       of near-copies of the node that has that ID number.  The  new  nodes\n"\
"       have  ID  numbers immediately following that of the duplicated node.\n"\
"       All higher numbered nodes will get increased ID  numbers.   The  new\n"\
"       nodes  are  connected  as though all connect statements touching the\n"\
"       original had been ID spans including the new  nodes,  and  they  are\n"\
"       initialized  with  very small randomized weights proportional to the\n"\
"       weights of the connections to the original node.\n"\
"\n"\
"       Input and output nodes may be deleted or duplicated the same as  any\n"\
"       other  node, and this will change the number of inputs taken or out‐\n"\
"       puts produced respectively. Matching changes in the 'data'  sections\n"\
"       will to be needed.\n"\
"\n"\
"   Data Sections\n"\
"       Data  sections  define data (or data sources) for training, testing,\n"\
"       validation, and deployment.  They also define  a  filename  (or  the\n"\
"       name  of a named pipe) to which results are to be written in deploy‐\n"\
"       ment.  They begin with the keyword StartData and end  with  EndData,\n"\
"       and in between contain Data statements.\n"\
"\n"\
"       The Syntax of a Data statement is\n"\
"             Data(<data_source> <uses> <flags> <output_dest> <data>)\n"\
"\n"\
"       <data_source>  starts  with  one of the keywords 'Immediate', 'From‐\n"\
"       File', 'FromDirectory', or 'FromPipe'.  Any of the last  three  must\n"\
"       be  followed  by  a  string  containing the filename, named pipe, or\n"\
"       directory name to open and read from,  and  also  means  the  <data>\n"\
"       argument  may  not  be  present.  'Immediate' means nnet should read\n"\
"       cases inline directly from the <data> argument.\n"\
"\n"\
"       All types of data source must give cases in the same format  as  the\n"\
"       <data>  argument  described  below. 'FromDirectory' means that every\n"\
"       file in that directory is to be opened and read as a data source.\n"\
"\n"\
"       <use> is one or more of the keywords 'Training', 'Testing', 'Valida‐\n"\
"       tion',  or  'Deployment',  and  describe  what  operations this data\n"\
"       source is to be used for.   The keywords may come in any sequence.\n"\
"\n"\
"       <flags> if present are zero or one  of  the  keywords  'NoInput'  or\n"\
"       'NoOutput'  which  serve  to  notify  nnet that the cases it will be\n"\
"       reading from this source do not include one or the other,  and  zero\n"\
"       or one of the keywords 'WriteNoInput' or 'WriteNoOutput' which serve\n"\
"       to notify nnet that Output written to a  file  or  pipe  should  not\n"\
"       include one or the other.  Flag keywords may come in any sequence.\n"\
"\n"\
"       <output_dest> may be skipped, unless 'Deployment' has been specified\n"\
"       among the <use> arguments or 'WriteNoInput' or 'WriteNoOutput'  have\n"\
"       been  specified  among the flags. If present, <output_dest> consists\n"\
"       of either the keyword 'ToFile' or the keyword 'ToPipe', followed  by\n"\
"       the  file name or pipe name.  nnet will open the file in append mode\n"\
"       (existing file contents will not be overwritten)  or  open  a  named\n"\
"       pipe,  and write its output to that location as a sequence of cases,\n"\
"       in the same text format as the <data> segment.\n"\
"\n"\
"       <data> if present is one or more cases.  If 'ReadNoInput' or  'Read‐\n"\
"       NoOutput' are specified, each case is a single sequence.  Otherwise,\n"\
"       each case is a double sequence.  Single Sequences are an open square\n"\
"       bracket,  a  sequence of real numbers, and a closing square bracket.\n"\
"       Double Sequences are an open square bracket, a Single  Sequence  for\n"\
"       Input,  a  Single Sequence for output, and a closing square bracket.\n"\
"       The number of values in each Single Sequence  must  match  the  net‐\n"\
"       work's number of inputs or number of outputs, respectively.\n"\
"\n"\
"       Here are some examples of correct Data Statements:\n"\
"          Data(Immediate Training Testing Validation\n"\
"              [[0.0 0.0] [0.0]]\n"\
"              [[0.0 1.0] [1.0]]\n"\
"              [[1.0 0.0] [1.0]]\n"\
"              [[1.0 1.0] [0.0]] )\n"\
"       Gives  the  four  cases  of the classic XOR problem.  This same data\n"\
"       will be used whether the script says to train,  test,  or  validate.\n"\
"       The output produced by the network will not be written anywhere, but\n"\
"       the testing summary (accuracy) will be written on  stdout.   If  the\n"\
"       developer wanted to see the output from testing, he would write\n"\
"             Data(Immediate Training Testing Validation\n"\
"              [[0.0 0.0] [0.0]]\n"\
"              [[0.0 1.0] [1.0]]\n"\
"              [[1.0 0.0] [1.0]]\n"\
"              [[1.0 1.0] [0.0]] ToPipe stdout)\n"\
"       This  is exactly the same as the above except that the actual output\n"\
"       of the network will be written to stdout before the test results are\n"\
"       written on stderr.\n"\
"          Data(FromPipe \"inputstream\" Deployment\n"\
"               ReadNoOutput WriteNoInput ToPipe \"outputstream\")\n"\
"       Says  that  when  Deploying, we will read input (but no output) from\n"\
"       the pipe named inputstream and write output(but not  input)  to  the\n"\
"       pipe named outputstream.\n"\
"          Data(FromFile \"Examples.nnx\" Testing\n"\
"               ToFile \"Results.nnx\")\n"\
"\n"\
"       Directs  nnet,  when testing, to read complete cases (input and out‐\n"\
"       put) from Examples.nnx, then write complete  cases  to  Results.nnx.\n"\
"       The  results  given  in Examples.nnx will be compared to the results\n"\
"       produced by the network for accuracy; the results  produced  by  the\n"\
"       network will be written in cases with the inputs that produced them.\n"\
"       Summary testing results and accuracy will be displayed on stdout.\n"\
"         Data(FromDirectory \"ExampleDir\" Testing)\n"\
"       Directs nnet, when testing, to open every file  in  ExampleDir  read\n"\
"       cases  from it, and not to bother writing the output anywhere.  This\n"\
"       statement just allows the developer to view the testing summary  and\n"\
"       accuracy achieved which will be written on stdout.\n"\
"\n"\
"   Training Sections\n"\
"       define  training,  testing, and/or deployment plans.  nnet will exe‐\n"\
"       cute these plans and create an output file with the network as modi‐\n"\
"       fied  by  the execution.  Any output files created during the execu‐\n"\
"       tion of training or testing will contain  reduced  plans  (the  plan\n"\
"       necessary  to  finish  the  original training plan starting from the\n"\
"       point at which the output file was created). The final  output  file"\
"       will contain no training or testing instructions.\n"\
"\n"\
"       SECTION INCOMPLETE. TBD\n"\
"\n"\
"AUTHOR\n"\
"       Man page written by Ray Dillinger <bear at sonic dot net>\n"\
"\n"\
"COPYRIGHT\n"\
"       nnet  is  Copyright (C) 2016 Ray Dillinger, Jean-Michel Sellier, and\n"\
"       the Gneural_Network project.  Permission is granted  to  modify  and\n"\
"       distribute the software and its sources subject to the conditions of\n"\
"       the GNU Public License, version 3 or later.  Permission  is  granted\n"\
"       to distribute the documentation, including this man page, subject to\n"\
"       the conditions of the GNU Free Documentation License, version 1.3 or\n"\
"       later.\n"\
"\n"\
"REPORTING BUGS\n"\
"       nnet  is  part  of  the  gneural_network  project. You may report or\n"\
"       review bugs  at  https://savannah.gnu.org/bugs/?group=gneuralnetwork\n"\
"       ⟨gneural_network⟩\n"\
"\n"\
"See Also\n"\
"       nnet(1)\n"\
"\n"\
"Linux                             2016-11                  NNET_LANGUAGE(1)\n"

#endif
