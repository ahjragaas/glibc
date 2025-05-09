/* Implement __kernel_standard_f.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.
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

#include <math.h>
#include <math-svid-compat.h>


#if LIBM_SVID_COMPAT
/* Handle errors for a libm function as specified by TYPE (see
   comments in k_standard.c for details), with arguments X and Y,
   returning the appropriate return value for that function.  */

float
__kernel_standard_f (float x, float y, int type)
{
  return __kernel_standard (x, y, type);
}
#endif
