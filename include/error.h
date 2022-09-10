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

#ifndef ERROR_H
#define ERROR_H
#include "network.h"

typedef struct _network network;
typedef struct _network_config network_config;

double error (network *, network_config *);

#endif
