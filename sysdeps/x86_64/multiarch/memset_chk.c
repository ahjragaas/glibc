/* Multiple versions of __memset_chk
   All versions must be listed in ifunc-impl-list.c.
   Copyright (C) 2017-2025 Free Software Foundation, Inc.
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

/* Define multiple versions only for the definition in libc.so. */
#if IS_IN (libc) && defined SHARED
# define __memset_chk __redirect_memset_chk
# include <string.h>
# undef __memset_chk

# define SYMBOL_NAME memset_chk
# include "ifunc-memset.h"

libc_ifunc_redirected (__redirect_memset_chk, __memset_chk,
		       IFUNC_SELECTOR ());
# ifdef SHARED
__hidden_ver1 (__memset_chk, __GI___memset_chk, __redirect_memset_chk)
  __attribute__ ((visibility ("hidden"))) __attribute_copy__ (__memset_chk);
# endif
#endif
