/* Check that a marked main executable can dlopen a module that requires
   MTE stack protection.
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

#include <sys/auxv.h>
#include <support/capture_subprocess.h>
#include <support/check.h>
#include <support/xdlfcn.h>

#include "tst-mte-helper.h"

static void (*mod_run_mte_test) (void *);

static void
run_dlopened_test (void *closure)
{
  mod_run_mte_test (closure);
}

static int
do_test (void)
{
  if (!(getauxval (AT_HWCAP2) & HWCAP2_MTE))
    FAIL_UNSUPPORTED ("kernel or CPU does not support or enable MTE");

  TEST_VERIFY_EXIT (mte_enable ());

  void *h = xdlopen ("tst-memtag-mod2.so", RTLD_NOW);
  mod_run_mte_test = xdlsym (h, "run_mte_test");

  struct support_capture_subprocess result =
    support_capture_subprocess (run_dlopened_test, (void *) TEST_MAIN);
  support_capture_subprocess_check (&result, "MTE stack fault in dlopened "
				    "module", EXIT_MTESERR, sc_allow_none);
  support_capture_subprocess_free (&result);

  xdlclose (h);

  return 0;
}

#include <support/test-driver.c>
