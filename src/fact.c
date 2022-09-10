/*
   Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
   Copyright (C) 2016-2017 Jean Michel Sellier <jeanmichel.sellier@gmail.com>

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

// returns the factorial of a given integer

// originally written by : Jean Michel Sellier
// improved by           : Dmytriy Tereschenko

#include "fact.h"

inline uint64_t
fact (uint32_t n)
{
  uint64_t res = 1;

  while (n)
    {
      res *= n;
      --n;
    }
  return (res);
}
