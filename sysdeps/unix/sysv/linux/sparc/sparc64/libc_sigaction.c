/* POSIX.1 sigaction call for Linux/SPARC64.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
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

#include <string.h>
#include <syscall.h>
#include <sysdep.h>

/* Defined on sigreturn_stub.S.  */
void __rt_sigreturn_stub (void);

#define STUB(act, sigsetsize) \
  (((unsigned long) &__rt_sigreturn_stub) - 8),	\
  (sigsetsize)

#include <sysdeps/unix/sysv/linux/libc_sigaction.c>
