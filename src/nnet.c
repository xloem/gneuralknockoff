/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Ray Dillinger <bear@sonic.net>

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




#include "includes.h"
#include "defines.h"
#include "parser.h"
#include "save.h"

#define HELPSTRING  "usage: nnet <filename> | nnet -v | nnet -h | nnet -H | nnet -l \nOptions:\n\
  -h, -?, --help:  print this help and exit.\n\
  -H, --manpage:   print a manual fully describing nnet.\n\
  -l, --language:  print a manual describing the nnet script language.\n\
  -v, --version:   version and copyright information.\n"

int
HandleOptions (int argc, char **argv)
{                               // name, args, NULL, returnval
  static struct option options[] = {
    {"help", no_argument, NULL, 'h'}, {"version", no_argument, NULL, 'v'},
    {"manpage", no_argument, NULL, 'H'}, {"language", no_argument, NULL, 'l'},
    {"?", no_argument, NULL, '?'}, {0, 0, 0, 0}
  };
  int opt;
  opterr = 0;
  while ((opt = getopt_long (argc, argv, "hvHl", options, NULL)) != -1)
    switch (opt)
      {
      case 'v':
        printf
          ("nnet 0.0.1\nCopyright(C) 2016-2017 gneural_network developers\nLicense LGPLv3+. For information about copying, modifying, "
           "and distribution see <http://gnu.org/licenses/lgpl.html>.\n");
        return (0);
      case 'H':
        printf (MANPAGE);
        return (0);
      case 'l':
        printf (LANGMAN);
        return (0);
      default:
        printf (HELPSTRING);
        return (0);
      }
  return (1);
}

// objectives: establish name for reading and default nonserialized name for writing.  readname should be arg (plus .nnet extension if .nnet extension isn't
// already present) config->savename should be arg (without serial number and without .nnet extension), plus .out extension.  serial number if present in
// filename should be saved in config, else zero should be saved in config.
void
GetFileNames (char *readname, struct conf *config, const char *arg)
{
  config->flags |= SAVE_DEFAULT;
  size_t index = 0;
  size_t serialstart = 0;
  size_t serialend = 0;
  size_t serial = 0;
  size_t len = strlen (arg);
  for (index = 0; serialend == 0 && index < len && index < 255; index++)
    {
      if (arg[index] == '.' && serialstart == 0)
        serialstart = index + 1;
      else if (isdigit (arg[index]) && serialstart != 0 && serialend == 0)
        {
          serial *= 10;
          serial += arg[index] - '0';
          if (arg[index + 1] == '.' || arg[index + 1] == 0)
            serialend = index;
        }
      else if (serialstart != 0 && serialend == 0)
        {
          serialstart = 0;
          serial = 0;
        }
    }
  config->serialnum = serial;
  config->savename = malloc (sizeof (char) * (len + 25));
  if (config->savename == NULL)
    {
      fprintf (stderr, "allocation failure atempting to save filename.\n");
      exit (1);
    }
  strcpy (readname, arg);
  if (strcmp (".nnet", &(arg[len - 5])) == 0)
    readname[len - 5] = 0;
  strcat (readname, ".nnet");
  len = strlen (readname) - 5;
  if (serialstart != 0)
    {
      for (index = 0; index < serialstart; index++)
        config->savename[index] = readname[index];
      for (index = 1; serialend + index < len; index++)
        config->savename[serialend + index] = readname[index];
      config->savename[serialend + index + 1] = 0;
    }
  else
    {
      strcpy (config->savename, readname);
      config->savename[len] = 0;
    }
  strcat (config->savename, ".out");
}


// if not serializing, filename = config->savename.
// if serializing & there is a dot in the filename, last dot in filename is replaced with dot,++serial,dot
// if serializing & there is no dot in the filename, the filename is extended with dot-serial.
// if serializing, increment serial number in config.
// filename is a 256-char buffer.
void
NameOutputFile (char *filename, struct conf *config)
{
  size_t lastdot = 0;
  size_t len = strlen (config->savename);
  size_t index = len;
  size_t writeindex = 0;
  if ((config->flags & SAVE_SERIALIZE) == 0)
    {
      strncpy (filename, config->savename, 255);
      return;
    }
  for (index = 1; index <= len && config->savename[len - index] != '.';
       index++);
  lastdot = (config->savename[len - index] == '.') ? len - index : len;
  for (index = 0; (index <= len) && writeindex < 256; index++)
    if (index != lastdot)
      {
        filename[writeindex++] = config->savename[index];
        if (index + 1 == len && lastdot == len)
          writeindex +=
            snprintf (&(filename[writeindex]), 255 - index, ".%d",
                      ++(config->serialnum));
      }
    else
      writeindex +=
        snprintf (&(filename[writeindex]), 255 - index, ".%d.",
                  ++(config->serialnum));
  filename[writeindex] = 0;
}

int
main (int argc, char **argv)
{
  struct nnet newt;
  struct conf netconf;
  struct slidingbuffer bf;
  char fname[256];
  fname[0] = 0;
  char *filename = &(fname[0]);
  if (!HandleOptions (argc, argv))
    exit (0);
  if (argc > 2)
    fprintf (stderr,
             "%s does not process more than one nnet script in a single invocation.\n",
             argv[0]);
  if (argc != 2)
    {
      fprintf (stderr,
               "Usage:  %s [filename] where 'filename.nnet' is the name of a nnet script.\n",
               argv[0]);
      exit (1);
    }
  memset (&newt, 0, sizeof (struct nnet));
  memset (&bf, 0, sizeof (struct slidingbuffer));
  memset (&netconf, 0, sizeof (struct conf));
  GetFileNames (filename, &netconf, argv[1]);
  if (NULL == (bf.input = fopen (filename, "r")))
    {
      fprintf (stderr, "unable to open %s\n", filename);
      exit (1);
    }
  nnetparser (&newt, &netconf, &bf);
  fflush (stdout);
  PrintWarnings (&bf);
  if ((netconf.flags & SILENCE_DEBUG) != 0)
    {
      printf ("\n\nParse successful.");
      printf ("\n%d nodes created. Node 0 is a bias node.  ", newt.nodecount);
      if (newt.inputcount > 0)
        printf ("Nodes {1 %d} are input nodes.  ", newt.inputcount - 1);
      if (newt.nodecount - newt.inputcount - newt.outputcount > 0)
        printf (" Nodes {%d %d} are hidden nodes.", newt.inputcount,
                (newt.nodecount - 1) - newt.outputcount);
      if (newt.outputcount > 0)
        printf (" Nodes {%d %d} are output nodes. ",
                newt.nodecount - newt.outputcount, (newt.nodecount - 1));
      printf ("\n%d Connections created. ", newt.synapsecount);
      for (int syn = 0; syn < newt.synapsecount; syn++)
        printf ("%d=%d->%d,", syn, newt.sources[syn], newt.dests[syn]);
      printf ("\b \n");
    }
  fclose (bf.input);
  bf.input = NULL;
  NameOutputFile (filename, &netconf);
  FILE *outf = fopen (filename, "w");
  if (outf == NULL)
    {
      fprintf (stderr, "unable to open %s", filename);
      exit (1);
    }
  if ((netconf.flags & SILENCE_DEBUG) != 0)
    debugnnet (&newt);
  nnetwriter (&newt, &netconf, outf);
  fclose (outf);
}
