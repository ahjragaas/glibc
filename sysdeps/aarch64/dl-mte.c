/* AArch64 Memtag ABI Extension for ELF support.
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

#include <ldsodefs.h>
#include <sys/auxv.h>
#include <dl-mte.h>
#include <dl-prop.h>

#ifdef USE_AARCH64_MEMTAG_ABI

/* The maximal set of permitted tags that the MTE random tag generation
   instruction may use.  */
# define MTE_ALLOWED_TAGS (0xfffe << PR_MTE_TAG_SHIFT)

# define DT_AARCH64(x) (DT_AARCH64_##x - DT_LOPROC + DT_NUM)

/* Parsed view of the DT_AARCH64_MEMTAG_xxx dynamic tags of an object.  */
struct memtag_marking
{
  /* DT_AARCH64_MEMTAG_MODE, valid iff MODE_PRESENT.  */
  uint64_t mode;
  bool mode_present;
  /* The object requires the respective MTE protection.  */
  bool stack;
  bool heap;
  bool globals;
  /* A tag has a value outside the ones defined by the ABI.  */
  bool invalid;
  /* MEMTAG_MODE without any protection being requested, or only one of
     MEMTAG_GLOBALS and MEMTAG_GLOBALSSZ.  */
  bool orphan;
};

static struct memtag_marking
read_marking (const struct link_map *l)
{
  struct memtag_marking m = { 0 };
  const ElfW(Dyn) *d;

  bool stack_present = false;
  bool heap_present = false;

  if ((d = l->l_info[DT_AARCH64 (MEMTAG_MODE)]) != NULL)
    {
      m.mode_present = true;
      m.mode = d->d_un.d_val;
      if (m.mode > 1)
	m.invalid = true;
    }

  if ((d = l->l_info[DT_AARCH64 (MEMTAG_STACK)]) != NULL)
    {
      stack_present = true;
      if (d->d_un.d_val > 1)
	m.invalid = true;
      m.stack = d->d_un.d_val != 0;
    }

  if ((d = l->l_info[DT_AARCH64 (MEMTAG_HEAP)]) != NULL)
    {
      heap_present = true;
      if (d->d_un.d_val > 1)
	m.invalid = true;
      m.heap = d->d_un.d_val != 0;
    }

  bool globals_present = l->l_info[DT_AARCH64 (MEMTAG_GLOBALS)] != NULL;
  bool globalssz_present = l->l_info[DT_AARCH64 (MEMTAG_GLOBALSSZ)] != NULL;
  m.globals = globals_present;

  if (globals_present != globalssz_present)
    m.orphan = true;
  if (m.mode_present && !stack_present && !heap_present && !globals_present)
    m.orphan = true;

  return m;
}

/* PROGRAM is non-NULL during program startup, where any failure is fatal;
   otherwise the object comes from dlopen and the error can be handled by
   the caller.  */
static void _Noreturn
fail (const struct link_map *l, const char *program, const char *reason)
{
  if (program == NULL)
    _dl_signal_error (0, l->l_name, "dlopen", reason);

  if (l->l_name[0] != '\0')
    _dl_fatal_printf ("%s: error: %s: %s\n", program, l->l_name, reason);
  _dl_fatal_printf ("%s: error: %s\n", program, reason);
}

static void
check_marking (const struct link_map *l, const char *program,
	       const struct memtag_marking *m)
{
  if (m->invalid)
    fail (l, program, "invalid DT_AARCH64_MEMTAG_xxx dynamic tag value");
  if (m->orphan)
    fail (l, program, "inconsistent DT_AARCH64_MEMTAG_xxx dynamic tags");
  if (m->heap)
    fail (l, program, "MTE protection heap is not supported");
  if (m->globals)
    fail (l, program, "MTE protection globals is not supported");
}
#endif /* USE_AARCH64_MEMTAG_ABI */

void
_dl_mte_check (struct link_map *l, const char *program)
{
#ifdef USE_AARCH64_MEMTAG_ABI
  /* During startup L is the main executable map, otherwise it is the
     dlopen'ed object.  */
  bool startup = program != NULL;

  /* Whether the MTE stack protection was enabled at startup.  */
  bool mte_stack = (GL(dl_aarch64_mte) & MTE_STACK) != 0;

  struct memtag_marking m = read_marking (l);
  check_marking (l, program, &m);

  /* Only the main executable marking enables the MTE stack protection.  If
     the main executable is not marked, any dependency that requires it makes
     the process to fail (either at startup or on dlopen).  */
  if (startup)
    mte_stack = m.stack;
  else if (m.stack && !mte_stack)
    fail (l, program,
	  "shared object requires MTE stack protection, but it was not "
	  "enabled at program startup");

  for (unsigned int i = 0; i < l->l_searchlist.r_nlist; i++)
    {
      const struct link_map *dep = l->l_searchlist.r_list[i];
      if (dep == l)
	continue;
      struct memtag_marking dm = read_marking (dep);
      check_marking (dep, program, &dm);
      if (dm.stack && !mte_stack)
	fail (dep, program,
	      startup
	      ? "shared object requires MTE stack protection, but it is not "
		"enabled by the main executable"
	      : "shared object requires MTE stack protection, but it was not "
		"enabled at program startup");
    }

  if (startup && mte_stack)
    {
      GL(dl_aarch64_mte) |= MTE_STACK;
      /* Use asynchronous mode if DT_AARCH64_MEMTAG_MODE is not present.  */
      GL(dl_aarch64_mte) |= (m.mode_present && m.mode == 0)
			    ? MTE_MODE_SYNC : MTE_MODE_ASYNC;
    }
#endif
}

/* Enable the MTE state required by the DT_AARCH64_MEMTAG_xxx dynamic tags of
   the main executable, parsed by _dl_mte_check.  It is done after initial
   relocation processing, but prior any user code execution (including ELF
   constructors).  */
void
_dl_mte_init (void)
{
#ifdef USE_AARCH64_MEMTAG_ABI
  if ((GL(dl_aarch64_mte) & MTE_STACK) == 0)
    return;

  if ((GLRO (dl_hwcap2) & HWCAP2_MTE) == 0)
    _dl_fatal_printf ("Fatal glibc error: MTE stack protection required, "
		      "but not supported by the kernel or CPU\n");

  uint64_t flags = PR_TAGGED_ADDR_ENABLE | MTE_ALLOWED_TAGS;
  flags |= (GL(dl_aarch64_mte) & MTE_MODE_SYNC) != 0
	   ? PR_MTE_TCF_SYNC : PR_MTE_TCF_ASYNC;

  int r = INLINE_SYSCALL_CALL (prctl, PR_SET_TAGGED_ADDR_CTRL, flags, 0, 0, 0);
  if (r < 0)
    _dl_fatal_printf ("Fatal glibc error: failed to enable MTE: %d\n", -r);

  if (!_dl_mte_setup_stack ())
    _dl_fatal_printf ("Fatal glibc error: MTE stack protection setup "
		      "failed\n");
#endif
}
