/* Processor capability information handling macros - aarch64 version.
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

#ifndef _DL_PROCINFO_H
#define _DL_PROCINFO_H	1

#include <sys/auxv.h>
#include <unistd.h>
#include <ldsodefs.h>
#include <sysdep.h>

/* We cannot provide a general printing function.  */
#define _dl_procinfo(type, word) -1

static inline const char *
__attribute__ ((unused))
_dl_hwcap_string (int idx)
{
  return (unsigned)idx < _DL_HWCAP_COUNT ? GLRO(dl_aarch64_cap_flags)[idx] : "";
};

#endif /* dl-procinfo.h */
