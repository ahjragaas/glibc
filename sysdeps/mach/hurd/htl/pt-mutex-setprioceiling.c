/* pthread_mutex_setprioceiling.  Hurd version.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
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
   License along with the GNU C Library;  if not, see
   <https://www.gnu.org/licenses/>.  */

#include <pthreadP.h>
#include <stdlib.h>
#include <assert.h>
#include <pt-internal.h>
#include "pt-mutex.h"
#include <hurdlock.h>
#include <shlib-compat.h>

int
__pthread_mutex_setprioceiling (pthread_mutex_t *mtxp, int cl, int *prp)
{
  (void) mtxp;
  (void) cl;
  (void) prp;
  return ENOSYS;
}
libc_hidden_def (__pthread_mutex_setprioceiling)
versioned_symbol (libc, __pthread_mutex_setprioceiling, pthread_mutex_setprioceiling, GLIBC_2_42);

#if OTHER_SHLIB_COMPAT (libpthread, GLIBC_2_12, GLIBC_2_42)
compat_symbol (libpthread, __pthread_mutex_setprioceiling,pthread_mutex_setprioceiling, GLIBC_2_12);
#endif
stub_warning (pthread_mutex_setprioceiling)
