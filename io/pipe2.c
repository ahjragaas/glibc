/* Copyright (C) 1991-2025 Free Software Foundation, Inc.
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

#include <errno.h>
#include <unistd.h>
#include <stddef.h>

/* Create a one-way communication channel (__pipe).  If successful,
   two file descriptors are stored in PIPEDES; bytes written on
   PIPEDES[1] can be read from PIPEDES[0].  Apply FLAGS to the new
   file descriptors.  Returns 0 if successful, -1 if not.  */
int
__pipe2 (int pipedes[2], int flags)
{
  if (pipedes == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  __set_errno (ENOSYS);
  return -1;
}
weak_alias (__pipe2, pipe2)
stub_warning (pipe2)
