/* Function erf vectorized with AVX-512, wrapper to AVX2.
   Copyright (C) 2021-2025 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include "svml_d_wrapper_impl.h"

	.section .text.evex512, "ax", @progbits
ENTRY (_ZGVeN8v_erf)
WRAPPER_IMPL_AVX512 _ZGVdN4v_erf
END (_ZGVeN8v_erf)
