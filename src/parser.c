/* parser.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016 Jean Michel Sellier  <jeanmichel.sellier@gmail.com>
   Copyright (C) 2016 Francesco Maria Virlinzi  <francesco.virlinzi@gmail.com>
   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 3, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// read the input script to define the network and its task

#include "includes.h"
#include "parser.h"
#include "network.h"

enum main_token_id
{
  _COMMENT,
  _NUMBER_OF_NEURONS,
  _NEURON,
  _NETWORK,
  _WEIGHT_MINIMUM,
  _WEIGHT_MAXIMUM,

  _LOAD_NEURAL_NETWORK,
  _SAVE_NEURAL_NETWORK,

  _ERROR_TYPE,
  _INITIAL_WEIGHTS_RANDOMIZATION,

  _NUMBER_OF_TRAINING_CASES,
  _TRAINING_CASE,
  _TRAINING_METHOD,

  _NUMBER_OF_INPUT_CASES,
  _NETWORK_INPUT,

  _SAVE_OUTPUT,
  _OUTPUT_FILE_NAME,
};


static const char *main_token_n[] = {
  [_COMMENT] = "#",
  [_NUMBER_OF_NEURONS] = "NUMBER_OF_NEURONS",
  [_NEURON] = "NEURON",
  [_NETWORK] = "NETWORK",
  [_WEIGHT_MINIMUM] = "WEIGHT_MINIMUM",
  [_WEIGHT_MAXIMUM] = "WEIGHT_MAXIMUM",
  [_LOAD_NEURAL_NETWORK] = "LOAD_NEURAL_NETWORK",
  [_SAVE_NEURAL_NETWORK] = "SAVE_NEURAL_NETWORK",
  [_ERROR_TYPE] = "ERROR_TYPE",
  [_INITIAL_WEIGHTS_RANDOMIZATION] = "INITIAL_WEIGHTS_RANDOMIZATION",
  [_NUMBER_OF_TRAINING_CASES] = "NUMBER_OF_TRAINING_CASES",
  [_TRAINING_CASE] = "TRAINING_CASE",
  [_TRAINING_METHOD] = "TRAINING_METHOD",
  [_NUMBER_OF_INPUT_CASES] = "NUMBER_OF_INPUT_CASES",
  [_NETWORK_INPUT] = "NETWORK_INPUT",
  [_SAVE_OUTPUT] = "SAVE_OUTPUT",
  [_OUTPUT_FILE_NAME] = "OUTPUT_FILE_NAME",
};


const int main_token_count = 17;

enum direction_enum
{
  _IN,
  _OUT,
};

static const char *direction_n[] = {
  [_IN] = "IN",
  [_OUT] = "OUT",
};

const int direction_count = 2;

static const char *error_names[] = {
  [MSE] = "MSE",
  [ME] = "ME",
};

const int error_name_count = 2;

static int
find_id (char *name, const char *type, const char **array, int last)
{
  int id;
  for (id = 0; id < last; ++id)
    if (strcmp (name, array[id]) == 0)
      return id;
  printf (" -> '%s' not supported as '%s' token\n", name, type);
  exit (-1);
}

static int
get_positive_number (FILE * fp, const char *token_n)
{
  int ret, num;
  double tmp;

  ret = fscanf (fp, "%lf", &tmp);
  if (ret < 0)
    exit (-1);

  num = (int) (tmp);
  if (tmp != num)
    {
      printf ("%s must be an integer number! (%f)\n", token_n, tmp);
      exit (-1);
    }
  if (num < 0)
    {
      printf ("%s must be a positive number! (%d)\n ", token_n, num);
      exit (-1);
    }
  //printf(" -> %s = %d [OK]\n", token_n, num);
  return num;
}

static int
get_strictly_positive_number (FILE * fp, const char *token_n)
{
  int num = get_positive_number (fp, token_n);
  if (num == 0)
    {
      printf ("%s must be a strictly positive number! (%d)\n ", token_n, num);
      exit (-1);
    }
  return num;
}

static const char *switch_n[] = {
  [OFF] = "OFF",
  [ON] = "ON",
};

static const int switch_n_size = 2;

static int
get_switch_value (FILE * fp, const char *name)
{
  int ret;
  char s[128];
  ret = fscanf (fp, "%126s", s);
  if (ret < 0)
    exit (-1);
  return find_id (s, name, switch_n, switch_n_size);
}

static double
get_double_number (FILE * fp)
{
  double tmp;
  int ret = fscanf (fp, "%lf", &tmp);
  if (ret < 0)
    exit (-1);
  return tmp;
}

static double
get_double_positive_number (FILE * fp, const char *msg)
{
  double tmp = get_double_number (fp);
  if (tmp > 0)
    return tmp;
  printf ("%s must be greater than 0!", msg);
  exit (-1);
}

/*
 * sub_neuron_parser: parse the neuron attribute
 * fp: pointer to the script file
 */
static void
sub_neuron_parser (network * nn, network_config * config, FILE * fp)
{
  enum neuron_sub_token_id
  {
    NUMBER_OF_CONNECTIONS,
    ACTIVATION,
    ACCUMULATOR,
    CONNECTION,
  };
  static const char *neuron_sub_token_n[] = {
    [NUMBER_OF_CONNECTIONS] = "NUMBER_OF_CONNECTIONS",
    [ACTIVATION] = "ACTIVATION",
    [ACCUMULATOR] = "ACCUMULATOR",
    [CONNECTION] = "CONNECTION",
  };

  static const char *accumulator_names[] = {
    [LINEAR] = "LINEAR",
    [LEGENDRE] = "LEGENDRE",
    [LAGUERRE] = "LAGUERRE",
    [FOURIER] = "FOURIER",
  };

  static const char *activation_names[] = {
    [TANH] = "TANH",
    [EXP] = "EXP",
    [EXP_SIGNED] = "EXP_SIGNED",
    [SOFTSIGN] = "SOFTSIGN",
    [RAMP] = "RAMP",
    [SOFTRAMP] = "SOFTRAMP",
    [ID] = "ID",
    [POL1] = "POL1",
    [POL2] = "POL2",
  };

  const int neuron_sub_token_count = 4;
  const int accumulator_names_count = 4;
  const int activation_names_count = 9;

  int sub_token, sub_num, index, ret;
  char s[128];

  index = get_positive_number (fp, "NEURON");
  if (index > (nn->num_of_neurons - 1))
    {
      printf ("neuron id out of range!\n");
      exit (-1);
    };

  ret = fscanf (fp, "%126s", s);
  if (ret < 0)
    exit (-1);
  sub_token = find_id (s, "NEURON sub-token",
                       neuron_sub_token_n, neuron_sub_token_count);

  switch (sub_token)
    {
    case NUMBER_OF_CONNECTIONS:
      {
        sub_num = get_positive_number (fp, neuron_sub_token_n[sub_token]);
        network_neuron_set_connection_number (&nn->neurons[index], sub_num);
        printf ("NEURON %d NUMBER_OF_CONNECTIONS = %d [OK]\n", index,
                sub_num);
      }
      break;
    case ACTIVATION:
      {
        ret = fscanf (fp, "%126s", s);
        int activ = find_id (s, neuron_sub_token_n[sub_token],
                             activation_names, activation_names_count);
        nn->neurons[index].activation = activ;
        printf ("NEURON %d ACTIVATION = %s\n", index,
                activation_names[activ]);
      }
      break;
    case ACCUMULATOR:
      {
        ret = fscanf (fp, "%126s", s);
        int accum = find_id (s, neuron_sub_token_n[sub_token],
                             accumulator_names, accumulator_names_count);
        nn->neurons[index].accumulator = accum;
        printf ("NEURON %d ACCUMULATOR = %s\n", index,
                accumulator_names[accum]);
      };
      break;
    case CONNECTION:
      {
        int connection_id =
          get_positive_number (fp, neuron_sub_token_n[sub_token]);
        if (connection_id > (nn->num_of_neurons - 1))
          {
            printf ("the connection index is out of range!\n");
            exit (-1);
          }
        if (connection_id > (nn->neurons[index].num_input - 1))
          {
            printf ("the connection index is out of range!\n");
            exit (-1);
          }

        int global_neuron_id_2 =
          get_positive_number (fp, neuron_sub_token_n[sub_token]);
        if (global_neuron_id_2 > (nn->num_of_neurons - 1))
          {
            printf ("the global index of neuron #2 is out of range!\n");
            exit (-1);
          }
        printf ("NEURON %d CONNECTION %d %d [OK]\n",
                index, connection_id, global_neuron_id_2);
        nn->neurons[index].connection[connection_id] =
          &nn->neurons[global_neuron_id_2];
      };
      break;
      /* close NEURON sub-case; */
    }
}

static void
sub_network_parser (network * nn, network_config * config, FILE * fp)
{
  enum network_sub_token_id
  {
    NUMBER_OF_LAYERS,
    LAYER,
    ASSIGN_NEURON_TO_LAYER,
  };
  static const char *network_sub_token_n[] = {
    [NUMBER_OF_LAYERS] = "NUMBER_OF_LAYERS",
    [LAYER] = "LAYER",
    [ASSIGN_NEURON_TO_LAYER] = "ASSIGN_NEURON_TO_LAYER",
  };
  const int network_sub_token_count = 3;
  int ret, sub_token;
  char s[128];

  /*
   * parser NETWORK Sub-Token
   */
  ret = fscanf (fp, "%126s", s);
  if (ret < 0)
    exit (-1);
  sub_token = find_id (s, "NETWORK sub-token",
                       network_sub_token_n, network_sub_token_count);
  switch (sub_token)
    {
    case NUMBER_OF_LAYERS:
      {
        int num_layers = get_positive_number (fp, "NUMBER_OF_LAYERS");
        network_set_layer_number (nn, num_layers);
        printf ("NETWORK NUMBER_OF_LAYERS = %d [OK]\n", num_layers);
      }
      break;
      // specify the number of neurons of a layer
      // syntax: NETWORK LAYER ind NUMBER_OF_NEURONS num
    case LAYER:
      {
        int ind = get_positive_number (fp, "LAYER");
        if (ind > (nn->num_of_layers - 1))
          {
            printf ("layer index is out of range!\n");
            exit (-1);
          }
        ret = fscanf (fp, "%126s", s);
        if (strcmp (s, "NUMBER_OF_NEURONS") != 0)
          {
            printf ("syntax error!\nNUMBER_OF_NEURONS expected!\n");
            exit (-1);
          }
        int num = get_positive_number (fp, "NUMBER_OF_NEURONS");
        if (num > nn->num_of_neurons)
          {
            printf
              ("the number of neurons in the layer is grater the the total number of neurons!\n");
            printf ("please check your configuration!\n");
            exit (-1);
          }
        nn->layers[ind].num_of_neurons = num;
        printf ("NETWORK LAYER %d NUMBER_OF_NEURONS %d [OK]\n", ind, num);
      }
      break;
      // assigns the neurons to the layers
      // syntax: NETWORK ASSIGN_NEURON_TO_LAYER layer_id local_neuron_id global_neuron_id
    case ASSIGN_NEURON_TO_LAYER:
      {
        int layer_id = get_positive_number (fp, "ASSIGN_NEURON_TO_LAYER 1.");
        if (layer_id > (nn->num_of_layers - 1))
          {
            printf ("layer index out of range!\n");
            exit (-1);
          }
        int local_neuron_id =
          get_positive_number (fp, "ASSIGN_NEURON_TO_LAYER 2.");
        if (local_neuron_id > (nn->layers[layer_id].num_of_neurons - 1))
          {
            printf ("local neuron index out of range!\n");
            exit (-1);
          }
        int global_neuron_id =
          get_positive_number (fp, "ASSIGN_NEURON_TO_LAYER 3.");
        if (global_neuron_id > (nn->num_of_neurons - 1))
          {
            printf ("global neuron index out of range!\n");
            exit (-1);
          }
        printf ("NETWORK ASSIGN_NEURON_TO_LAYER %d %d %d [OK]\n",
                layer_id, local_neuron_id, global_neuron_id);
        /* assign only the first-one */
        if (!nn->layers[layer_id].neurons)
          nn->layers[layer_id].neurons = &nn->neurons[global_neuron_id];
      };
      break;
    default:
      printf ("the specified network feature is unknown!\n");
      exit (-1);
      break;
    }

}

static void
sub_training_method_parser (network * nn, network_config * config, FILE * fp)
{
  static const char *sub_method_token_n[] = {
    [SIMULATED_ANNEALING] = "SIMULATED_ANNEALING",
    [RANDOM_SEARCH] = "RANDOM_SEARCH",
    [GRADIENT_DESCENT] = "GRADIENT_DESCENT",
    [GENETIC_ALGORITHM] = "GENETIC_ALGORITHM",
    [MSMCO] = "MSMCO",
  };
  const int sub_method_token_count = 5;

  int ret, method_id;
  char s[128];
  ret = fscanf (fp, "%126s", s);
  if (ret < 0)
    exit (-1);

  method_id =
    find_id (s, "TRAINING_METHOD", sub_method_token_n,
             sub_method_token_count);
  switch (method_id)
    {
    case SIMULATED_ANNEALING:
      {
        // simulated annealing
        // syntax: verbosity mmax nmax kbtmin kbtmax accuracy
        // where
        // verbosity = ON/OFF
        // mmax      = outer loop - number of effective temperature steps
        // nmax      = inner loop - number of test configurations
        // kbtmin    = effective temperature minimum
        // kbtmax    = effective temperature maximum
        // accuracy  = numerical accuracy
        int verbosity = get_switch_value (fp, "verbosity");
        int mmax = get_positive_number (fp, "simulated annealing mmax");
        if (mmax < 2)
          {
            printf ("MMAX must be greater than 1!\n");
            exit (-1);
          }
        int nmax = get_positive_number (fp, "simulated annealing nmax");
        double kbtmin = get_double_number (fp);
        double kbtmax = get_double_number (fp);;
        if (kbtmin >= kbtmax)
          {
            printf ("KBTMIN must be smaller then KBTMAX!\n");
            exit (-1);
          }
        double eps = get_double_positive_number (fp, "ACCURACY");
        printf ("TRAINING METHOD = SIMULATED ANNEALING %d %d %g %g %g [OK]\n",
                mmax, nmax, kbtmin, kbtmax, eps);
        config->optimization_type = SIMULATED_ANNEALING;
        config->verbosity = verbosity;
        config->mmax = mmax;
        config->nmax = nmax;
        config->kbtmin = kbtmin;
        config->kbtmax = kbtmax;
        config->accuracy = eps;
      };
      break;
      // random search
      // syntax: verbosity nmax accuracy
      // verbosity = ON/OFF
      // nmax      = maximum number of random attempts
      // accuracy  = numerical accuracy
    case RANDOM_SEARCH:
      {
        int verbosity = get_switch_value (fp, "verbosity");
        int nmax = get_positive_number (fp, "random search nmax");
        double eps = get_double_positive_number (fp, "ACCURACY");
        printf ("OPTIMIZATION METHOD = RANDOM SEARCH %d %g [OK]\n", nmax,
                eps);
        config->verbosity = verbosity;
        config->optimization_type = RANDOM_SEARCH;
        config->nmax = nmax;
        config->accuracy = eps;
      }
      break;
      // gradient descent
      // syntax: verbosity nxw maxiter gamma accuracy
      // where
      // verbosity = ON/OFF
      // nxw       = number of cells in one direction of the weight space
      // maxiter   = maximum number of iterations
      // gamma     = step size
      // accuracy  = numerical accuracy
    case GRADIENT_DESCENT:
      {
        int verbosity = get_switch_value (fp, "verbosity");
        int nxw = get_positive_number (fp, "gradient descent nxw");
        int maxiter = get_positive_number (fp, "gradient descent MAXITER");
        double gamma = get_double_positive_number (fp, "GAMMA");
        double eps = get_double_positive_number (fp, "ACCURACY");
        printf ("OPTIMIZATION METHOD = GRADIENT DESCENT %d %d %g %g [OK]\n",
                nxw, maxiter, gamma, eps);
        config->verbosity = verbosity;
        config->optimization_type = GRADIENT_DESCENT;
        config->nxw = nxw;
        config->maxiter = maxiter;
        config->gamma = gamma;
        config->accuracy = eps;
      };
      break;
      // genetic algorithm
      // syntax: verbosity nmax npop rate accuracy
      // where:
      // verbosity = ON/OFF
      // nmax      = number of generations
      // npop      = number of individuals per generation
      // rate      = rate of change between one generation and the parent
      // accuracy  = numerical accuracy
    case GENETIC_ALGORITHM:
      {
        int verbosity = get_switch_value (fp, "verbosity");
        int nmax = get_positive_number (fp, "genetic algorithm nmax");
        int npop = get_positive_number (fp, "genetic algorithm npop");
        double rate = get_double_positive_number (fp, "RATE");
        double eps = get_double_positive_number (fp, "ACCURACY");
        printf ("OPTIMIZATION METHOD = GENETIC ALGORITHM %d %d %g %g [OK]\n",
                nmax, npop, rate, eps);
        config->verbosity = verbosity;
        config->optimization_type = GENETIC_ALGORITHM;
        config->nmax = nmax;
        config->npop = npop;
        config->rate = rate;
        config->accuracy = eps;
      };
      break;
      // multi-scale Monte Carlo algorithm
      // syntax: verbosity mmax nmax rate
      // where:
      // verbosity = ON/OFF
      // mmax      = number of Monte Carlo outer iterations
      // nmax      = number of MC inner iterations
      // rate      = rate of change of the space of search at each iteration
    case MSMCO:
      {
        int verbosity = get_switch_value (fp, "verbosity");
        int mmax = get_positive_number (fp, "multi-scale Monte Carlo mmax");
        int nmax = get_positive_number (fp, "multi-scale Monte Carlo nmax");
        double rate = get_double_positive_number (fp, "RATE");
        printf
          ("OPTIMIZATION METHOD = MULTI-SCALE MONTE CARLO OPTIMIZATION %d %d %g [OK]\n",
           mmax, nmax, rate);
        config->verbosity = verbosity;
        config->optimization_type = MSMCO;
        config->mmax = mmax;
        config->nmax = nmax;
        config->rate = rate;
      };
      break;
    default:
      break;
    }
}

void
parser (network * nn, network_config * config, FILE * fp)
{
  int ret;
  char s[256];
  double tmp;
  unsigned int token_id;

  printf ("\n\
=========================\n\
processing the input file\n\
=========================\n");
  do
    {
      // read the current row
      ret = fscanf (fp, "%254s", s);
      if (ret < 0)
        return;

      token_id = find_id (s, "Token", main_token_n, main_token_count);

      switch (token_id)
        {

        case _COMMENT:
          fgets (s, 80, fp);
          printf ("COMMENT ---> %s", s);
          break;

        case _NUMBER_OF_NEURONS:
          network_set_neuron_number (nn,
                                     get_strictly_positive_number (fp,
                                                                   "TOTAL NUMBER OF NEURONS"));
          printf ("TOTAL NUMBER OF NEURONS = %d [OK]\n", nn->num_of_neurons);
          break;

        case _NEURON:
          sub_neuron_parser (nn, config, fp);
          break;

        case _NETWORK:
          sub_network_parser (nn, config, fp);
          break;

        case _NUMBER_OF_TRAINING_CASES:
          {
            int ncase =
              get_strictly_positive_number (fp, main_token_n[token_id]);
            if (ncase > MAX_TRAINING_CASES)
              {
                printf ("NUMBER_OF_TRAINING_CASES is too large!\n");
                printf
                  ("please increase MAX_TRAINING_CASES and recompile!\n");
                exit (-1);
              }
            printf ("NUMBER_OF_TRAINING_CASES = %d [OK]\n", ncase);
            config->num_cases = ncase;
          }
          break;

          // specify the training cases for supervised learning
          // syntax: TRAINING_CASE IN case_index neuron_index _connection_index value
          // syntax: TRAINING_CASE OUT case_index neuron_index value
        case _TRAINING_CASE:
          {
            ret = fscanf (fp, "%254s", s);
            int direction = find_id (s, main_token_n[token_id],
                                     direction_n, direction_count);
            switch (direction)
              {
              case _IN:
                {
                  int ind = get_positive_number (fp, "training data index");

                  if (ind > (config->num_cases - 1))
                    {
                      printf ("training data index out of range!\n");
                      exit (-1);
                    }
                  int neu =
                    get_positive_number (fp, "TRAINING_CASE neuron index");

                  if (neu > (nn->num_of_neurons - 1))
                    {
                      printf
                        ("TRAINING_CASE IN neuron index out of range!\n");
                      exit (-1);
                    }

                  int conn =
                    get_positive_number (fp,
                                         "TRAINING_CASE connection index");
                  if (conn > (nn->neurons[neu].num_input - 1))
                    {
                      printf
                        ("TRAINING_CASE connection index out of range!\n");
                      exit (-1);
                    }
                  tmp = get_double_number (fp);
                  printf ("TRAINING_CASE IN %d %d %d %f [OK]\n",
                          ind, neu, conn, tmp);
                  config->cases_x[ind][neu][conn] = tmp;
                };
                break;
              case _OUT:
                {
                  int ind = get_positive_number (fp, "training data index");
                  if (ind > (config->num_cases - 1))
                    {
                      printf ("training data index out of range!\n");
                      exit (-1);
                    }
                  int neu =
                    get_positive_number (fp,
                                         "TRAINING_CASE OUT neuron index");
                  if (neu > (nn->num_of_neurons - 1))
                    {
                      printf
                        ("TRAINING_CASE OUT neuron index out of range!\n");
                      exit (-1);
                    }
                  tmp = get_double_number (fp);
                  printf ("TRAINING_CASE OUT %d %d %f [OK]\n", ind, neu, tmp);
                  config->cases_y[ind][neu] = tmp;
                }
              }                 /* close switch (direction) */
          };
          break;

        case _WEIGHT_MINIMUM:
          config->wmin = get_double_number (fp);;
          printf ("WEIGHT_MINIMUM = %f [OK]\n", config->wmin);
          break;

        case _WEIGHT_MAXIMUM:
          config->wmax = get_double_number (fp);
          printf ("WEIGHT_MAXIMUM = %f [OK]\n", config->wmax);
          break;
          // specify the training method
          // syntax: TRAINING_METHOD method values (see below)..
        case _TRAINING_METHOD:
          sub_training_method_parser (nn, config, fp);
          break;

          // specify if some output has to be saved
          // syntax: SAVE_OUTPUT ON/OFF
        case _SAVE_OUTPUT:
          config->save_output = get_switch_value (fp, "save_output");
          printf ("SAVE_OUTPUT %s [OK]\n", switch_n[config->save_output]);
          break;

          // specify the output file name
          // syntax: OUTPUT_FILE_NAME filename
        case _OUTPUT_FILE_NAME:
          {
            ret = fscanf (fp, "%254s", s);
            config->output_file_name = malloc (strlen (s) + 1);
            strcpy (config->output_file_name, s);
            printf ("OUTPUT FILE NAME = %s [OK]\n", config->output_file_name);
            break;
            // specify the number of cases for the output file
            // syntax: NUMBER_OF_INPUT_CASES num
        case _NUMBER_OF_INPUT_CASES:
            {
              int num =
                get_strictly_positive_number (fp, "NUMBER_OF_INPUT_CASES");
              if (num > MAX_NUM_CASES)
                {
                  printf ("NUMBER_OF_INPUT_CASES is too large!\n");
                  printf ("please increase MAX_NUM_CASES and recompile!\n");
                  exit (-1);
                }
              printf ("NUMBER OF INPUT CASES = %d [OK]\n", num);
              config->num_of_cases = num;
            };
            break;
            // specify the input cases for the output file
            // syntax: NETWORK_INPUT case_id neuron_id conn_id val
        case _NETWORK_INPUT:
            {
              int num = get_positive_number (fp, "NETWORK_INPUT case index");
              if (num > (config->num_of_cases - 1))
                {
                  printf ("NETWORK_INPUT case index out of range!\n");
                  exit (-1);
                }

              int neu =
                get_positive_number (fp, "NETWORK_INPUT neuron index");
              if (neu > nn->layers[0].num_of_neurons - 1)
                {
                  printf ("NETWORK_INPUT neuron index out of range!\n");
                  exit (-1);
                }
              int conn =
                get_positive_number (fp, "NETWORK_INPUT connection index");
              if (conn > (nn->neurons[neu].num_input - 1))
                {
                  printf ("NETWORK_INPUT connection index out of range!\n");
                  exit (-1);
                }
              double val = get_double_number (fp);
              printf ("NETWORK INPUT CASE #%d %d %d = %g [OK]\n",
                      num, neu, conn, val);
              config->output_x[num][neu][conn] = val;
            };
            break;
            // save a neural network (structure and weights) in the file network.dat
            // at the end of the training process
            // syntax: SAVE_NEURAL_NETWORK
        case _SAVE_NEURAL_NETWORK:
            {
              ret = fscanf (fp, "%254s", s);
              config->save_network_file_name = malloc (strlen (s) + 1);
              strcpy (config->save_network_file_name, s);
              config->save_neural_network = ON;
              printf ("SAVE NEURAL NETWORK to %s [OK]\n", s);
            };
            break;

            // load a neural network (structure and weights) from the file network.dat
            // at the begining of the training process
            // syntax: LOAD_NEURAL_NETWORK
        case _LOAD_NEURAL_NETWORK:
            {
              ret = fscanf (fp, "%254s", s);
              config->load_network_file_name = malloc (strlen (s) + 1);
              strcpy (config->load_network_file_name, s);
              config->save_neural_network = ON;
              printf ("LOAD NEURAL NETWORK from %s [OK]\n", s);
            };
            break;

            // perform a random initialization of the weights
            // syntax: INITIAL_WEIGHTS_RANDOMIZATION ON/OFF
        case _INITIAL_WEIGHTS_RANDOMIZATION:
            {
              int flag = get_switch_value (fp, main_token_n[token_id]);
              config->initial_weights_randomization = flag;
              printf ("INITIAL_WEIGHTS_RANDOMIZATION = %s [OK]\n",
                      switch_n[flag]);
            };
            break;
            // specify the error function for the training process
            // syntax: ERROR_TYPE MSE/ME
        case _ERROR_TYPE:
            {
              ret = fscanf (fp, "%254s", s);
              int errtype = find_id (s, main_token_n[token_id],
                                     error_names, error_name_count);
              config->error_type = errtype;
              printf ("ERROR_TYPE = %s [OK]\n", error_names[errtype]);
            };
            break;
          }                     /* close switch(token_id) */
        }
      s[0] = '\0';              // empty the buffer
    }
  while (!feof (fp));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Below this point, all the definitions are provided for nnet (second build target with new network representation)    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static const char *acctokens[ACCUMCOUNT] = { ACCTOKENS };
static const char *outtokens[OUTPUTCOUNT] = { OUTTOKENS };

// output parse error msg and halt.  To free any dynamically allocated buffers, pass their address as free1. Otherwise pass NULL.
void
ErrStopParsing (struct slidingbuffer *bf, const char *msg, void *free1)
{
  assert (msg != NULL);
  free (free1);
  fflush (stdout);
  fclose (bf->input);
  if (bf == NULL)
    fprintf (stderr, "\n %s\n", msg);
  else
    fprintf (stderr, "\nLine %d Col %d : %s\n", bf->line, bf->col, msg);
  exit (1);
}

// Add a warning to the slidingbuffer - don't output it yet.
void
AddWarning (struct slidingbuffer *bf, const char *msg)
{
  assert (msg != NULL);
  int warnlen = bf->warnings == NULL ? 0 : strlen (bf->warnings);
  int size = warnlen + strlen (msg) + 50;
  bf->warnings = (char *) realloc (bf->warnings, size);
  char *cursor = &(bf->warnings[warnlen]);
  snprintf (cursor, size, "Line %3d Col %3d : %s\n", bf->line, bf->col, msg);
}

// Print all warning messages so far saved in the slidingbuffer. Called from nnet.c
void
PrintWarnings (struct slidingbuffer *bf)
{
  if (bf->warnings != NULL)
    fprintf (stderr, "\n%s", bf->warnings);
}

// Ensure that there are at least min characters available in buffer.  Return 0 on fail, #chars available on success.
int
ChAvailable (struct slidingbuffer *bf, int min)
{
  assert (bf != NULL);
  if (bf->head - bf->end < min)
    while (!feof (bf->input) && (bf->head - bf->end < BFLEN))
      bf->buffer[(bf->head++) % BFLEN] = fgetc (bf->input);
  if (bf->head - bf->end < min)
    return (0);
  return (bf->head - bf->end);
}

// tell what the next character is without accepting it.
int
NextCh (struct slidingbuffer *bf)
{
  assert (bf != NULL);
  if (ChAvailable (bf, 1))
    return (bf->buffer[bf->end % BFLEN]);
  else
    return (0);
}

// accept the specified number of characters updating the line & column counts.  Return #characters accepted.  Error messages give line numbers, so for verbose
// debugging this routine outputs accepted lines prefixed with line numbers.
int
AcceptCh (struct slidingbuffer *bf, struct conf *config, int len)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (len > 0);
  char next;
  static int AnnouncedLineZero = 0;
  if (!AnnouncedLineZero++)
    printf ("    0: ");
  if (ChAvailable (bf, len))
    {
      for (int cn = 0; cn < len; cn++)
        {
          next = NextCh (bf);
          if ('\n' == next)
            {
              if ((config->flags & SILENCE_ECHO) == 0)
                printf ("\n %4d: ", ++(bf->line));
              bf->col = 0;
            }
          else
            {
              if ((config->flags & SILENCE_ECHO) == 0)
                printf ("%c", next);
              bf->col++;
            }
          bf->end++;
        }
      return (len);
    }
  return (0);
}

// return tokenlength iff a given token is ready to read from bf, 0 otherwise.
int
TokenAvailable (struct slidingbuffer *bf, const char *token)
{
  assert (bf != NULL);
  assert (token != NULL);
  int goal = strlen (token);
  if (!ChAvailable (bf, goal))
    return (0);
  for (int ct = 0; ct < goal; ct++)
    if (bf->buffer[(bf->end + ct) % BFLEN] != token[ct])
      return (0);
  return (goal);
}

// my duck is still on fire.

// accept a given token iff it is available at beginning of input.  Return #characters accepted
int
AcceptToken (struct slidingbuffer *bf, struct conf *config, const char *token)
{
  assert (bf != NULL);
  assert (token != NULL);
  return (AcceptCh (bf, config, TokenAvailable (bf, token)));
}


// skip as much whitespace as there is available.
void
SkipWhiteSpace (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  while (ChAvailable (bf, 1) && isspace (NextCh (bf)))
    AcceptCh (bf, config, 1);
}

// skip a comment iff a comment is at head of input. (comments are everything between # and EOL.)
void
SkipComment (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  assert (config != NULL);
  if (TokenAvailable (bf, "#"))
    while (NextCh (bf) != '\n')
      AcceptCh (bf, config, 1);
}


// read past all whitespace and comments to the next non-skipped character.
void
SkipToNext (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  assert (config != NULL);
  do
    {
      SkipWhiteSpace (bf, config);
      SkipComment (bf, config);
    }
  while (isspace (NextCh (bf)) || TokenAvailable (bf, "#"));
}

// returns nonzero iff a number is available starting at the next character.
int
NumberAvailable (struct slidingbuffer *bf)
{
  assert (bf != NULL);
  int next = NextCh (bf);
  return (next == '+' || next == '-' || isdigit (next));
}


// Read and return the number.  Controlled fail with helpful message if none available or wrong syntax. You must call 'NumberAvailable' to check validity first.
int
ReadInteger (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (NumberAvailable (bf));
  static const int maxlen = (int) (log10 (INT_MAX)) + 2;
  char buf[maxlen];
  ChAvailable (bf, 2);
  int count = 0;
  int negate = (NextCh (bf) == '-');
  if (negate || (NextCh (bf) == '+'))
    AcceptCh (bf, config, 1);
  if (!isdigit (NextCh (bf)))
    ErrStopParsing (bf, "Expected Integer.", NULL);
  while (count < maxlen && ChAvailable (bf, 1) && isdigit (NextCh (bf)))
    {
      buf[count++] = NextCh (bf);
      AcceptCh (bf, config, 1);
    }
  if ('.' == NextCh (bf))
    ErrStopParsing (bf, "Found Decimal Fraction, Expected Integer.", NULL);
  if (count >= maxlen - 2)
    ErrStopParsing (bf, "Integer too long.", NULL);
  buf[count] = 0;
  return ((1 - (2 * negate)) * atoi (buf));
}

// Read and return the number.  Controlled fail with helpful message if none available or wrong syntax.  You must call 'NumberAvailable' first.
flotype
ReadFloatingPoint (struct slidingbuffer * bf, struct conf * config)
{
  assert (bf != NULL);
  assert (config != 0);
  assert (NumberAvailable (bf));
  static const int maxlen = 1085;       // max decimal length for (negative, denormalized, 64-bit) double is 1079!!  That's CRAZY!
  char buf[maxlen];
  int count = 0;
  int before = 0;
  int after = 0;
  if (!TokenAvailable (bf, "+") && !TokenAvailable (bf, "-")
      && !isdigit (NextCh (bf)))
    return (0);
  if (ChAvailable (bf, 1) && (NextCh (bf) == '-' || NextCh (bf) == '+'))
    {
      buf[count++] = NextCh (bf);
      AcceptCh (bf, config, 1);
    }
  if (!ChAvailable (bf, 1) || !isdigit (NextCh (bf)))
    ErrStopParsing (bf, "Expected Floating Point Value.", NULL);
  while (ChAvailable (bf, 1) && isdigit (NextCh (bf)) && count < maxlen - 1)
    {
      before = 1;
      buf[count++] = NextCh (bf);
      AcceptCh (bf, config, 1);
    }
  if (!ChAvailable (bf, 1) || NextCh (bf) != '.')
    ErrStopParsing (bf, "Floating-point values must have a decimal point.",
                    NULL);
  else
    {
      buf[count++] = NextCh (bf);
      AcceptCh (bf, config, 1);
    }
  while (ChAvailable (bf, 1) && isdigit (NextCh (bf)) && count < maxlen - 1)
    {
      after = 1;
      buf[count++] = NextCh (bf);
      AcceptCh (bf, config, 1);
    }
  if (before == 0 || after == 0)
    ErrStopParsing (bf,
                    "Floating-point values must have digits before and after decimal.",
                    NULL);
  if (ChAvailable (bf, 2) && count < (maxlen - 2)
      && (AcceptToken (bf, config, "e") || AcceptToken (bf, config, "E")))
    {
      buf[count++] = 'e';
      if (count < maxlen - 2 && (NextCh (bf) == '+' || NextCh (bf) == '-'))
        {
          buf[count++] = NextCh (bf);
          AcceptCh (bf, config, 1);
        }
      if (!isdigit (NextCh (bf)))
        ErrStopParsing (bf,
                        "Scientific notation floats must have digits in exponent.",
                        NULL);
      else
        while (count < maxlen - 2 && ChAvailable (bf, 1)
               && isdigit (NextCh (bf)))
          {
            buf[count++] = NextCh (bf);
            AcceptCh (bf, config, 1);
          }
    }
  if (count >= maxlen - 2)
    ErrStopParsing (bf, "Floating-point value is too long.", NULL);
  buf[count] = 0;
  int non0digits = 0;
  for (; --count >= 0;)
    non0digits +=
      buf[count] == 'e' ? -non0digits : (int) (isdigit (buf[count])
                                               && buf[count] != '0');
  double retval = atof (buf);
  if (retval == 0.0 && non0digits != 0)
    ErrStopParsing (bf,
                    "Nonzero float in source was rounded to zero on read.",
                    NULL);
  if (retval == HUGE_VAL)
    ErrStopParsing (bf, "Float value in source exceeds float range.", NULL);
  return ((flotype) retval);
}


// returns zero for failure, nonzero for success.
int
ReadIntSpan (struct slidingbuffer *bf, struct conf *config, int *start,
             int *end)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (start != NULL);
  assert (end != NULL && end != start);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "{"))
    return (0);
  SkipToNext (bf, config);
  if (!NumberAvailable (bf))
    ErrStopParsing (bf,
                    "Integer span starts with non-numeric token.  Integer Expected.",
                    NULL);
  *start = ReadInteger (bf, config);
  SkipToNext (bf, config);
  if (!NumberAvailable (bf))
    ErrStopParsing (bf,
                    "Integer span ends with non-numeric token.  Integer Expected.",
                    NULL);
  *end = ReadInteger (bf, config);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "}"))
    ErrStopParsing (bf, "Integer spans must end with \'}\'.", NULL);
  if (*start > *end)
    {
      int shuffle = *end;
      *end = *start;
      *start = shuffle;
    }
  return (1);
}


// read a weight matrix.  Return 0 for failure, 1 for success
int
ReadWeightMatrix (struct slidingbuffer *bf, struct conf *config,
                  flotype * target, int size)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (target != NULL);
  assert (size > 0);
  int index;
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "["))
    return (0);
  for (index = 0; index < size; index++)
    {
      SkipToNext (bf, config);
      if (NumberAvailable (bf))
        target[index] = ReadFloatingPoint (bf, config);
      else if (TokenAvailable (bf, "]"))
        ErrStopParsing (bf, "Weight matrix has too few weights.", NULL);
      else
        ErrStopParsing (bf, "Non-numeric token in weight matrix.", NULL);
    }
  SkipToNext (bf, config);
  if (NumberAvailable (bf))
    ErrStopParsing (bf, "Weight matrix has too many weights.", NULL);
  if (!AcceptToken (bf, config, "]"))
    ErrStopParsing (bf, "']' expected at end of weight matrix.", NULL);
  return (1);
}


// reads a quoted string with c-style control-character escapes. This procedure allocates.
int
ReadQuotedString (struct slidingbuffer *bf, struct conf *config,
                  char **retstring)
{
  assert (bf != NULL);
  assert (config != NULL);
  int allocsize = 0;
  char *retval = NULL;
  *retstring = NULL;
  if (!AcceptToken (bf, config, "\""))
    return (0);
  for (int index = 0; 1 == 1; index++)
    {
      if (!ChAvailable (bf, 1))
        ErrStopParsing (bf,
                        "While reading a string, reached end of input without finding a closing quote.",
                        retval);
      if (index + 3 >= allocsize)
        {
          allocsize = MAX (allocsize * 2, 256);
          retval = (char *) realloc (retval, allocsize * sizeof (char));        //increase allocation.
          if (retval == NULL)
            {
              fprintf (stderr,
                       "Runtime Error: Allocation Failure(1) in ReadQuotedString\n");
              exit (1);
            }
        }
      if (AcceptToken (bf, config, "\""))
        {
          retval[index++] = '\0';
          *retstring = realloc (retval, index * sizeof (char)); //reduce allocation to size actually used.
          if (retval == NULL)
            {
              fprintf (stderr,
                       "Runtime Error: Allocation Failure(2) in ReadQuotedString\n");
              exit (1);
            }
          return (1);
        }
      else if (ChAvailable (bf, 2) && AcceptToken (bf, config, "\\"))
        {                       // handle c-style escapes
          if (AcceptToken (bf, config, "b"))
            retval[index++] = '\b';
          else if (AcceptToken (bf, config, "f"))
            retval[index++] = '\f';
          else if (AcceptToken (bf, config, "n"))
            retval[index++] = '\n';
          else if (AcceptToken (bf, config, "r"))
            retval[index++] = '\r';
          else if (AcceptToken (bf, config, "t"))
            retval[index++] = '\t';
          else if (AcceptToken (bf, config, "v"))
            retval[index++] = '\v';
          else if (AcceptToken (bf, config, "\\"))
            retval[index++] = '\\';
          else if (isspace (NextCh (bf)))
            SkipWhiteSpace (bf, config);        // backslash-whitespace eats any amount of whitespace (for return and indentation)
        }
      else
        {
          retval[index++] = NextCh (bf);
          AcceptCh (bf, config, 1);
        }
    }
  fprintf (stderr, "Program Error: Unhandled case in ReadQuotedString.\n");
  exit (1);
}

#define WARNSIZE 256

int
ReadCreateNodeStmt (struct slidingbuffer *bf, struct conf *config,
                    struct nnet *net)
{
  assert (bf != NULL);
  assert (net != NULL);
  int in_hid_out = 0;
  int NumToCreate = 0;
  int Accum = 0;
  int Transfer = 0;
  int unitwidth = 1;
  char warnstring[WARNSIZE];
  SkipToNext (bf, config);
  in_hid_out =
    AcceptToken (bf, config, "CreateInput") ? 1 : AcceptToken (bf, config,
                                                               "CreateHidden")
    ? 2 : AcceptToken (bf, config, "CreateOutput") ? 3 : 0;
  if (in_hid_out == 0)
    return (0);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf, "Expected Open Parenthesis", NULL);
  SkipToNext (bf, config);
  if (!NumberAvailable (bf))
    ErrStopParsing (bf,
                    "First argument should give the number of nodes you want to create.",
                    NULL);
  NumToCreate = ReadInteger (bf, config);
  if (NumToCreate <= 0)
    ErrStopParsing (bf, "It doesn't make sense to create less than one node.",
                    NULL);
  SkipToNext (bf, config);
  for (Accum = 0;
       Accum < ACCUMCOUNT && !AcceptToken (bf, config, acctokens[Accum]);
       Accum++);
  if (Accum == ACCUMCOUNT)
    {
      int ct = 0;
      int printed =
        snprintf (warnstring, WARNSIZE,
                  "Expected the name of an accumulator function:");
      for (ct = 0;
           ct < ACCUMCOUNT && printed + strlen (acctokens[ct]) + 5 < WARNSIZE;
           ct++)
        printed +=
          snprintf (&(warnstring[printed]), WARNSIZE - printed, " %s",
                    acctokens[ct]);
      snprintf (&(warnstring[printed]), WARNSIZE - printed, ".");
      if (ct != ACCUMCOUNT)
        {
          fprintf (stderr,
                   "Program Error: warnstring too small to hold list of input functions in ReadCreateNodeStmt.\n");
          exit (1);
        }
    }
  if (in_hid_out == 3 && (strcmp (acctokens[Accum], "None") == 0))
    AddWarning (bf,
                "Warning: an output node with 'None' for an accumulator function will never produce a useful output.");
  SkipToNext (bf, config);
  for (Transfer = 0;
       Transfer < OUTPUTCOUNT
       && !AcceptToken (bf, config, outtokens[Transfer]); Transfer++);
  if (Transfer == OUTPUTCOUNT)
    {
      int ct = 0;
      int printed =
        snprintf (warnstring, WARNSIZE,
                  "Expected the name of an activation function:");
      for (ct = 0;
           ct < OUTPUTCOUNT
           && printed + strlen (outtokens[ct]) + 5 < WARNSIZE; ct++)
        printed +=
          snprintf (&(warnstring[printed]), WARNSIZE - printed, " %s",
                    outtokens[ct]);
      snprintf (&(warnstring[printed]), WARNSIZE - printed, ".");
      if (ct != ACCUMCOUNT)
        {
          fprintf (stderr,
                   "Program Error: warnstring too small to hold list of activation functions in ReadCreateNodeStmt.\n");
          exit (1);
        }
      ErrStopParsing (bf, warnstring, NULL);
    }
  if (Transfer >= SINGLEOUTPUTS)
    {
      SkipToNext (bf, config);
      if (!NumberAvailable (bf))
        ErrStopParsing (bf, "Expected Integer.", NULL);
      unitwidth = ReadInteger (bf, config);
      SkipToNext (bf, config);
    }
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf, "Expected Close Parenthesis", NULL);
  switch (in_hid_out)
    {
    case 1:
      AddInputNodes (net, NumToCreate, Transfer, Accum, unitwidth);
      if ((config->flags & SILENCE_RENUMBER) != 0
          && net->nodecount > net->inputcount)
        {
          snprintf (warnstring, WARNSIZE,
                    "Warning: New Input nodes are numbered %d to %d.  Existing hidden and output nodes have been renumbered %d to %d.",
                    net->inputcount - NumToCreate, net->inputcount - 1,
                    net->inputcount, net->nodecount - 1);
          AddWarning (bf, warnstring);
        }
      break;
    case 2:
      AddHiddenNodes (net, NumToCreate, Transfer, Accum, unitwidth);
      if ((config->flags & SILENCE_RENUMBER) != 0 && net->outputcount > 0)
        {
          snprintf (warnstring, WARNSIZE,
                    "Warning: New Hidden nodes are numbered %d to %d.  Existing output nodes have been renumbered %d to %d.",
                    net->nodecount - net->outputcount - NumToCreate,
                    net->nodecount - net->outputcount - 1,
                    net->nodecount - net->outputcount, net->nodecount - 1);
          AddWarning (bf, warnstring);
        }
      break;
    case 3:
      AddOutputNodes (net, NumToCreate, Transfer, Accum, unitwidth);
      break;
    default:
      fprintf (stderr,
               "Program Error: Unhandled case in ReadCreateNodeStmt.\n");
      exit (1);
    }
  return (1);
}

int
ReadConnectStmt (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (bf != NULL);
  assert (net != NULL);
  int firstlow;
  int secondlow;
  int firsthigh;
  int secondhigh;
  flotype weight = ZERO;
  int imm_rnd_matrix = 0;
  int weightcount = 0;
  flotype *weightlist = NULL;
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "Connect"))
    return (0);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf, "Expected Open Parenthesis", NULL);
  SkipToNext (bf, config);
  if (TokenAvailable (bf, "{"))
    ReadIntSpan (bf, config, &firstlow, &firsthigh);
  else if (NumberAvailable (bf))
    firstlow = firsthigh = ReadInteger (bf, config);
  else
    ErrStopParsing (bf, "Expected Integer or '{'.", NULL);
  SkipToNext (bf, config);
  if (TokenAvailable (bf, "{"))
    ReadIntSpan (bf, config, &secondlow, &secondhigh);
  else if (NumberAvailable (bf))
    secondlow = secondhigh = ReadInteger (bf, config);
  else
    ErrStopParsing (bf, "Expected Integer or '{'.", NULL);
  SkipToNext (bf, config);
  if (NumberAvailable (bf))
    weight = ReadFloatingPoint (bf, config);
  else if (AcceptToken (bf, config, "Randomize"))
    imm_rnd_matrix = 1;
  else if (TokenAvailable (bf, "["))
    {
      weightcount = (1 + firsthigh - firstlow) * (1 + secondhigh - secondlow);
      weightlist = (flotype *) malloc (sizeof (flotype) * weightcount);
      if (weightlist == NULL)
        {
          fprintf (stderr,
                   "Runtime Error: Allocation Failure in ReadConnectStmt\n");
          exit (1);
        }
      imm_rnd_matrix = 2;
      ReadWeightMatrix (bf, config, weightlist, weightcount);
    }
  else
    ErrStopParsing (bf, "Expected floating point value, 'Randomize', or '['",
                    NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf, "Expected Close Parenthesis", weightlist);
  if (firstlow < 0 || secondlow < 0)
    ErrStopParsing (bf, "Connect Statement contains negative node ID.",
                    weightlist);
  if (secondlow == 0)
    ErrStopParsing (bf, "Connect Statement names bias node as destination.",
                    weightlist);
  if (firsthigh >= net->nodecount || secondhigh >= net->nodecount)
    ErrStopParsing (bf,
                    "Connect statement contains node index greater than network's node count.",
                    weightlist);
  switch (imm_rnd_matrix)
    {
    case 0:
      AddConnections (net, firstlow, firsthigh, secondlow, secondhigh,
                      &weight);
      break;
    case 1:
      AddRandomizedConnections (net, firstlow, firsthigh, secondlow,
                                secondhigh);
      break;
    case 2:
      AddConnections (net, firstlow, firsthigh, secondlow, secondhigh,
                      weightlist);
      break;
    default:
      {
        fprintf (stderr,
                 "Program Error: unhandled case in ReadConnectStmt.\n");
        exit (1);
      }
    }
  free (weightlist);
  return (1);
}


// an opening comment from the nnet file is to be preserved and written back to the output file.  Aside from people putting project notes into the source file
// and notes on how to connect it etc, this should allow for the preservation of a "shebang" line invoking nnet as a script handler on an executable script
// file. We don't want to confuse the line/column counts for subsequent error messages, so no bypassing AcceptCh with the 'obvious' fgets or getline call.
void
ReadOpeningComment (struct slidingbuffer *bf, struct conf *config)
{

  assert (config != NULL);
  assert (bf != NULL);
  size_t index = 0;
  size_t allocsize = 240;
  char *checkval;
  char ch2Add;

  if (!TokenAvailable (bf, "#"))
    {
      config->openingcomment = NULL;
      return;
    }
  config->openingcomment = (char *) malloc (allocsize * sizeof (char));
  if (config->openingcomment == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure(1) in ReadOpeningComment.\n");
      exit (1);
    }
  while (TokenAvailable (bf, "#"))
    {
      while ('\n' != (ch2Add = NextCh (bf)))
        {
          AcceptCh (bf, config, 1);
          if (allocsize <= index + 2)
            {
              allocsize = 2 * allocsize;
              config->openingcomment =
                realloc (config->openingcomment, allocsize);
              if (config->openingcomment == NULL)
                {
                  fprintf (stderr,
                           "Runtime Error: Allocation failure(2) in ReadOpeningComment.\n");
                  exit (1);
                }
            }
          config->openingcomment[index++] = ch2Add;
        }
      config->openingcomment[index++] = '\n';
    }
  config->openingcomment[index] = '\0';
  checkval = realloc (config->openingcomment, index);   // reduce allocation
  if (checkval != NULL)
    config->openingcomment = checkval;  // existing pointer value. It's not an allocation failure because we don't need to read more.
}


int
ReadSilenceStatement (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  assert (config != NULL);
  if (!AcceptToken (bf, config, "Silence"))
    return (0);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf, "Silence statements must be followed by '('", NULL);
  while (1 == 1)
    {                           // Loop exit is via function return or ErrStopParsing call.
      SkipToNext (bf, config);
      if (AcceptToken (bf, config, ")"))
        return (1);
      else if (AcceptToken (bf, config, "Bias"))
        config->flags |= SILENCE_BIAS;
      else if (AcceptToken (bf, config, "Debug"))
        config->flags |= SILENCE_DEBUG;
      else if (AcceptToken (bf, config, "Echo"))
        config->flags |= SILENCE_ECHO;
      else if (AcceptToken (bf, config, "Input"))
        config->flags |= SILENCE_INPUT;
      else if (AcceptToken (bf, config, "Output"))
        config->flags |= SILENCE_OUTPUT;
      else if (AcceptToken (bf, config, "NodeInput"))
        config->flags |= SILENCE_NODEINPUT;
      else if (AcceptToken (bf, config, "NodeOutput"))
        config->flags |= SILENCE_NODEOUTPUT;
      else if (AcceptToken (bf, config, "MultiActivation"))
        config->flags |= SILENCE_MULTIACTIVATION;
      else if (AcceptToken (bf, config, "Recurrence"))
        config->flags |= SILENCE_RECURRENCE;
      else if (AcceptToken (bf, config, "Renumber"))
        config->flags |= SILENCE_RENUMBER;
      else
        ErrStopParsing
          (bf,
           "Expected close paren or one of Bias, Debug, Echo, Input, Output, NodeInput, NodeOutput, MultiActivation, Recurrence, and Renumber.",
           NULL);
    }
}

// semantics of save arguments:  <Integer>: make 1 save per epoch or this many equally-distributed saves whichever's less.
// <String>: a string in quotes gives the (non-serialized) output filename.  in this case the ".out" extension is not used and serial number (plus a period)
// goes after the last period in the string.
// Serialize: insert a serial number into the savefile filenames.  Serial numbers start with any serial# in the input filename, or 0 if none.

int
ReadSaveStatement (struct slidingbuffer *bf, struct conf *config)
{
  assert (bf != NULL);
  assert (config != NULL);
  char *fname = NULL;
  if (!AcceptToken (bf, config, "Save"))
    return (0);
  config->flags &= (~SAVE_DEFAULT);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf, "Save Statements must be followed by '('", NULL);
  while (1 == 1)
    {
      SkipToNext (bf, config);
      if (AcceptToken (bf, config, ")"))
        return (1);
      else if (AcceptToken (bf, config, "Serialize"))
        config->flags |= SAVE_SERIALIZE;
      else if (ReadQuotedString (bf, config, &fname))
        {
          free (config->savename);
          config->savename = fname;
        }
      else if (NumberAvailable (bf))
        config->savecount = ReadInteger (bf, config);
      else
        ErrStopParsing (bf,
                        "Expected close parenthesis, the keyword 'Serialize', a count of saves to make, or savefile name(in \"quotes\").",
                        fname);
    }
}

// Silence statements
// Save("string"), Save(Serialize),Save(#intermediate savefiles);
int
ReadConfigSection (struct slidingbuffer *bf, struct conf *config,
                   struct nnet *net)
{
  assert (net != NULL);
  assert (config != NULL);
  assert (bf != NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "StartConfig"))
    return (0);
  SkipToNext (bf, config);
  while (ReadSilenceStatement (bf, config) || ReadSaveStatement (bf, config))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "EndConfig"))
    ErrStopParsing (bf, "Expected 'Silence', 'Save', or 'EndConfig'", NULL);
  return (1);
}



// read a case data list.  Return 0 for failure, 1 for success.  This is separate from ReadWeightMatrix solely because it provides customized error messages.
int
ReadIOVals (struct slidingbuffer *bf, struct conf *config, flotype * target,
            int size)
{
  assert (bf != NULL);
  assert (target != NULL);
  assert (size > 0);
  int index;
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "["))
    return (0);
  for (index = 0; index < size; index++)
    {
      SkipToNext (bf, config);
      if (NumberAvailable (bf))
        target[index] = ReadFloatingPoint (bf, config);
      else if (TokenAvailable (bf, "]"))
        ErrStopParsing (bf, "Not enough Inputs/Outputs in case.", NULL);
      else
        ErrStopParsing (bf, "Non-numeric token in Input/Output list.", NULL);
    }
  SkipToNext (bf, config);
  if (NumberAvailable (bf))
    ErrStopParsing (bf, "Case has too many inputs or outputs.", NULL);
  if (!AcceptToken (bf, config, "]"))
    ErrStopParsing (bf, "']' expected at end of Inputs/Outputs.", NULL);
  return (1);
}


// [[ each case is double open square bracket, list of float values, optional ][(close-open braces), list of values, closing double square bracket.]]  note that
// close-open braces are required unless ReadNoInput or ReadNoOutput have been specified.  We allocate and save a number of values consistent with our
// input/output size, which may mismatch the number of values present in the case.
int
ReadImmediateCase (struct slidingbuffer *bf, struct conf *config,
                   struct cases *dat, size_t casenumber)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (dat != NULL);
  size_t casesize = dat->inputcount + dat->outputcount;
  size_t startingpoint = casenumber * casesize;
  SkipToNext (bf, config);
  if (!TokenAvailable (bf, "["))
    return (0);
  if (dat->data == NULL)
    {
      dat->data = (flotype *) calloc (16, casesize * sizeof (flotype));
      dat->entrycount = 16;
    }
  if (dat->data == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure (1) in ReadImmediateCase.\n");
      exit (1);
    }
  else if (casenumber == dat->entrycount)
    {
      dat->data =
        realloc (dat->data,
                 2 * dat->entrycount * casesize * sizeof (flotype));
      dat->entrycount *= 2;
    }
  if (dat->data == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure (2) in ReadImmediateCase.\n");
      exit (1);
    }
  if (dat->inputcount == 0 || dat->outputcount == 0)
    {
      ReadIOVals (bf, config, &(dat->data[startingpoint]), casesize);
      return (1);
    }
  AcceptToken (bf, config, "[");
  if (!ReadIOVals (bf, config, &(dat->data[startingpoint]), dat->inputcount))
    ErrStopParsing (bf, "Input Sequence must begin with '['.", NULL);
  if (!ReadIOVals
      (bf, config, &(dat->data[startingpoint + dat->inputcount]),
       dat->outputcount))
    ErrStopParsing (bf, "Output Sequence must begin with '['.", NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "]"))
    ErrStopParsing (bf, "Data cases must end with ']' character.", NULL);
  return (1);
}


// Reading Data Statements is complicated.
// It is possible for the program to exit here with unreleased allocated memory.  In a threaded future or if ErrStopParsing ever ceases to exit this will need fixed.
int
ReadDataStatement (struct slidingbuffer *bf, struct conf *config,
                   struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  struct cases *newdata;
  if (!AcceptToken (bf, config, "Data"))
    return (0);
  else
    {
      newdata = (struct cases *) calloc (1, sizeof (struct cases));
      newdata->inputcount = net->inputcount;
      newdata->outputcount = net->outputcount;
      newdata->next = net->data;
      SkipToNext (bf, config);
    }
  if (AcceptToken (bf, config, "("))
    SkipToNext (bf, config);
  else
    ErrStopParsing (bf, "Expected open paren in Data Statement.", newdata);
  if (AcceptToken (bf, config, "Immediate"))
    newdata->flags |= DATA_IMMEDIATE | DATA_SEEKABLE;   // cases to follow inline
  else if (AcceptToken (bf, config, "FromFile"))
    newdata->flags |= DATA_FROMFILE | DATA_SEEKABLE;    // each line is one case
  else if (AcceptToken (bf, config, "FromDirectory"))
    newdata->flags |= DATA_FROMDIRECTORY | DATA_SEEKABLE;       // each file is one case
  else if (AcceptToken (bf, config, "FromPipe"))
    newdata->flags |= DATA_FROMPIPE;
  else
    ErrStopParsing (bf,
                    "Data statement missing source keyword (Immediate, FromFile, FromDirectory, or FromPipe).",
                    newdata);
  SkipToNext (bf, config);
  if ((TokenAvailable (bf, "Immediate") || TokenAvailable (bf, "FromFile")
       || TokenAvailable (bf, "FromDirector")
       || TokenAvailable (bf, "FromPipe")))
    ErrStopParsing (bf,
                    "Extra data source found.  If you need to read data from multiple sources the script needs multiple data statements.",
                    NULL);
  if ((newdata->
       flags & (DATA_FROMFILE | DATA_FROMDIRECTORY | DATA_FROMPIPE)) != 0x0)
    {
      if (!ReadQuotedString (bf, config, &(newdata->inname)))
        ErrStopParsing (bf,
                        "From(File|Directory|Pipe) must be followed by a quoted string giving the name of a file, directory, or named pipe.",
                        newdata);
      else
        SkipToNext (bf, config);
    }
  if (!
      (TokenAvailable (bf, "Training") || TokenAvailable (bf, "Testing")
       || TokenAvailable (bf, "Validation")
       || TokenAvailable (bf, "Deployment")))
    ErrStopParsing (bf,
                    "Data Statement missing use keyword (Training, Testing, Validation, or Deployment).",
                    newdata);
  else
    while (TokenAvailable (bf, "Training") || TokenAvailable (bf, "Testing")
           || TokenAvailable (bf, "Validation")
           || TokenAvailable (bf, "Deployment"))
      {
        if (AcceptToken (bf, config, "Training"))
          newdata->flags |= DATA_TRAINING;
        else if (AcceptToken (bf, config, "Testing"))
          newdata->flags |= DATA_TESTING;
        else if (AcceptToken (bf, config, "Validation"))
          newdata->flags |= DATA_VALIDATION;
        else if (AcceptToken (bf, config, "Deployment"))
          newdata->flags |= DATA_DEPLOYMENT;
        SkipToNext (bf, config);
      }
  while (TokenAvailable (bf, "ReadNoInput")
         || TokenAvailable (bf, "ReadNoOutput")
         || TokenAvailable (bf, "ReadNoInput")
         || TokenAvailable (bf, "ReadNoOutput"))
    {
      if (AcceptToken (bf, config, "ReadNoInput"))
        {
          newdata->flags |= DATA_NOINPUT;
          newdata->inputcount = 0;
        }
      else if (AcceptToken (bf, config, "ReadNoOutput"))
        {
          newdata->flags |= DATA_NOOUTPUT;
          newdata->outputcount = 0;
        }
      else if (AcceptToken (bf, config, "ReadNoInput"))
        newdata->flags |= DATA_NOWRITEINPUT;
      else if (AcceptToken (bf, config, "ReadNoOutput"))
        newdata->flags |= DATA_NOWRITEOUTPUT;
      SkipToNext (bf, config);
    }
  if ((newdata->flags & DATA_NOINPUT) != 0x0
      && (newdata->flags & DATA_NOOUTPUT) != 0x0)
    ErrStopParsing (bf,
                    "Both of flags ReadNoInput and ReadNoOutput found.  This means there is no data to read.",
                    newdata);
  if ((newdata->flags & DATA_NOWRITEINPUT) != 0x0
      && (newdata->flags & DATA_NOWRITEOUTPUT) != 0x0)
    ErrStopParsing (bf,
                    "Both WriteNoInput and WriteNoOutput found.  If there is nothing to write then don't use ToFile or ToPipe.",
                    NULL);
  if ((newdata->flags & DATA_DEPLOYMENT) != 0
      && (newdata->
          flags & (DATA_NOOUTPUT | DATA_TRAINING | DATA_TESTING |
                   DATA_VALIDATION)) != DATA_NOOUTPUT)
    ErrStopParsing (bf,
                    "Deployment-only data set lacks ReadNoOutput keyword. (You wouldn't need to deploy if you already had the answers).",
                    newdata);
  SkipToNext (bf, config);
  while (TokenAvailable (bf, "ToFile") || TokenAvailable (bf, "ToPipe"))
    {
      if (AcceptToken (bf, config, "ToFile"))
        newdata->flags |= DATA_WRITEFILE;
      else if (AcceptToken (bf, config, "ToPipe"))
        newdata->flags |= DATA_WRITEPIPE;
      if ((newdata->flags & DATA_WRITEPIPE) != 0
          && (newdata->flags & DATA_WRITEFILE) != 0)
        ErrStopParsing (bf,
                        "Both of keywords ToFile and ToPipe found.  Only one may be used.",
                        NULL);
      SkipToNext (bf, config);
      if (!ReadQuotedString (bf, config, &(newdata->outname)))
        ErrStopParsing (bf,
                        "ToPipe/ToFile keyword must be followed by a quoted string giving the name of a pipe or file.",
                        newdata);
      SkipToNext (bf, config);
    }

  if ((newdata->flags & DATA_DEPLOYMENT) != 0x0 && (newdata->outname == NULL))
    ErrStopParsing (bf,
                    "There would be no point in Deployment if we didn't need the answers. Use 'ToFile/ToPipe' to say where to send them.",
                    newdata);
  size_t casecount = 0;
  if ((newdata->flags & DATA_IMMEDIATE) != 0x0)
    while (ReadImmediateCase (bf, config, newdata, casecount++));
  newdata->entrycount = casecount;
  newdata->data =
    (flotype *) realloc (newdata->data,
                         casecount * sizeof (flotype) * (newdata->inputcount +
                                                         newdata->
                                                         outputcount));
  if (newdata->data == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Reallocation failure in ReadDataStatement.\n");
      exit (1);
    }
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf,
                    "Close Parenthesis expected at end of Data Statement.",
                    newdata);
  else
    net->data = newdata;
  return (1);
}


struct cases *
DeleteFirstData (struct cases *arg)
{
  if (arg == NULL)
    return (NULL);
  free (arg->inname);
  free (arg->outname);
  free (arg->data);             // not doing fcloses here as files are not yet open when this is called.
  struct cases *nxt = arg->next;
  free (arg);
  return nxt;
}

// Data section: Specifies inline data, or data locations (files and/or writable pipes).  Training data (readable inputs and outputs) Testing data (readable
// inputs and outputs) Validation data (readable inputs and outputs) Production input (readable inputs) Production output (writable outputs)
int
ReadDataSection (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (net != NULL);
  assert (config != NULL);
  assert (bf != NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "StartData"))
    return (0);
  else
    SkipToNext (bf, config);
  while (ReadDataStatement (bf, config, net))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "EndData"))
    {
      while (net->data != NULL)
        net->data = DeleteFirstData (net->data);
      ErrStopParsing (bf, "Expected a Data Statement or EndData", NULL);
    }
  return (1);
}


// Node definition statements, until 'EndNodes'
int
ReadNodeSection (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "StartNodes"))
    return (0);
  if (net->nodecount != 0)
    ErrStopParsing (bf,
                    "Only one Node Definition section is allowed in a configuration file.",
                    NULL);
  if (ReadCreateNodeStmt (bf, config, net))
    while (ReadCreateNodeStmt (bf, config, net));
  else
    ErrStopParsing (bf,
                    "No node definitions found. Expected 'CreateInput','CreateHidden' or 'CreateOutput'.",
                    NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "EndNodes"))
    ErrStopParsing (bf,
                    "Expected 'EndNodes' terminator after Node Definition section.",
                    NULL);
  if (net->nodecount == 0)
    ErrStopParsing (bf, "\nNo nodes created.", NULL);
  return (1);
}



/*int ReadTrOutFile(struct slidingbuffer *bf, struct conf *config, struct plans *pl){
    assert(bf != NULL); assert(config != NULL); assert(pl != NULL);
    if (!AcceptToken(bf, config, "OutputTo")) return(0);else SkipToNext(bf, config);
    if (ReadQuotedString(bf, config, &(pl->outputdest))) return(1);
    else ErrStopParsing(bf, "Expected the name of a file or pipe (in \"quotes\") to send network output to.", NULL);
    return(1);
    }*/
int
ReadTrReportFile (struct slidingbuffer *bf, struct conf *config,
                  struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "ReportTo"))
    return (0);
  else
    SkipToNext (bf, config);
  if (ReadQuotedString (bf, config, &(pl->reportdest)))
    return (1);
  else
    ErrStopParsing (bf,
                    "Expected the name of a file or pipe (in \"quotes\") to send the the training report to.",
                    NULL);
  return (1);
}

int
ReadTrMaxEpochs (struct slidingbuffer *bf, struct conf *config,
                 struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "MaxEpoch"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->epochmax = ReadInteger (bf, config);
  else
    ErrStopParsing (bf, "An integer number of epochs must follow 'MaxEpoch'.",
                    NULL);
  return (1);
}

int
ReadTrMinEpochs (struct slidingbuffer *bf, struct conf *config,
                 struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "MinEpoch"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->epochmin = ReadInteger (bf, config);
  else
    ErrStopParsing (bf, "An integer number of epochs must follow 'MinEpoch'.",
                    NULL);
  return (1);
}

int
ReadTrGoal (struct slidingbuffer *bf, struct conf *config, struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "TrainingGoal"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->goal = ReadFloatingPoint (bf, config);
  else
    ErrStopParsing (bf,
                    "A floating-point accuracy must follow 'TrainingGoal'.",
                    NULL);
  return (1);
}

int
ReadTrLearnRate (struct slidingbuffer *bf, struct conf *config,
                 struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "LearningRate"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->trainrate = ReadFloatingPoint (bf, config);
  else
    ErrStopParsing (bf,
                    "A floating-point learning rate must follow 'TrainingGoal'.",
                    NULL);
  return (1);
}

int
ReadTrBatchSize (struct slidingbuffer *bf, struct conf *config,
                 struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "BatchSize"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->batchsize = ReadInteger (bf, config);
  else
    ErrStopParsing (bf, "An integer number of cases must follow 'BatchSize'.",
                    NULL);
  return (1);
}

int
ReadTrEpochSize (struct slidingbuffer *bf, struct conf *config,
                 struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (!AcceptToken (bf, config, "Goal"))
    return (0);
  else
    SkipToNext (bf, config);
  if (NumberAvailable (bf))
    pl->epochsize = ReadInteger (bf, config);
  else
    ErrStopParsing (bf,
                    "A an integer number of batches must follow 'EpochSize'.",
                    NULL);
  return (1);
}

int
ReadGradientDescentArg (struct slidingbuffer *bf, struct conf *config,
                        struct plans *pl)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (pl != NULL);
  if (ReadTrBatchSize (bf, config, pl) || ReadTrLearnRate (bf, config, pl))
    return (1);
  return (0);
}


int
ReadTrainingPlan (struct slidingbuffer *bf, struct conf *config,
                  struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  struct plans buf;
  struct plans *ret = &buf;
  if (!AcceptToken (bf, config, "TrainingPlan"))
    return (0);
  else
    SkipToNext (bf, config);
  ret->planflags |= PLAN_TRAIN;
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf,
                    "'TrainingPlan' must be followed by an open parenthesis.",
                    NULL);
  else
    SkipToNext (bf, config);
  if (AcceptToken (bf, config, "GradientDescent"))
    {
      SkipToNext (bf, config);
      ret->batchsize = 1;
      ret->epochsize = PLAN_DEFAULT_EPOCHS;
      ret->goal = PLAN_DEFAULT_GOAL;
      ret->trainrate = PLAN_DEFAULT_RATE;
      ret->epochmax = PLAN_DEFAULT_MAXEP;
      ret->planflags |= PLAN_GRAD_DESCENT;
      while (ReadGradientDescentArg (bf, config, ret)
             || ReadTrMaxEpochs (bf, config, ret)
             || ReadTrMinEpochs (bf, config, ret)
             || ReadTrReportFile (bf, config, ret)
             || ReadTrGoal (bf, config, ret))
        SkipToNext (bf, config);
    }
  // else if (AcceptToken(bf, config, "Genetic")) etc...
  else
    ErrStopParsing (bf,
                    "Expected a training method to use. Methods available are: 'GradientDescent'.",
                    NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf,
                    "Expected 'TrainingGoal', 'LearningRate', 'BatchSize', 'EpochSize','ReportTo', 'MaxEpoch', 'MinEpoch', or closing parenthesis.",
                    NULL);
  ret = (struct plans *) malloc (sizeof (struct plans));
  if (ret == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure in ReadTrainingPlan.\n");
      exit (1);
    }
  memcpy ((void *) &buf, (void *) ret, sizeof (struct plans));
  ret->next = net->plan;
  net->plan = ret;
  return (1);
}

int
ReadTestingPlan (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  struct plans buf;
  struct plans *ret = &buf;
  if (!AcceptToken (bf, config, "TestingPlan"))
    return (0);
  else
    SkipToNext (bf, config);
  ret->planflags = PLAN_TEST;
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf,
                    "'TestingPlan' must be followed by an open parenthesis.",
                    NULL);
  else
    SkipToNext (bf, config);
  if (ReadTrReportFile (bf, config, ret))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf,
                    "Testing Plans may have 'ReportTo' arguments, or end with closing parenthesis.",
                    NULL);
  ret = (struct plans *) malloc (sizeof (struct plans));
  if (ret == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure in ReadTestingPlan.\n");
      exit (1);
    }
  memcpy ((void *) &buf, (void *) ret, sizeof (struct plans));
  ret->next = net->plan;
  net->plan = ret;
  return (1);
}

int
ReadValidationPlan (struct slidingbuffer *bf, struct conf *config,
                    struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  struct plans buf;
  struct plans *ret = &buf;
  if (!AcceptToken (bf, config, "ValidationPlan"))
    return (0);
  else
    SkipToNext (bf, config);
  ret->planflags |= PLAN_VALIDATE;
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf,
                    "'ValidationPlan' must be followed by an open parenthesis.",
                    NULL);
  else
    SkipToNext (bf, config);
  if (ReadTrReportFile (bf, config, ret))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf,
                    "Validation Plans may have 'ReportTo' arguments, or end with closing parenthesis.",
                    NULL);
  ret = (struct plans *) malloc (sizeof (struct plans));
  if (ret == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure in ReadValidationPlan.\n");
      exit (1);
    }
  memcpy ((void *) &bf, (void *) ret, sizeof (struct plans));
  ret->next = net->plan;
  net->plan = ret;
  return (1);
}

int
ReadDeploymentPlan (struct slidingbuffer *bf, struct conf *config,
                    struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  struct plans buf;
  struct plans *ret = &buf;
  if (!AcceptToken (bf, config, "DeploymentPlan"))
    return (0);
  else
    SkipToNext (bf, config);
  ret->planflags |= PLAN_DEPLOY;
  if (!AcceptToken (bf, config, "("))
    ErrStopParsing (bf,
                    "'DeploymentPlan' must be followed by an open parenthesis.",
                    NULL);
  else
    SkipToNext (bf, config);
  if (ReadTrReportFile (bf, config, ret))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, ")"))
    ErrStopParsing (bf,
                    "Deployment Plans may have 'ReportTo' arguments, and end with closing parenthesis.",
                    NULL);
  ret = (struct plans *) malloc (sizeof (struct plans));
  if (ret == NULL)
    {
      fprintf (stderr,
               "Runtime Error: Allocation failure in ReadDeploymentPlan.\n");
      exit (1);
    }
  memcpy ((void *) &bf, (void *) ret, sizeof (struct plans));
  ret->next = net->plan;
  net->plan = ret;
  return (1);
}

int
ReadPlanSection (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  if (!AcceptToken (bf, config, "StartPlan"))
    return (0);
  else
    SkipToNext (bf, config);
  while (ReadTrainingPlan (bf, config, net)
         || ReadTestingPlan (bf, config, net)
         || ReadValidationPlan (bf, config, net)
         || ReadDeploymentPlan (bf, config, net))
    SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "EndPlan"))
    ErrStopParsing (bf,
                    "Expected 'TestPlan', 'TrainingPlan', 'ValidationPlan', 'DeploymentPlan',  or 'EndPlan'.",
                    NULL);
  return (1);
}



// Check at end of connections and add warning messages for questionable topology.

int
ValidateConnections (struct slidingbuffer *bf, struct conf *config,
                     struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  int indexconn = 0;
  int indexnode = 1;
  char wstr[WARNSIZE];
  if ((config->flags & SILENCE_BIAS) == 0)
    {
      while (net->sources[indexconn] == 0 && indexconn++ < net->synapsecount);
      if (indexconn == net->synapsecount)
        AddWarning (bf,
                    "Warning:  Are you sure you wanted to define a network with no bias connections (connections with source 0)?");
      for (indexconn = 0;
           indexconn < net->synapsecount && net->dests[indexconn] != 0;
           indexconn++);
      if (indexconn != net->synapsecount)
        AddWarning (bf,
                    "Warning: Connections whose destination is the bias node (node zero) are being ignored.");
    }
  if ((config->flags & SILENCE_NODEOUTPUT) == 0)
    for (indexnode = 1; indexnode < net->nodecount - net->outputcount;
         indexnode++)
      {
        for (indexconn = 0;
             indexconn < net->synapsecount
             && net->sources[indexconn] != indexnode; indexconn++);
        if (indexconn == net->synapsecount)
          {
            if (indexnode <= net->inputcount)
              snprintf (wstr, WARNSIZE,
                        "Warning: node %d is an input node but does not send any signals.",
                        indexnode);
            else
              snprintf (wstr, WARNSIZE,
                        "Warning: node %d is a hidden node that does not send any signals.",
                        indexnode);
            AddWarning (bf, wstr);
          }
      }
  if ((config->flags & SILENCE_NODEINPUT) == 0)
    for (indexnode = net->inputcount + 1; indexnode < net->nodecount;
         indexnode++)
      {
        for (indexconn = 0;
             indexconn < net->synapsecount
             && net->dests[indexconn] != indexnode; indexconn++);
        if (indexconn == net->synapsecount)
          {
            if (indexnode >= net->nodecount - net->outputcount)
              snprintf (wstr, WARNSIZE,
                        "Warning: node %d is an output node but does not receive any signals.",
                        indexnode);
            else
              snprintf (wstr, WARNSIZE,
                        "Warning: node %d is a hidden node that does not receive any signals.",
                        indexnode);
            AddWarning (bf, wstr);
          }
      }
  if ((config->flags & SILENCE_OUTPUT) == 0 && net->outputcount == 0)
    AddWarning (bf, "Warning: the network as defined has no output nodes.");
  if ((config->flags & SILENCE_INPUT) == 0 && net->inputcount == 0)
    AddWarning (bf, "Warning: the network as defined has no input nodes.");
  if ((config->flags & SILENCE_MULTIACTIVATION) == 0
      || (config->flags & SILENCE_RECURRENCE) == 0)
    {
      int *checkbuf = calloc (net->nodecount, sizeof (int));
      // checkbuf meanings = 0: no use yet recorded.  1: recorded as dest only.  2: recorded as dest then source.  3: recorded as source only.  4: recorded
      // as source then dest. 5: recorded as dest then source then dest. 6: recorded as source then dest then source. 7: recorded as source / dest /
      // source / dest OR dest / source / dest / source (multiactivation)
      if (checkbuf != NULL)
        {
          for (indexconn = 0; indexconn < net->synapsecount; indexconn++)
            {
              switch (checkbuf[net->sources[indexconn]])
                {
                case 0:
                  checkbuf[net->dests[indexconn]] = 3;
                  break;
                case 1:
                  checkbuf[net->dests[indexconn]] = 2;
                  break;
                case 4:
                  checkbuf[net->dests[indexconn]] = 6;
                  break;
                case 5:
                  checkbuf[net->dests[indexconn]] = 7;
                  break;
                }
              switch (checkbuf[net->dests[indexconn]])
                {
                case 0:
                  checkbuf[net->dests[indexconn]] = 1;
                  break;
                case 2:
                  checkbuf[net->dests[indexconn]] = 5;
                  break;
                case 3:
                  checkbuf[net->dests[indexconn]] = 4;
                  break;
                case 6:
                  checkbuf[net->dests[indexconn]] = 7;
                  break;
                }
            }
          for (indexnode = 0; indexnode < net->nodecount; indexnode++)
            {
              if ((config->flags & SILENCE_MULTIACTIVATION) == 0
                  && (checkbuf[indexnode] == 7
                      || (checkbuf[indexnode] > 4
                          && indexnode >= net->nodecount - net->outputcount)))
                {
                  snprintf (wstr, WARNSIZE,
                            "Warning: node %d will be activated more than once per activation sequence.",
                            indexnode);
                  AddWarning (bf, wstr);
                }
              if ((config->flags & SILENCE_RECURRENCE) == 0
                  && checkbuf[indexnode] >= 4
                  && indexnode < net->nodecount - net->outputcount)
                {
                  snprintf (wstr, WARNSIZE,
                            "Warning: the network is recurrent because node %d saves signal across activation sequences.",
                            indexnode);
                  AddWarning (bf, wstr);
                }
            }
          free (checkbuf);
        }
    }
  return (1);                   // For now always returns 1.  TODO, maybe, a return code that indicates what topological reduction can be done on it.
}


// Connection statements, until 'EndConnections'
int
ReadConnections (struct slidingbuffer *bf, struct conf *config,
                 struct nnet *net)
{
  assert (bf != NULL);
  assert (config != NULL);
  assert (net != NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "StartConnections"))
    return (0);
  if (net->nodecount == 0)
    ErrStopParsing (bf,
                    "Found 'StartConnections', expected 'StartNodes.' Nodes cannot be connected before they are defined.",
                    NULL);
  SkipToNext (bf, config);
  if (ReadConnectStmt (bf, config, net))
    while (ReadConnectStmt (bf, config, net));
  else
    ErrStopParsing (bf, "No 'Connect' statement found.", NULL);
  SkipToNext (bf, config);
  if (!AcceptToken (bf, config, "EndConnections"))
    ErrStopParsing (bf,
                    "Expected 'Connect' statement or 'EndConnections' terminator.",
                    NULL);
  if (net->synapsecount == 0)
    ErrStopParsing (bf, "No connections created.", NULL);
  return (ValidateConnections (bf, config, net));
}

// prints low-level information about network suitable for automated analysis/modification via sed, etc.
void
debugnnet (struct nnet *net)
{
  assert (net != NULL);
  int count;
  printf ("nodecount %d, inputcount %d, outputcount %d\n", net->nodecount,
          net->inputcount, net->outputcount);
  printf ("Nodes\n");
  for (count = 0; count < net->nodecount; count++)
    {
      if (count == 0)
        printf ("   Bias Node 0: Always outputs one.\n");
      else if (count < net->inputcount)
        printf ("   Input ");
      else if (count < net->nodecount - net->outputcount)
        printf ("   Hidden ");
      else
        printf ("   Output ");
      if (count != 0)
        printf ("Node %d: accum %s, transfer %s, width %d\n", count,
                acctokens[net->accum[count]], outtokens[net->transfer[count]],
                net->transferwidths[count]);
    }
  printf ("Connections\n");
  for (count = 0; count < net->synapsecount; count++)
    printf ("#%d:%d(x" FLOFMT3 ")->%d  ", count, net->sources[count],
            net->weights[count], net->dests[count]);
  printf ("\n");
}


void
nnetparser (struct nnet *net, struct conf *cfg, struct slidingbuffer *in)
{
  assert (net != NULL);
  assert (cfg != NULL);
  assert (in != NULL);
  ReadOpeningComment (in, cfg);
  SkipToNext (in, cfg);
  if (ReadConfigSection (in, cfg, net) || ReadNodeSection (in, cfg, net)
      || ReadConnections (in, cfg, net) || ReadPlanSection (in, cfg, net)
      || ReadDataSection (in, cfg, net))
    while (ReadConfigSection (in, cfg, net) || ReadNodeSection (in, cfg, net)
           || ReadConnections (in, cfg, net) || ReadPlanSection (in, cfg, net)
           || ReadDataSection (in, cfg, net))
      SkipToNext (in, cfg);
  else
    ErrStopParsing (in,
                    "Expected 'StartConfig', 'StartNodes', 'StartConnections', 'StartPlan', or 'StartData' statement.",
                    NULL);
  if (net->nodecount == 0)
    ErrStopParsing (in,
                    "No nodes defined. A nonempty 'StartNodes' section is needed.",
                    NULL);
  if (net->synapsecount == 0)
    ErrStopParsing (in,
                    "No connections defined. A nonempty 'StartConnections' section is needed.",
                    NULL);
  printf ("\n");
}
