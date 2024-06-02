/* Basic tests for sealing.
   Copyright (C) 2024 Free Software Foundation, Inc.
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

#include <array_length.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <support/capture_subprocess.h>
#include <support/check.h>
#include <support/support.h>
#include <support/xdlfcn.h>
#include <support/xstdio.h>

#define LIB_PRELOAD              "lib-tst-dl_mseal-preload.so"

#define LIB_NEEDED_1             "lib-tst-dl_mseal-1.so"
#define LIB_NEEDED_2             "lib-tst-dl_mseal-2.so"

#define LIB_DLOPEN_DEFAULT       "lib-tst-dl_mseal-dlopen-1.so"
#define LIB_DLOPEN_DEFAULT_DEP   "lib-tst-dl_mseal-dlopen-1-1.so"
#define LIB_DLOPEN_NODELETE      "lib-tst-dl_mseal-dlopen-2.so"
#define LIB_DLOPEN_NODELETE_DEP  "lib-tst-dl_mseal-dlopen-2-1.so"

static int
new_flags (const char flags[4])
{
  bool read_flag  = flags[0] == 'r';
  bool write_flag = flags[1] == 'w';
  bool exec_flag  = flags[2] == 'x';

  write_flag = !write_flag;

  return (read_flag ? PROT_READ : 0)
	 | (write_flag ? PROT_WRITE : 0)
	 | (exec_flag ? PROT_EXEC : 0);
}

/* Expected libraries that loader will seal.  */
static const char *expected_sealed_libs[] =
{
#ifdef TEST_STATIC
  "tst-dl_mseal-static",
#else
  "libc.so",
  "ld.so",
  "tst-dl_mseal",
  LIB_PRELOAD,
  LIB_NEEDED_1,
  LIB_NEEDED_2,
  LIB_DLOPEN_NODELETE,
  LIB_DLOPEN_NODELETE_DEP,
#endif
  "[vdso]",
};

/* Libraries/VMA that could not be sealed.  */
static const char *non_sealed_vmas[] =
{
  ".",				/* basename value for empty string anonymous
				   mappings.  */
  "[heap]",
  "[vsyscall]",
  "[vvar]",
  "[stack]",
  "zero",			/* /dev/zero  */
#ifndef TEST_STATIC
  "tst-dl_mseal-mod-2.so",
  LIB_DLOPEN_DEFAULT,
  LIB_DLOPEN_DEFAULT_DEP
#endif
};

static int
is_in_string_list (const char *s, const char *const list[], size_t len)
{
  for (size_t i = 0; i != len; i++)
    if (strcmp (s, list[i]) == 0)
      return i;
  return -1;
}

static int
handle_restart (void)
{
#ifndef TEST_STATIC
  xdlopen (LIB_DLOPEN_NODELETE, RTLD_NOW | RTLD_NODELETE);
  xdlopen (LIB_DLOPEN_DEFAULT, RTLD_NOW);
#endif

  FILE *fp = xfopen ("/proc/self/maps", "r");
  char *line = NULL;
  size_t linesiz = 0;

  unsigned long pagesize = getpagesize ();

  bool found_expected[array_length(expected_sealed_libs)] = { false };
  while (xgetline (&line, &linesiz, fp) > 0)
    {
      uintptr_t start;
      uintptr_t end;
      char flags[5] = { 0 };
      char name[256] = { 0 };
      int idx;

      /* The line is in the form:
	 start-end flags offset dev inode pathname   */
      int r = sscanf (line,
		      "%" SCNxPTR "-%" SCNxPTR " %4s %*s %*s %*s %256s",
		      &start,
		      &end,
		      flags,
		      name);
      TEST_VERIFY_EXIT (r == 3 || r == 4);

      int found = false;

      const char *libname = basename (name);
      if ((idx = is_in_string_list (libname, expected_sealed_libs,
				    array_length (expected_sealed_libs)))
	   != -1)
	{
	  /* Check if we can change the protection flags of the segment.  */
	  int new_prot = new_flags (flags);
	  TEST_VERIFY_EXIT (mprotect ((void *) start, end - start,
				      new_prot) == -1);
	  TEST_VERIFY_EXIT (errno == EPERM);

	  /* Also checks trying to map over the sealed libraries.  */
	  {
	    char *p = mmap ((void *) start, pagesize, new_prot,
			    MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	    TEST_VERIFY_EXIT (p == MAP_FAILED);
	    TEST_VERIFY_EXIT (errno == EPERM);
	  }

	  /* And if remap is also blocked.  */
	  {
	    char *p = mremap ((void *) start, end - start, end - start, 0);
	    TEST_VERIFY_EXIT (p == MAP_FAILED);
	    TEST_VERIFY_EXIT (errno == EPERM);
	  }

	  printf ("sealed:     vma: %#" PRIxPTR "-%#" PRIxPTR " %s %s\n",
		  start,
		  end,
		  flags,
		  name);

	  found_expected[idx] = true;
	  found = true;
	}

      if (!found)
	{
	  if (is_in_string_list (libname, non_sealed_vmas,
				 array_length (non_sealed_vmas)) != -1)
	    printf ("not-sealed: vma: %#" PRIxPTR "-%#" PRIxPTR " %s %s\n",
		    start,
		    end,
		    flags,
		    name);
	  else
	    FAIL_EXIT1 ("unexpected vma: %#" PRIxPTR "-%#" PRIxPTR " %s %s\n",
			start,
			end,
			flags,
			name);
	}
    }
  xfclose (fp);

  printf ("\n");

  /* Also check if all the expected sealed maps were found.  */
  for (int i = 0; i < array_length (expected_sealed_libs); i++)
    if (!found_expected[i])
      FAIL_EXIT1 ("expected VMA %s not sealed\n", expected_sealed_libs[i]);

  return 0;
}

static int restart;
#define CMDLINE_OPTIONS \
  { "restart", no_argument, &restart, 1 },

static int
do_test (int argc, char *argv[])
{
  /* We must have either:
     - One or four parameters left if called initially:
       + path to ld.so         optional
       + "--library-path"      optional
       + the library path      optional
       + the application name  */
  if (restart)
    return handle_restart ();

  /* Check the test requirements.  */
  {
    int r = mseal (NULL, 0, 0);
    if (r == -1 && errno == ENOSYS)
      FAIL_UNSUPPORTED ("mseal is not supported by the kernel");
    else
      TEST_VERIFY_EXIT (r == 0);
  }
  support_need_proc ("Reads /proc/self/maps to get stack names.");

  char *spargv[9];
  int i = 0;
  for (; i < argc - 1; i++)
    spargv[i] = argv[i + 1];
  spargv[i++] = (char *) "--direct";
  spargv[i++] = (char *) "--restart";
  spargv[i] = NULL;

  char *envvarss[3];
  envvarss[0] = (char *) "GLIBC_TUNABLES=glibc.rtld.seal=2";
#ifndef TEST_STATIC
  envvarss[1] = (char *) "LD_PRELOAD=" LIB_PRELOAD;
  envvarss[2] = NULL;
#else
  envvarss[1] = NULL;
#endif

  struct support_capture_subprocess result =
    support_capture_subprogram (spargv[0], spargv, envvarss);
  support_capture_subprocess_check (&result, "tst-dl_mseal", 0,
				    sc_allow_stdout);

  {
    FILE *out = fmemopen (result.out.buffer, result.out.length, "r");
    TEST_VERIFY (out != NULL);
    char *line = NULL;
    size_t linesz = 0;
    while (xgetline (&line, &linesz, out))
      printf ("%s", line);
    fclose (out);
  }

  support_capture_subprocess_free (&result);

  return 0;
}

#define TEST_FUNCTION_ARGV do_test
#include <support/test-driver.c>
