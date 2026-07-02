/* Module for tst-malloc-ifunc-resolver, loaded into a second namespace.
   Copyright (C) 2026 Free Software Foundation, Inc.
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

#include <dlfcn.h>
#include <stdlib.h>

/* Called from tst-malloc-ifunc-resolver after this module has been loaded
   with dlmopen into a fresh namespace. */

int
resolve_malloc_via_dlsym (void)
{
  void *(*malloc_p) (size_t) = (void *(*) (size_t)) dlsym (RTLD_DEFAULT,
							  "malloc");
  if (malloc_p == NULL)
    return 0;
  void *p = malloc_p (48);
  int ok = p != NULL;
  free (p);
  return ok;
}
