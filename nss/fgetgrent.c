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
#include <grp.h>
#include <libc-lock.h>
#include <stdio.h>
#include <stdlib.h>
#include <set-freeres.h>


/* We need to protect the dynamic buffer handling.  */
__libc_lock_define_initialized (static, lock);

static char *buffer;

/* Read one entry from the given stream.  */
struct group *
fgetgrent (FILE *stream)
{
  static size_t buffer_size;
  static struct group resbuf;
  fpos_t pos;
  struct group *result;
  int save;

  if (__builtin_expect (fgetpos (stream, &pos), 0) != 0)
    return NULL;

  /* Get lock.  */
  __libc_lock_lock (lock);

  /* Allocate buffer if not yet available.  */
  if (buffer == NULL)
    {
      buffer_size = NSS_BUFLEN_GROUP;
      buffer = malloc (buffer_size);
    }

  while (buffer != NULL
	 && (__fgetgrent_r (stream, &resbuf, buffer, buffer_size, &result)
	     == ERANGE))
    {
      char *new_buf;
      buffer_size += NSS_BUFLEN_GROUP;
      new_buf = realloc (buffer, buffer_size);
      if (__glibc_unlikely (new_buf == NULL))
	{
	  /* We are out of memory.  Free the current buffer so that the
	     process gets a chance for a normal termination.  */
	  save = errno;
	  free (buffer);
	  __set_errno (save);
	}
      buffer = new_buf;

      /* Reset the stream.  */
      if (fsetpos (stream, &pos) != 0)
	buffer = NULL;
    }

  if (buffer == NULL)
    result = NULL;

  /* Release lock.  Preserve error value.  */
  save = errno;
  __libc_lock_unlock (lock);
  __set_errno (save);

  return result;
}

weak_alias (buffer, __libc_fgetgrent_freemem_ptr)
