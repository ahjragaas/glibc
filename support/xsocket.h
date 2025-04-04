/* Error-checking wrappers for socket functions.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
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

#ifndef SUPPORT_XSOCKET_H
#define SUPPORT_XSOCKET_H

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

int xsocket (int, int, int);
void xsetsockopt (int, int, int, const void *, socklen_t);
void xgetsockname (int, struct sockaddr *, socklen_t *);
void xgetpeername (int, struct sockaddr *, socklen_t *);
void xconnect (int, const struct sockaddr *, socklen_t);
void xbind (int, const struct sockaddr *, socklen_t);
void xlisten (int, int);
int xaccept (int, struct sockaddr *, socklen_t *);
int xaccept4 (int, struct sockaddr *, socklen_t *, int);
void xsendto (int, const void *, size_t, int,
              const struct sockaddr *, socklen_t);
size_t xrecvfrom (int, void *, size_t, int, struct sockaddr *, socklen_t *);
int xpoll (struct pollfd *, nfds_t, int);

#endif /* SUPPORT_XSOCKET_H */
