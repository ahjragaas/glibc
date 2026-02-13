#ifndef _FTS_H
#include <io/fts.h>

#ifndef _ISOMAC
# if __TIMESIZE != 64
#  include <sys/stat.h>

typedef struct
{
  struct _ftsent64_time64 *fts_cur;
  struct _ftsent64_time64 *fts_child;
  struct _ftsent64_time64 **fts_array;
  dev_t fts_dev;
  char *fts_path;
  int fts_rfd;
  int fts_pathlen;
  int fts_nitems;
  int (*fts_compar) (const void *, const void *);
  int fts_options;
} FTS64_TIME64;

typedef struct _ftsent64_time64
{
  struct _ftsent64_time64 *fts_cycle;
  struct _ftsent64_time64 *fts_parent;
  struct _ftsent64_time64 *fts_link;
  long fts_number;
  void *fts_pointer;
  char *fts_accpath;
  char *fts_path;
  int fts_errno;
  int fts_symfd;
  unsigned short fts_pathlen;
  unsigned short fts_namelen;

  ino64_t fts_ino;
  dev_t fts_dev;
  nlink_t fts_nlink;

  short fts_level;
  unsigned short fts_info;
  unsigned short fts_flags;
  unsigned short fts_instr;

  struct __stat64_t64 *fts_statp;
  char fts_name[1];
} FSTENT64_TIME64;

# endif

__typeof (fts_open) __fts_open;
libc_hidden_proto (__fts_open);
__typeof (fts64_open) __fts64_open;
libc_hidden_proto (__fts64_open);
__typeof (fts64_open) __fts64_open;
libc_hidden_proto (__fts64_open);
#if __TIMESIZE != 64
extern FTS64_TIME64* __fts64_open_time64 (char *const*, int,
					  int (*)(const FSTENT64_TIME64 **,
						  const FSTENT64_TIME64 **));
libc_hidden_proto (__fts64_open_time64)
#endif

__typeof (fts_close) __fts_close;
libc_hidden_proto (__fts_close);
__typeof (fts64_close) __fts64_close;
libc_hidden_proto (__fts64_close);
#if __TIMESIZE != 64
extern int __fts64_close_time64 (FTS64_TIME64 *);
libc_hidden_proto (__fts64_close_time64)
#endif

__typeof (fts_read) __fts_read;
libc_hidden_proto (__fts_read);
__typeof (fts64_read) __fts64_read;
libc_hidden_proto (__fts64_read);
#if __TIMESIZE != 64
extern FSTENT64_TIME64* __fts64_read_time64 (FTS64_TIME64 *);
libc_hidden_proto (__fts64_read_time64)
#endif

__typeof (fts_set) __fts_set;
libc_hidden_proto (__fts_set);
__typeof (fts64_set) __fts64_set;
libc_hidden_proto (__fts64_set);
#if __TIMESIZE != 64
extern int __fts64_set_time64 (FTS64_TIME64 *, FSTENT64_TIME64 *, int);
libc_hidden_proto (__fts64_set_time64)
#endif

__typeof (fts_children) __fts_children;
libc_hidden_proto (__fts_children);
__typeof (fts64_children) __fts64_children;
libc_hidden_proto (__fts64_children);
#if __TIMESIZE != 64
extern FSTENT64_TIME64* __fts64_children_time64 (FTS64_TIME64 *, int);
libc_hidden_proto (__fts64_children_time64)
#endif

#endif

#endif /* _FTS_H  */
