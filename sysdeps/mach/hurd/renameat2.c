/* Rename a file using relative source and destination names.  Hurd version.
   Copyright (C) 1991-2025 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <hurd.h>
#include <hurd/fd.h>

/* Rename the file OLD relative to OLDFD to NEW relative to NEWFD.  */
int
__renameat2 (int oldfd, const char *old, int newfd, const char *new,
             unsigned int flags)
{
  error_t err;
  file_t olddir, newdir;
  const char *oldname, *newname;
  int excl = 0;

  if ((flags & (RENAME_EXCHANGE | RENAME_NOREPLACE)) == (RENAME_EXCHANGE | RENAME_NOREPLACE))
    return __hurd_fail (EINVAL);
  if (flags & (RENAME_EXCHANGE | RENAME_WHITEOUT))
    return __hurd_fail (ENOSYS);
  if (flags & RENAME_NOREPLACE)
    excl = 1;

  olddir = __file_name_split_at (oldfd, old, (char **) &oldname);
  if (olddir == MACH_PORT_NULL)
    return -1;
  if (!*oldname)
    {
      /* Trailing slash.  */
      __mach_port_deallocate (__mach_task_self (), olddir);
      return __hurd_fail (ENOTDIR);
    }
  newdir = __file_name_split_at (newfd, new, (char **) &newname);
  if (newdir == MACH_PORT_NULL)
    {
      __mach_port_deallocate (__mach_task_self (), olddir);
      return -1;
    }
  if (!*newname)
    {
      /* Trailing slash.  */
      __mach_port_deallocate (__mach_task_self (), olddir);
      __mach_port_deallocate (__mach_task_self (), newdir);
      return __hurd_fail (ENOTDIR);
    }

  err = __dir_rename (olddir, oldname, newdir, newname, excl);
  __mach_port_deallocate (__mach_task_self (), olddir);
  __mach_port_deallocate (__mach_task_self (), newdir);
  if (err)
    return __hurd_fail (err);
  return 0;
}
libc_hidden_def (__renameat2)
weak_alias (__renameat2, renameat2)
