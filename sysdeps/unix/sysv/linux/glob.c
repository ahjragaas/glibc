/* Find pathnames matching a pattern.  Linux version.
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

#include <sys/stat.h>
#include <kernel_stat.h>

#define struct_stat    struct stat
#define struct_stat64  struct stat64
#define GLOB_LSTAT     gl_lstat
#define GLOB_STAT64    __stat64
#define GLOB_LSTAT64   __lstat64

#define glob64 __no_glob64_decl
#define __glob64 __no___glob64_decl
#include <posix/glob.c>
#undef glob64
#undef __glob64

#if XSTAT_IS_XSTAT64
strong_alias (__glob, __glob64)
versioned_symbol (libc, __glob64, glob64, GLIBC_2_27);
#endif
