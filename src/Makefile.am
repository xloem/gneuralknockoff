AM_CFLAGS = -Wall -O3 -fopenmp -I../include
AM_LDFLAGS =

bin_PROGRAMS = gneural_network nnet
gneural_network_SOURCES = activation.c error.c feedforward.c gneural_network.c load.c network.c randomize.c rnd.c   \
simulated_annealing.c binom.c fact.c genetic_algorithm.c gradient_descent.c msmco.c parser.c random_search.c save.c


nnet_SOURCES = activation.c error.c feedforward.c load.c network.c nnet.c randomize.c rnd.c		    \
simulated_annealing.c binom.c fact.c genetic_algorithm.c gradient_descent.c msmco.c parser.c random_search.c save.c

gneural_network_LDADD = -lm
nnet_LDADD = -lm
