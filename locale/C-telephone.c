/* Copyright (C) 1998-2025 Free Software Foundation, Inc.
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

#include <endian.h>

#include "localeinfo.h"

/* This table's entries are taken from ISO 14652, the table in section
   4.11 "LC_TELEPHONE".  */

const struct __locale_data _nl_C_LC_TELEPHONE attribute_hidden =
{
  _nl_C_name,
  NULL, 0, 0,			/* no file mapped */
  NULL,				/* No cached data.  */
  UNDELETABLE,
  0,
  5,
  {
    { .string = "+%c %a %l" },
    { .string = "" },
    { .string = "" },
    { .string = "" },
    { .string = _nl_C_codeset }
  }
};
