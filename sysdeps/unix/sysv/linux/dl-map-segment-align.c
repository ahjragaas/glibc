/* _dl_map_segment_align.  Linux version.
   Copyright (C) 2026 Free Software Foundation, Inc.
   Copyright The GNU Toolchain Authors.
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

#include <dl-map-segment-align.h>
#include <dl-tunables.h>
#include <hugepages.h>

ElfW (Addr)
_dl_map_segment_align (const struct loadcmd *c, ElfW (Addr) p_align_max)
{
  static enum thp_mode_t thp_mode = thp_mode_not_supported;
  static unsigned long int thp_pagesize;

  if (TUNABLE_GET (glibc, elf, thp, int32_t, NULL) == 0)
    return p_align_max;

  if (__glibc_unlikely (thp_mode == thp_mode_not_supported
                        || thp_pagesize == 0))
    {
      unsigned long int default_thp_pagesize = DL_MAP_DEFAULT_THP_PAGESIZE;
      thp_mode = default_thp_pagesize ? thp_mode_always : __get_thp_mode ();
      thp_pagesize = default_thp_pagesize ? : __get_thp_size ();
    }

  /* Aligning load segments that are large enough to the PMD size helps
     improve THP eligibility and reduces TLB pressure.
     We cap the huge page size at MAX_THP_PAGESIZE to avoid over-aligning
     on systems with very large normal pages (like 64K pages with 512M
     huge pages). */
  if (thp_mode == thp_mode_always
      && thp_pagesize <= MAX_THP_PAGESIZE
      && ((c->mapstart | c->mapoff) & (thp_pagesize - 1)) == 0
      && (c->mapend - c->mapstart) >= thp_pagesize
      && p_align_max < thp_pagesize
      && (c->prot & PROT_WRITE) == 0)
    return thp_pagesize;

  return p_align_max;
}
