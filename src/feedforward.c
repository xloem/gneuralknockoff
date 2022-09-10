/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>
   Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

   This file is part of Gneural Knockoff.

   Gneural Knockoff is free software; you can redistribute it and/or modify it
   under the terms of the GNU Affero General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any later
   version.

   Gneural Knockoff is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
   License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with Gneural Knockoff.  If not, see <http://www.gnu.org/licenses/>.
*/

// feedforward propagation of information through the neural network

#include "feedforward.h"
#include "includes.h"
#include "defines.h"
#include "binom.h"
#include "fact.h"
#include "activation.h"

#define PI M_PI

void
feedforward (network * nn)
{
  register int i, j;
  register int l, n;

  /*
   * scan all the layers (execpt the first)
   */
  for (l = 1; l < nn->num_of_layers; l++)
    {
      /*
       * scan all the neurons in layer 'l'
       */
      for (n = 0; n < nn->layers[l].num_of_neurons; n++)
        {

          neuron *ne = &nn->layers[l].neurons[n];

          double x = 0.;
          double tmp;

          /* fmv no more required */
          //for (i=0;i<NEURON[id].nw;i++) NEURON[id].x[i]=NEURON[NEURON[id].connection[i]].output;
          switch (ne->accumulator)
            {
            case LINEAR:
              for (i = 0; i < ne->num_input; i++)
                x += ne->connection[i]->output * ne->w[i];      // linear product between w[] and x[]
              break;
            case LEGENDRE:
              for (i = 0; i < ne->num_input; i++)
                {
                  for (tmp = 0., j = 0; j <= i; j++)
                    tmp +=
                      pow (ne->connection[i]->output, j) * binom (i,
                                                                  j) *
                      binom ((i + j - 1) / 2, j);
                  tmp *= pow (2, i) * ne->w[i];
                  x += tmp;
                }
              break;
            case LAGUERRE:
              for (i = 0; i < ne->num_input; i++)
                {
                  for (tmp = 0., j = 0; j <= i; j++)
                    tmp +=
                      binom (i, j) * pow (ne->connection[i]->output,
                                          j) * pow (-1, j) / fact (j);
                  tmp *= ne->w[i];
                  x += tmp;
                }
              break;
            case FOURIER:
              for (i = 0; i < ne->num_input; i++)
                {
                  for (tmp = 0., j = 0; j <= i; j++)
                    tmp += sin (2. * j * PI * ne->connection[i]->output);
                  tmp *= ne->w[i];
                  x += tmp;
                }
              break;
            default:
              break;
            }
          ne->output = activation (ne->activation, x);
        }
    }
}


// input combining functions. Some widely used, some just plain strange.  Routine by Ray D. 6 September 2016
flotype
combine (const int combiner, const flotype currentval, const flotype ad)
{
  switch (combiner)
    {
    case 0:
      return currentval;        // Null combiner.  Ignores all inputs.
    case 1:
      return currentval + ad;   // sigma - for "normal" networks.
    case 2:
      return currentval * ad;   // pi - useful for LSTM, etc.
    case 3:
      return currentval + (ad / (1 + absolute (ad)));   // softlimit - for ignoring outliers
    case 4:
      return currentval + absolute (ad);        // measures magnitude only.
    case 5:
      return currentval + (ad > 0 ? logarithm (ad + 1) : -logarithm (absolute (ad) + 1));       // softlog - for discounting outliers
    case 6:
      return currentval > ad ? currentval : ad; // max - for join layers
    case 7:
      return ad > 0 ? currentval + ad : currentval;     // ignore negative inputs
    default:
      fprintf (stderr, "unknown combination function\n");
      exit (1);
    }
}

// return the identity element for the combiner - same numeric aliases for combiners as in the fn above. Routine by Ray
// D. 6 September 2016
static inline flotype
identity (const int combiner)
{
  return (combiner == 1) ? ONE : ZERO;
}

// initialize an activation vector for use by a network. Routine by Ray D. 6 September 2016
void
init_activations (const struct nnet *const net, flotype * vec)
{
#pragma omp parallel for
  for (size_t pos = 0; pos < net->nodecount; pos++)
    vec[pos] = identity (net->transfer[pos]);
}

// most of the popular transfer functions, and a few deliberate peculiarities, vectorized for OMP.
// Routine by Ray D. 6 September 2016
void
transfer (int fchoice, double *ins, double *outs, size_t width)
{
  switch (fchoice)
    {
    case 0:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = ins[count];
      break;                    // identity
    case 1:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = hypertan (ins[count]);
      break;                    // tanh sigmoid
    case 2:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = arctan (ins[count]);
      break;                    // arctangent sigmoid
    case 3:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = ONE / (ONE + exponential (-ins[count]));
      break;                    // unsigned logistic sigmoid
    case 4:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = (TWO / (ONE + exp (-ins[count]))) - ONE;
      break;                    // signed logistic sigmoid
    case 5:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = ins[count] / (ONE + absolute (ins[count]));
      break;                    // softsign sigmoid
    case 6:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)    // mirrored logarithmic transfer
        outs[count] =
          ins[count] >
          ZERO ? logarithm (absolute (ins[count] + ONE)) :
          -logarithm (absolute (-ins[count] - ONE));
      break;
    case 7:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = ins[count] > ZERO ? ONE : -ONE;
      break;                    // signed step function
    case 8:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = ins[count] > ZERO ? ins[count] : ZERO;
      break;                    // rectified linear unit
    case 9:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)
        outs[count] = logarithm (ONE + exponential (ins[count]));
      break;                    // softplus rectifier
    case 10:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)    // logarithmic rectifier - mimics spike freq. in biological networks.
        outs[count] = ins[count] >= ONE ? logarithm (ins[count]) : ZERO;
    case 11:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)    // sinusoid Radial Bias Function
        outs[count] = cosine (ins[count]);
      break;
    case 12:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)    // gaussian Radial Bias Function
        outs[count] = exponential (-ins[count] * ins[count]);
      break;
    case 13:
#pragma omp parallel for
      for (size_t count = 0; count < width; count++)    // thin plate spline Radial Bias Function
        outs[count] = (ins[count] * ins[count]) * logarithm (ins[count]);
      break;
      // Note: Activation functions below this point operate on multiple nodes. This is an experimental capability.
    case 14:
#pragma omp parallel for
      for (size_t count = 1; count < width; count++)    // multiplication by first input.
        outs[count] = ins[count] * ins[0];
      outs[0] = 0;
      break;
    case 15:
#pragma omp parallel for
      for (size_t count = 0; count < (width - 1); count += 2)
        {                       // parallel pairwise addition & multiplication.
          outs[count] = ins[count] * ins[count + 1];
          outs[count + 1] = ins[count] + ins[count + 1];
        }
      break;
    default:
      fprintf (stderr, "unknown transfer function\n");
      exit (1);
    }
}


// fwdprop: The second argument is a pointer to a vector of inputs at least as long as the network's inputcount. The
// third is a pointer to a vector of activation values at least as long as the network's nodecount. Reuse the activation
// vector on subsequent calls for recurrent networks, otherwise be sure to initialize it to identity elements before the
// call. The fourth argument is a pointer to a vector the same length as activation for recording the activation levels
// at the time of firing. It's needed later for backprop, etc, but if you don't need it for training (ie, in production
// or when training by other methods) you can leave it NULL. The last argument is a pointer to a vector of doubles at
// least as long as net->outputcount, which will get filled with the network output.

// Routine by Ray D, 31 Aug 2016.

// Handles recurrent networks. Saves history if desired so we can later do backprop.  Handles combinators varying by
// node.  Handles transfer functions varying by node.  Handles transfer functions of differing widths.

void
fwdprop (const struct nnet *const net, const flotype * const inputs,
         flotype * const activations, flotype * const history,
         flotype * const outputs)
{
  size_t wcount = 0;
  size_t nodecount = 0;
  flotype *res =
    history != NULL ? history : alloca (sizeof (flotype) * net->nodecount);
  res[nodecount++] = ONE;       // bias.
#pragma omp parallel for
  for (size_t incount = nodecount; incount <= net->inputcount; incount++)       // process inputs.
    activations[incount] =
      combine (net->accum[incount], activations[incount],
               inputs[incount - 1]);
  for (wcount = 0; wcount < net->synapsecount; wcount++)
    {                           // process connections.
      // perform transfer function for all nodes up to and including that required by current connection.
      for (; nodecount <= net->sources[wcount];
           nodecount += net->transferwidths[nodecount])
        {
          transfer (net->transfer[nodecount], &(activations[nodecount]),
                    &(res[nodecount]), net->transferwidths[nodecount]);
          // reset nodes whose transfers have run so recurrent transfers start from the identity element for their accumulator.
#pragma omp parallel for
          for (size_t resetcount = nodecount;
               resetcount <= nodecount + net->transferwidths[nodecount];
               resetcount++)
            {
              // capture activation history if history vector is provided. many training methods need it.
              activations[resetcount] = identity (net->accum[resetcount]);
            }
        }
      activations[net->dests[wcount]] =
        combine (net->accum[net->dests[wcount]],
                 activations[net->dests[wcount]],
                 res[net->sources[wcount]] * net->weights[wcount]);
    }
  // process transfer functions for any nodes following last weight source to be sure we get outputs for all output nodes.
  for (; nodecount < net->nodecount; nodecount++)
    {
      transfer (net->nodecount, &(activations[nodecount]), &(res[nodecount]),
                net->transferwidths[nodecount]);
#pragma omp parallel for
      for (size_t resetcount = nodecount;
           resetcount <= nodecount + net->transferwidths[nodecount];
           resetcount++)
        activations[resetcount] = identity (net->accum[resetcount]);
    }
  memcpy (&(res[0]), &(outputs[net->nodecount - net->outputcount - 1]), sizeof (flotype) * net->outputcount);   // send outputs to res
}
