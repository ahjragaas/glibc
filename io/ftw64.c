/* File tree walker functions.  LFS version.
   Copyright (C) 1996-2026 Free Software Foundation, Inc.
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

#define FTW_NAME ftw64
#define NFTW_NAME nftw64
#define NFTW_OLD_NAME __old_nftw64
#define NFTW_NEW_NAME __new_nftw64
#define INO_T ino64_t
#define STRUCT_STAT stat64
#define FTW_FUNC_T __ftw64_func_t
#define NFTW_FUNC_T __nftw64_func_t

#define FTS_TYPE FTS64
#define FTSENT_TYPE FTSENT64
#define FTS_OPEN __fts64_open
#define FTS_READ __fts64_read
#define FTS_SET __fts64_set
#define FTS_CLOSE __fts64_close

#include "ftw.c"
