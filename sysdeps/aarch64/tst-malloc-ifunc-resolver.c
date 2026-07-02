/* Test the caller check in the AArch64 malloc ifunc resolver.
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

/* The malloc ifunc resolver only accepts being invoked from within glibc:
   by the dynamic loader (relocation) or by libc.so (dlsym resolving the
   ifunc through _dl_sym_post), in any namespace.  Any other caller, such as
   a debugger evaluating "call malloc (...)" from an out-of-context dummy
   frame, is rejected with a fatal error.

   This test exercises both directions:

     - The rejection path is reproduced without a debugger by locating the
       STT_GNU_IFUNC resolver for "malloc" in the loaded libc and calling it
       directly from the test binary (which is neither ld.so nor libc.so).

     - The accepted paths must keep working: a plain malloc, a dlsym of the
       malloc ifunc (resolver runs in libc.so), and the same dlsym from a
       second namespace created with dlmopen (resolver runs in that
       namespace's libc.so).  */

#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/ifunc.h>
#include <sys/wait.h>
#include <unistd.h>

#include <support/capture_subprocess.h>
#include <support/check.h>
#include <support/xdlfcn.h>

struct resolver_search
{
  const char *name;
  uintptr_t address;
};

static uintptr_t
dynamic_pointer (const struct dl_phdr_info *info, ElfW(Addr) value)
{
  return value < info->dlpi_addr ? info->dlpi_addr + value : value;
}

/* dl_iterate_phdr callback: look up the STT_GNU_IFUNC symbol NAME in the
   dynamic symbol table of each object and record the runtime address of its
   resolver.  Symbol values are relative to the object load address.  */
static int
find_malloc_resolver (struct dl_phdr_info *info, size_t size, void *closure)
{
  struct resolver_search *search = closure;

  const ElfW(Sym) *symtab = NULL;
  const char *strtab = NULL;
  const ElfW(Word) *hash = NULL;

  for (const ElfW(Phdr) *phdr = info->dlpi_phdr;
       phdr < info->dlpi_phdr + info->dlpi_phnum; ++phdr)
    if (phdr->p_type == PT_DYNAMIC)
      for (ElfW(Dyn) *dyn = (ElfW(Dyn) *) (info->dlpi_addr + phdr->p_vaddr);
	   dyn->d_tag != DT_NULL; ++dyn)
	switch (dyn->d_tag)
	  {
	  case DT_SYMTAB:
	    symtab = (const void *) dynamic_pointer (info, dyn->d_un.d_ptr);
	    break;
	  case DT_STRTAB:
	    strtab = (const void *) dynamic_pointer (info, dyn->d_un.d_ptr);
	    break;
	  case DT_HASH:
	    hash = (const void *) dynamic_pointer (info, dyn->d_un.d_ptr);
	    break;
	  }

  if (symtab == NULL || strtab == NULL || hash == NULL)
    return 0;

  /* The second word of the SysV hash table is the number of symbol table
     entries (nchain).  */
  ElfW(Word) nsyms = hash[1];
  for (ElfW(Word) i = 0; i < nsyms; ++i)
    if (ELF64_ST_TYPE (symtab[i].st_info) == STT_GNU_IFUNC
	&& strcmp (strtab + symtab[i].st_name, search->name) == 0)
      {
	search->address = info->dlpi_addr + symtab[i].st_value;
	return 1;
      }

  return 0;
}

static uintptr_t malloc_resolver;

/* Invoke the malloc ifunc resolver directly, i.e. from a call site that is
   neither the dynamic loader nor libc.so.  The resolver must not return.  */
static void
call_malloc_resolver (void *closure)
{
  __ifunc_arg_t arg =
    {
      sizeof (arg),
      getauxval (AT_HWCAP),
      getauxval (AT_HWCAP2),
      getauxval (AT_HWCAP3),
      getauxval (AT_HWCAP4),
    };
  ElfW(Addr) (*resolver) (uint64_t, const __ifunc_arg_t *)
    = (void *) malloc_resolver;

  resolver (getauxval (AT_HWCAP) | _IFUNC_ARG_HWCAP, &arg);

  /* Unreachable if the resolver correctly rejects the call.  */
  _exit (0);
}

/* Resolve the malloc ifunc through dlsym (which runs the resolver from
   libc.so via _dl_sym_post) and use the result.  Returns the allocation, or
   NULL on failure.  */
static void *
malloc_via_dlsym (void)
{
  void *(*malloc_p) (size_t) = (void *(*) (size_t)) xdlsym (RTLD_DEFAULT,
							   "malloc");
  return malloc_p (32);
}

static int
do_test (void)
{
  struct resolver_search search = { "malloc", 0 };
  if (dl_iterate_phdr (find_malloc_resolver, &search) == 0
      || search.address == 0)
    FAIL_UNSUPPORTED ("could not locate the malloc ifunc resolver "
		      "(malloc not implemented as an ifunc?)");
  malloc_resolver = search.address;

  /* Accepted: the regular malloc, resolved by ld.so.  */
  void *p = malloc (64);
  TEST_VERIFY (p != NULL);
  free (p);

  /* Accepted: dlsym resolving the malloc ifunc runs the resolver from
     libc.so.  */
  void *q = malloc_via_dlsym ();
  TEST_VERIFY (q != NULL);
  free (q);

  /* Accepted: the same dlsym from a second namespace created with dlmopen
     runs the resolver in that namespace's libc.so.  */
  void *h = xdlmopen (LM_ID_NEWLM, "$ORIGIN/tst-malloc-ifunc-resolver-mod.so",
		      RTLD_NOW);
  int (*mod_resolve) (void) = (int (*) (void))
    xdlsym (h, "resolve_malloc_via_dlsym");
  TEST_VERIFY (mod_resolve () != 0);
  xdlclose (h);

  /* Rejected: calling the resolver out of context must abort the process
     through _dl_fatal_printf, which reports the error and calls
     _exit (127).  Run it in a subprocess so the test survives.  */
  struct support_capture_subprocess proc
    = support_capture_subprocess (call_malloc_resolver, NULL);

  TEST_VERIFY (WIFEXITED (proc.status));
  TEST_COMPARE (WEXITSTATUS (proc.status), 127);
  TEST_VERIFY (strstr (proc.err.buffer, "malloc ifunc resolver") != NULL);

  support_capture_subprocess_free (&proc);
  return 0;
}

#include <support/test-driver.c>
