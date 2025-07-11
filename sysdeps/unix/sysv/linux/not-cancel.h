/* Uncancelable versions of cancelable interfaces.  Linux/NPTL version.
   Copyright (C) 2003-2025 Free Software Foundation, Inc.
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

#ifndef NOT_CANCEL_H
# define NOT_CANCEL_H

#include <fcntl.h>
#include <sysdep.h>
#include <errno.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/random.h>

/* Non cancellable open syscall.  */
extern int __open_nocancel (const char *, int, ...);

/* Non cancellable open syscall (LFS version).  */
extern int __open64_nocancel (const char *, int, ...);

/* Non cancellable openat syscall.  */
extern int __openat_nocancel (int fd, const char *, int, ...);

/* Non cacellable openat syscall (LFS version).  */
extern int __openat64_nocancel (int fd, const char *, int, ...);

/* Non cancellable read syscall.  */
__typeof (__read) __read_nocancel;

/* Non cancellable pread syscall (LFS version).  */
__typeof (__pread64) __pread64_nocancel;

/* Uncancelable write.  */
__typeof (__write) __write_nocancel;

/* Uncancelable close.  */
__typeof (__close) __close_nocancel;

/* Uncancellable close that does not also set errno in case of failure.  */
void __close_nocancel_nostatus (int);

/* Uncancelable fcntl.  */
int __fcntl64_nocancel (int, int, ...);

#if IS_IN (libc) || IS_IN (rtld)
hidden_proto (__open_nocancel)
hidden_proto (__open64_nocancel)
hidden_proto (__openat_nocancel)
hidden_proto (__openat64_nocancel)
hidden_proto (__read_nocancel)
hidden_proto (__pread64_nocancel)
hidden_proto (__write_nocancel)
hidden_proto (__close_nocancel)
hidden_proto (__close_nocancel_nostatus)
hidden_proto (__fcntl64_nocancel)
#endif

/* Non cancellable writev syscall that does not also set errno in case of
   failure.  */
static inline void
__writev_nocancel_nostatus (int fd, const struct iovec *iov, int iovcnt)
{
  INTERNAL_SYSCALL_CALL (writev, fd, iov, iovcnt);
}

static inline ssize_t
__getrandom_nocancel_direct (void *buf, size_t buflen, unsigned int flags)
{
  return INLINE_SYSCALL_CALL (getrandom, buf, buflen, flags);
}

__typeof (getrandom) __getrandom_nocancel attribute_hidden;

/* Non cancellable getrandom syscall that does not also set errno in case of
   failure.  */
static inline ssize_t
__getrandom_nocancel_nostatus_direct (void *buf, size_t buflen, unsigned int flags)
{
  return INTERNAL_SYSCALL_CALL (getrandom, buf, buflen, flags);
}

static inline int
__poll_infinity_nocancel (struct pollfd *fds, nfds_t nfds)
{
  return INLINE_SYSCALL_CALL (ppoll, fds, nfds, NULL, NULL, 0);
}

#endif /* NOT_CANCEL_H  */
