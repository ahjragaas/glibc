/* Get integer exponent of a floating-point value.  LoongArch version.
   Copyright (C) 2022-2025 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <fpu_control.h>

static inline RET_TYPE
IMPL_NAME (float x)
{
  int x_cond;
  asm volatile ("fclass.s \t%0, %1" : "=f" (x_cond) : "f" (x));

  if (__glibc_unlikely (x_cond & _FCLASS_ZERO))
    return RET_INVALID (RET_LOGB0);
  else if (__glibc_unlikely (x_cond & ( _FCLASS_NAN | _FCLASS_INF)))
    return RET_INVALID (RET_LOGBNAN);
  else
    {
      asm volatile ("fabs.s \t%0, %1" : "=f" (x) : "f" (x));
      asm volatile ("flogb.s \t%0, %1" : "=f" (x) : "f" (x));
      return x;
    }
}
