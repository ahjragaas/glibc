/* Disable floating-point exceptions.
   Copyright (C) 2000-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#include <fenv.h>

int
fedisableexcept (int excepts)
{
  union { unsigned long long l; unsigned int sw[2]; } s;
  unsigned int old_exc;

  /* Get the current status word. */
  __asm__ ("fstd %%fr0,0(%1)" : "=m" (s.l) : "r" (&s.l) : "%r0");

  old_exc = s.sw[0] & FE_ALL_EXCEPT;

  s.sw[0] &= ~(excepts & FE_ALL_EXCEPT);
  __asm__ ("fldd 0(%0),%%fr0" : : "r" (&s.l), "m" (s.l) : "%r0");

  return old_exc;
}
