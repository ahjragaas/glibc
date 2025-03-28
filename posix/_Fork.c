/* _Fork implementation.  Generic version.
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

#include <errno.h>
#include <unistd.h>

/* Clone the calling process, creating an exact copy.  Return -1 for errors,
   0 to the new process, and the process ID of the new process to the
   old process.
   Different than fork, this functions is marked as async-signal-safe by
   POSIX (by Austin Group issue 62).  */
pid_t
_Fork (void)
{
  __set_errno (ENOSYS);
  return -1;
}
libc_hidden_def (_Fork)
stub_warning (_Fork)
