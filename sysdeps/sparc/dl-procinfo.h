/* Linux/sparc version of processor capability information handling macros.
   Copyright (C) 1999-2024 Free Software Foundation, Inc.
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

#include <ldsodefs.h>
#include <sysdep.h>

#define _DL_HWCAP_COUNT 28

static inline int
__attribute__ ((unused))
_dl_procinfo (unsigned int type, unsigned long int word)
{
  int i;

  /* Fallback to generic output mechanism.  */
  if (type != AT_HWCAP)
    return -1;

  _dl_printf ("AT_HWCAP:   ");

  for (i = 0; i < _DL_HWCAP_COUNT; ++i)
    if (word & (1 << i))
      _dl_printf (" %s", GLRO(dl_sparc_cap_flags)[i]);

  _dl_printf ("\n");

  return 0;
}

static inline const char *
__attribute__ ((unused))
_dl_hwcap_string (int idx)
{
  return GLRO(dl_sparc_cap_flags)[idx];
};

#include <bits/wordsize.h>
#define HWCAP_IMPORTANT_V9	(__WORDSIZE == 64 ? 0 : HWCAP_SPARC_V9)
#define HWCAP_IMPORTANT		(HWCAP_IMPORTANT_V9 | HWCAP_SPARC_ULTRA3 \
				 | HWCAP_SPARC_BLKINIT | HWCAP_SPARC_N2)

/* There're no platforms to filter out.  */
#define _DL_HWCAP_PLATFORM 0

#define _dl_string_platform(str) (-1)

#endif /* dl-procinfo.h */
