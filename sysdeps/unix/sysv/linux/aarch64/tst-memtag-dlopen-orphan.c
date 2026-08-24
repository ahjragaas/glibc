/* Check that dlopen of a module with an orphan DT_AARCH64_MEMTAG_MODE
   dynamic tag fails instead of aborting the process.
   Copyright (C) 2026 Free Software Foundation, Inc.
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

#include <dlfcn.h>
#include <string.h>
#include <support/check.h>
#include <support/xdlfcn.h>

#include "tst-mte-helper.h"

static int
do_test (void)
{
  TEST_VERIFY_EXIT (!mte_enable ());

  /* The module marking is inconsistent (DT_AARCH64_MEMTAG_MODE without any
     MTE protection request).  The dlopen should fail without aborting the
     process.  */
  void *h = dlopen ("tst-memtag-mod4.so", RTLD_NOW);
  TEST_VERIFY (h == NULL);
  const char *message = dlerror ();
  if (strstr (message, "inconsistent DT_AARCH64_MEMTAG_xxx") == NULL)
    FAIL_EXIT1 ("invalid dlopen error message: %s", message);

  /* The process should still work, including loading unmarked modules.  */
  xdlclose (xdlopen ("tst-memtag-mod1.so", RTLD_NOW));

  return 0;
}

#include <support/test-driver.c>
