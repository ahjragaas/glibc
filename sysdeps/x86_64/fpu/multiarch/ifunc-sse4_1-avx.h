/* Common definition for ifunc selections optimized with SSE4.1 and AVX.
   Copyright (C) 2025 Free Software Foundation, Inc.
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

#include <init-arch.h>

extern __typeof (REDIRECT_NAME) OPTIMIZE (sse2) attribute_hidden;
extern __typeof (REDIRECT_NAME) OPTIMIZE (sse41) attribute_hidden;
extern __typeof (REDIRECT_NAME) OPTIMIZE (avx) attribute_hidden;

static inline void *
IFUNC_SELECTOR (void)
{
  const struct cpu_features* cpu_features = __get_cpu_features ();

  if (CPU_FEATURE_USABLE_P (cpu_features, AVX))
    return OPTIMIZE (avx);

#if MINIMUM_X86_ISA_LEVEL == SSE4_1_X86_ISA_LEVEL
  return OPTIMIZE (sse41);
#else
  if (CPU_FEATURE_USABLE_P (cpu_features, SSE4_1))
    return OPTIMIZE (sse41);

  return OPTIMIZE (sse2);
#endif
}
