/* Default implementation of pwritev2 (LFS version).
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

#include <errno.h>
#include <sys/uio.h>

/* Same as preadv64 but with an additional flag argument.  */
ssize_t
pwritev64v2 (int fd, const struct iovec *vector, int count, off64_t offset,
	     int flags)
{
  __set_errno (ENOSYS);
  return -1;
}

stub_warning (pwritev64v2)
