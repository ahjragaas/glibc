/* Obtain the configured signal mask from a POSIX thread attribute.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.
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

#include <pthreadP.h>

int
__pthread_attr_getsigmask_np (const pthread_attr_t *attr, sigset_t *sigmask)
{
  struct pthread_attr *iattr = (struct pthread_attr *) attr;

  if (iattr->extension == NULL || !iattr->extension->sigmask_set)
    {
      __sigemptyset (sigmask);
      return PTHREAD_ATTR_NO_SIGMASK_NP;
    }
  else
    {
      *sigmask = iattr->extension->sigmask;
      return 0;
    }
}
libc_hidden_def (__pthread_attr_getsigmask_np)
weak_alias (__pthread_attr_getsigmask_np, pthread_attr_getsigmask_np)
