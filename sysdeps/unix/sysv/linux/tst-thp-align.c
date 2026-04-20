/* Test the THP compatible alignment of PT_LOAD segments.

   Copyright (C) 2026 Free Software Foundation, Inc.

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

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <intprops.h>
#include <inttypes.h>
#include <support/check.h>
#include <support/xdlfcn.h>
#include <support/xstdio.h>
#include <support/xunistd.h>

#define THP_SIZE_MOD_NAME "tst-thp-size-mod.so"
#define MAX_THP_PAGESIZE (32 * 1024 * 1024)

enum thp_mode_t
{
  thp_mode_always,
  thp_mode_madvise,
  thp_mode_never,
  thp_mode_not_supported
};

static unsigned long int
get_thp_size (void)
{
  int fd = open ("/sys/kernel/mm/transparent_hugepage/hpage_pmd_size",
                 O_RDONLY, 0);
  if (fd == -1)
    return 0;

  char str[INT_BUFSIZE_BOUND (unsigned long int)];
  ssize_t s = read (fd, str, sizeof (str));
  close (fd);
  if (s < 0)
    return 0;

  unsigned long int r = 0;
  for (ssize_t i = 0; i < s; i++)
    {
      if (str[i] == '\n')
    break;
      r *= 10;
      r += str[i] - '0';
    }
  return r;
}

static enum thp_mode_t
get_thp_mode (void)
{
  int fd = open ("/sys/kernel/mm/transparent_hugepage/enabled", O_RDONLY, 0);
  if (fd == -1)
    return thp_mode_not_supported;

  static const char mode_always[]  = "[always] madvise never\n";
  static const char mode_madvise[] = "always [madvise] never\n";
  static const char mode_never[]   = "always madvise [never]\n";

  char str[sizeof(mode_always)];
  ssize_t s = read (fd, str, sizeof (str));
  if (s >= sizeof str || s < 0)
    return thp_mode_not_supported;
  str[s] = '\0';
  close (fd);

  if (s == sizeof (mode_always) - 1)
    {
      if (strcmp (str, mode_always) == 0)
    return thp_mode_always;
      else if (strcmp (str, mode_madvise) == 0)
    return thp_mode_madvise;
      else if (strcmp (str, mode_never) == 0)
    return thp_mode_never;
    }
  return thp_mode_not_supported;
}

static void
check_align (void)
{
  unsigned long int thp_size = get_thp_size ();
  enum thp_mode_t thp_mode = get_thp_mode ();

  if (thp_size == 0)
    {
      FAIL_UNSUPPORTED ("unable to get THP size.\n");
      return;
    }

  if (thp_size > MAX_THP_PAGESIZE)
    {
      FAIL_UNSUPPORTED ("THP size exceeds MAX_THP_PAGESIZE.\n");
      return;
    }

  if (thp_mode != thp_mode_always)
    {
      FAIL_UNSUPPORTED ("THP mode is not always.\n");
      return;
    }

  FILE *f = xfopen ("/proc/self/maps", "r");
  char *line = NULL;
  size_t len;

  while (xgetline (&line, &len, f))
    {
      uintptr_t from, to;
      char *prot = NULL, *path = NULL;
      int r = sscanf (line, "%" SCNxPTR "-%" SCNxPTR "%ms%*s%*s%*s%ms",
                      &from, &to, &prot, &path);

      TEST_VERIFY (r == 3 || r == 4);

      if (strstr (prot, "x") && strstr (path, THP_SIZE_MOD_NAME))
        TEST_COMPARE (from % thp_size, 0);

      free (path);
    }

  free (line);
  xfclose (f);
}

static int
do_test (void)
{
  void *dl;

  dl = xdlopen (THP_SIZE_MOD_NAME, RTLD_NOW);
  check_align ();
  xdlclose (dl);

  return 0;
}

#include <support/test-driver.c>
