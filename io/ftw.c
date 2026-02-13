/* File tree walker functions.
   Copyright (C) 1996-2026 Free Software Foundation, Inc.
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

#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <ftw.h>
#include <scratch_buffer.h>
#include <search.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Support for the LFS API version.  */
#ifndef FTW_NAME
# define FTW_NAME ftw
# define NFTW_NAME nftw
# define NFTW_OLD_NAME __old_nftw
# define NFTW_NEW_NAME __new_nftw
# define INO_T ino_t
# define STRUCT_STAT stat
# define FTW_FUNC_T __ftw_func_t
# define NFTW_FUNC_T __nftw_func_t
#endif
#ifndef FTS_TYPE
# define FTS_TYPE    FTS
# define FTSENT_TYPE FTSENT
# define FTS_OPEN    __fts_open
# define FTS_READ    __fts_read
# define FTS_SET     __fts_set
# define FTS_CLOSE   __fts_close
#endif

struct known_object
{
  dev_t dev;
  INO_T ino;
};

static int
object_compare (const void *p1, const void *p2)
{
  /* We don't need a sophisticated and useful comparison.  We are only
     interested in equality.  However, we must be careful not to
     accidentally compare `holes' in the structure.  */
  const struct known_object *kp1 = p1, *kp2 = p2;
  int cmp1;
  cmp1 = (kp1->ino > kp2->ino) - (kp1->ino < kp2->ino);
  if (cmp1 != 0)
    return cmp1;
  return (kp1->dev > kp2->dev) - (kp1->dev < kp2->dev);
}

static int
add_object (void **known_objects, struct STRUCT_STAT *st)
{
  struct known_object *newp = malloc (sizeof (struct known_object));
  if (newp == NULL)
    return -1;
  newp->dev = st->st_dev;
  newp->ino = st->st_ino;
  return __tsearch (newp, known_objects, object_compare) ? 0 : -1;
}


static inline int
find_object (void **known_objects, struct STRUCT_STAT *st)
{
  struct known_object obj;
  obj.dev = st->st_dev;
  obj.ino = st->st_ino;
  return __tfind (&obj, known_objects, object_compare) != NULL;
}

union func_callback_t
{
  FTW_FUNC_T ftw_func;
  NFTW_FUNC_T nftw_func;
};

static int
ftw_startup (const char *dir, bool is_nftw, union func_callback_t func,
	     int descriptors, int flags)
{
  /* First make sure the parameters are reasonable.  */
  if (dir[0] == '\0')
    {
      __set_errno (ENOENT);
      return -1;
    }

  /* Data structure for keeping fingerprints of already processed
     object.  This is needed when not using FTW_PHYS.  */
  void *known_objects = NULL;
  /* Device of starting point.  Needed for FTW_MOUNT.  */
  dev_t root_dev = 0;

  /* NB: The fts FTS_LOGICAL implies on FTS_NOCHDIR, so to to proper implement
     FTW_CHDIR it requires manually manage the chdir / fchdir dance around
     the user's callback.  The BUF is used to create the required path.  */
  struct scratch_buffer buf;
  scratch_buffer_init (&buf);

  int start_fd = __open (".", O_RDONLY | O_CLOEXEC);
  if (start_fd < -1)
    return -1;

  int fts_options = 0;
  if (flags & FTW_PHYS)
    fts_options |= FTS_PHYSICAL;
  else
    fts_options |= FTS_LOGICAL;

  if (flags & FTW_MOUNT)
    fts_options |= FTS_XDEV;

  if (!(flags & FTW_CHDIR))
    fts_options |= FTS_NOCHDIR;

  char *const paths[] = { (char *)dir, NULL };

  /* NB: ingnore 'descriptors' limits since fts manages fds dynamically.  */
  FTS_TYPE *ftsp = FTS_OPEN (paths, fts_options, NULL);
  if (!ftsp)
    {
      __close (start_fd);
      return -1;
    }

  FTSENT_TYPE *ent = NULL;
  int rc = 0;
  int save_err;

  bool postorder = (flags & FTW_DEPTH) != 0;

  /* Used to proper support FTW_SKIP_SIBLINGS to avoid call fts_read again
     of the next iteration.  */
  bool skip_read = false;

  while (true)
    {
      if (!skip_read)
	{
	  errno = 0;
          ent = FTS_READ (ftsp);
        }
      skip_read = false;

      if (ent == NULL)
	{
	  if (errno != 0)
	    goto done;
	  break;
        }

      if (ent->fts_level == FTS_ROOTLEVEL)
	{
	  /* If the STARTING path cannot be accessed, nftw must fail rather
	     than calling the callback with FTW_NS (it is required only for
	     FTS_ROOTLEVEL).  */
	  if (ent->fts_info == FTS_NS && ent->fts_errno != 0)
	    {
	      rc = -1;
	      errno = ent->fts_errno;
	      goto done;
            }

	  /* Remember the device of the initial directory in case FTW_MOUNT. */
	  if (ent->fts_statp)
	    root_dev = ent->fts_statp->st_dev;
        }

      /* Handle FTW_MOUNT.  */
      bool is_mount_crossing = false;
      if ((flags & FTW_MOUNT) && ent->fts_level > FTS_ROOTLEVEL
	  && ent->fts_statp)
	if (ent->fts_statp->st_dev != root_dev)
	  is_mount_crossing = true;

      int fn_flag;
      switch (ent->fts_info)
	{
	case FTS_D:
	  /* Remember the device of the initial directory in case FTW_MOUNT
	     is given.  */
	  if (ent->fts_level == FTS_ROOTLEVEL)
	    {
	      if (!(flags & FTW_PHYS)
		  && add_object (&known_objects, ent->fts_statp) == -1)
		{
		  rc = -1;
		  goto done;
		}
	    }
	  else
	    {
	      if (!(flags & FTW_PHYS)
		  && find_object (&known_objects, ent->fts_statp))
		continue;
	      if (add_object (&known_objects, ent->fts_statp) == -1)
		{
		  rc = -1;
		  goto done;
		}
	    }

	  /* Normally we skip FTS_D in depth mode, but for a mount
	     crossing, fts (FTS_XDEV) will NOT descend.  We will never
	     get FTS_DP.  We must handle it manually.  */
	  if (postorder && !is_mount_crossing)
	    continue;

	  /* fts returns FTS_D for directories it has not tried to open yet,
	     even if it has not read permissions.  The nftw must report
	     FTW_DNR instead.   */
	  if (__faccessat (start_fd, ent->fts_accpath, R_OK, AT_EACCESS) == 0)
	    fn_flag = FTW_D;
	  else
	    {
	      fn_flag = FTW_DNR;
	      FTS_SET (ftsp, ent, FTS_SKIP);
	    }
	  break;

	case FTS_DP:
	  if (!postorder)
	    continue;
	  fn_flag = FTW_DP;
	  break;

	case FTS_DEFAULT:
        case FTS_F:
	  fn_flag = FTW_F;
	  break;

        case FTS_SL:
	  fn_flag = FTW_SL;
	  break;

	case FTS_SLNONE:
	  fn_flag = FTW_SLN;
	  break;

	case FTS_DNR:
	  fn_flag = FTW_DNR;
	  break;

	case FTS_NS:
	  fn_flag = FTW_NS;
	  break;

        case FTS_DC:
	  if (S_ISDIR (ent->fts_statp->st_mode))
	    {
	      if (!(flags & FTW_PHYS)
		  && find_object (&known_objects, ent->fts_statp))
		continue;
	      if (add_object (&known_objects, ent->fts_statp) == -1)
		{
		  rc = 1;
		  goto done;
		}
	    }
	  /* A directory cycle was detected (Logical walk only) and
	     instead of aborting with ELOOP, we report this as a
	     symlink that cannot be successfully followed (FTW_SLN).  */
          fn_flag = FTW_SLN;
          break;

        default:
	  rc = -1;
	  goto done;
        }

        struct FTW ftw_data = {
	  .base = ent->fts_pathlen - ent->fts_namelen,
	  .level = ent->fts_level
	};

        bool chdir_performed = false;
	if (flags & FTW_CHDIR)
	  {
	    const char *target_dir = NULL;

	    size_t dir_len = ent->fts_pathlen - ent->fts_namelen;
	    if (dir_len == 0)
	      /* Case: "filename" (Root level file/dir with no path prefix):
		 the containing directory is the start directory.  */
              target_dir = ".";
            else
	      {
		/* Case: "path/to/filename": we need to extract "path/to"  */
		if (!scratch_buffer_set_array_size (&buf, dir_len, 1))
		  goto done;
		memcpy (buf.data, ent->fts_accpath, dir_len);
		char *target = buf.data;
		if (dir_len > 1 && target[dir_len-1] == '/')
		  target[dir_len-1] = '\0';
		target_dir = target;
	      }

            if (__chdir (target_dir) == 0)
              chdir_performed = true;
            else
	      {
		rc = -1;
		goto done;
	      }
	  }

        rc = is_nftw
	  ? func.nftw_func (ent->fts_path, ent->fts_statp, fn_flag, &ftw_data)
	  : func.ftw_func (ent->fts_path, ent->fts_statp, fn_flag);

	if (chdir_performed && __fchdir (start_fd) != 0)
	  {
	    rc = -1;
	    goto done;
	  }

	if (!(flags & FTW_ACTIONRETVAL))
	  {
	    if (rc != 0)
	      break;
	    continue;
	  }
	/* FTW_ACTIONRETVAL support.  */
	switch (rc)
	  {
	  case FTW_CONTINUE:
	    /* Default behavior: just proceed to next entry.  */
	    break;

	  case FTW_STOP:
	    /* Stop traversal immediately (success return code).  We set rc=0
	       here because FTW_STOP is considered a "successful" stop,
	       unlike a non-zero return in standard mode.  */
	    rc = 0;
	    goto done;

	  case FTW_SKIP_SUBTREE:
	    /* Only meaningful if we are currently visiting a directory in
	       pre-order (FTS_D).  */
	    if (ent->fts_info == FTS_D)
	      FTS_SET (ftsp, ent, FTS_SKIP);
	    break;

	  case FTW_SKIP_SIBLINGS:
	    /* We must skip everything until we emerge at a lower level
	       (parent).  */
	    {
	      int current_level = ent->fts_level;

	      /* Drain fts until level < current.  */
	      while ((ent = FTS_READ (ftsp)) != NULL)
		{
		  if (ent->fts_level < current_level)
		    {
		      skip_read = true;
		      break;
		    }
		}

		if (ent == NULL)
		  goto done;
	    } break;

	  default:
	    rc = 0;
	    goto done;
	  }
    }

done:
  scratch_buffer_free (&buf);
  __tdestroy (known_objects, free);
  save_err = errno;
  FTS_CLOSE (ftsp);
  __close (start_fd);
  __set_errno (save_err);

  return rc;
}

/* Entry points.  */

int
FTW_NAME (const char *path, FTW_FUNC_T func, int descriptors)
{
  return ftw_startup (path,
		      false,
		      (union func_callback_t) { .ftw_func = func },
		      descriptors,
		      0);
}

#ifndef NFTW_OLD_NAME
int
NFTW_NAME (const char *path, NFTW_FUNC_T func, int descriptors, int flags)
{
  return ftw_startup (path, true,
		      (union func_callback_t) { .nftw_func = func },
		      descriptors,
		      flags);
}
#else

# include <shlib-compat.h>

int NFTW_NEW_NAME (const char *, NFTW_FUNC_T, int, int);

int
NFTW_NEW_NAME (const char *path, NFTW_FUNC_T func, int descriptors, int flags)
{
  if (flags
      & ~(FTW_PHYS | FTW_MOUNT | FTW_CHDIR | FTW_DEPTH | FTW_ACTIONRETVAL))
    {
      __set_errno (EINVAL);
      return -1;
    }
  return ftw_startup (path,
		      true,
		      (union func_callback_t) { .nftw_func = func },
		      descriptors,
		      flags);
}
versioned_symbol (libc, NFTW_NEW_NAME, NFTW_NAME, GLIBC_2_3_3);

# if SHLIB_COMPAT(libc, GLIBC_2_1, GLIBC_2_3_3)

/* Older nftw* version just ignored all unknown flags.  */

int NFTW_OLD_NAME (const char *, NFTW_FUNC_T, int, int);

int
attribute_compat_text_section
NFTW_OLD_NAME (const char *path, NFTW_FUNC_T func, int descriptors, int flags)
{
  flags &= (FTW_PHYS | FTW_MOUNT | FTW_CHDIR | FTW_DEPTH);
  return ftw_startup (path,
		      true,
		      (union func_callback_t) { .nftw_func = func },
		      descriptors,
		      flags);
}

compat_symbol (libc, NFTW_OLD_NAME, NFTW_NAME, GLIBC_2_1);
# endif
#endif /* NFTW_OLD_NAME  */
