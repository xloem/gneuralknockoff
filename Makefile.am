AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CFLAGS = -Wall -O3 -fopenmp -I$(top_srcdir)/include
AM_LDFLAGS =

bin_PROGRAMS = gneural_network nnet

noinst_HEADERS = \
  include/includes.h \
  include/defines.h \
  include/load.h \
  include/activation.h \
  include/error.h \
  include/feedforward.h \
  include/network.h \
  include/randomize.h \
  include/rnd.h \
  include/simulated_annealing.h \
  include/binom.h \
  include/fact.h \
  include/genetic_algorithm.h \
  include/gradient_descent.h \
  include/msmco.h \
  include/parser.h \
  include/random_search.h \
  include/save.h

gneural_network_SOURCES = \
  src/activation.c \
  src/error.c \
  src/feedforward.c \
  src/gneural_network.c \
  src/load.c \
  src/network.c \
  src/randomize.c \
  src/rnd.c \
  src/simulated_annealing.c \
  src/binom.c \
  src/fact.c \
  src/genetic_algorithm.c \
  src/gradient_descent.c \
  src/msmco.c \
  src/parser.c \
  src/random_search.c \
  src/save.c

nnet_SOURCES = \
  src/activation.c \
  src/error.c \
  src/feedforward.c \
  src/load.c \
  src/network.c \
  src/nnet.c \
  src/randomize.c \
  src/rnd.c \
  src/simulated_annealing.c \
  src/binom.c \
  src/fact.c \
  src/genetic_algorithm.c \
  src/gradient_descent.c \
  src/msmco.c \
  src/parser.c \
  src/random_search.c \
  src/save.c

gneural_network_LDADD = -lm
nnet_LDADD = -lm
