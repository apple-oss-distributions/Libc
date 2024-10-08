/*
 * Copyright (c) 2000-2014 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/* Copyright (c) 1995 NeXT Computer, Inc. All Rights Reserved */
/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stat.h	8.9 (Berkeley) 8/17/94
 */


#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <time.h>

struct stat {
    dev_t           st_dev;         /* [XSI] ID of device containing file */
    ino_t           st_ino;         /* [XSI] File serial number */
    mode_t          st_mode;        /* [XSI] Mode of file (see below) */
    nlink_t         st_nlink;       /* [XSI] Number of hard links */
    uid_t           st_uid;         /* [XSI] User ID of the file */
    gid_t           st_gid;         /* [XSI] Group ID of the file */
    dev_t           st_rdev;        /* [XSI] Device ID */
    struct timespec st_atimespec;   /* time of last access */
    struct timespec st_mtimespec;   /* time of last data modification */
    struct timespec st_ctimespec;   /* time of last file status change */
    off_t           st_size;        /* [XSI] file size, in bytes */
    blkcnt_t        st_blocks;      /* [XSI] blocks allocated for file */
    blksize_t       st_blksize;     /* [XSI] optimal blocksize for I/O */
    uint32_t        st_flags;       /* user defined flags for file */
    uint32_t        st_gen;         /* file generation number */
    int32_t         st_lspare;      /* RESERVED: DO NOT USE! */
    int64_t         st_qspare[2];   /* RESERVED: DO NOT USE! */
};

#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define st_atime st_atimespec.tv_sec
#define st_mtime st_mtimespec.tv_sec
#define st_ctime st_ctimespec.tv_sec
#define st_birthtime st_birthtimespec.tv_sec
#endif /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */

/*
 * [XSI] The following are symbolic names for the values of type mode_t.  They
 * are bitmap values.
 */
/*
 * [XSI] The symbolic names for file modes for use as values of mode_t
 * shall be defined as described in <sys/stat.h>
 */
#ifndef S_IFMT
/* File type */
#define S_IFMT          0170000         /* [XSI] type of file mask */
#define S_IFIFO         0010000         /* [XSI] named pipe (fifo) */
#define S_IFCHR         0020000         /* [XSI] character special */
#define S_IFDIR         0040000         /* [XSI] directory */
#define S_IFBLK         0060000         /* [XSI] block special */
#define S_IFREG         0100000         /* [XSI] regular */
#define S_IFLNK         0120000         /* [XSI] symbolic link */
#define S_IFSOCK        0140000         /* [XSI] socket */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define S_IFWHT         0160000         /* OBSOLETE: whiteout */
#endif

/* File mode */
/* Read, write, execute/search by owner */
#define S_IRWXU         0000700         /* [XSI] RWX mask for owner */
#define S_IRUSR         0000400         /* [XSI] R for owner */
#define S_IWUSR         0000200         /* [XSI] W for owner */
#define S_IXUSR         0000100         /* [XSI] X for owner */
/* Read, write, execute/search by group */
#define S_IRWXG         0000070         /* [XSI] RWX mask for group */
#define S_IRGRP         0000040         /* [XSI] R for group */
#define S_IWGRP         0000020         /* [XSI] W for group */
#define S_IXGRP         0000010         /* [XSI] X for group */
/* Read, write, execute/search by others */
#define S_IRWXO         0000007         /* [XSI] RWX mask for other */
#define S_IROTH         0000004         /* [XSI] R for other */
#define S_IWOTH         0000002         /* [XSI] W for other */
#define S_IXOTH         0000001         /* [XSI] X for other */

#define S_ISUID         0004000         /* [XSI] set user id on execution */
#define S_ISGID         0002000         /* [XSI] set group id on execution */
#define S_ISVTX         0001000         /* [XSI] directory restrcted delete */

#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define S_ISTXT         S_ISVTX         /* sticky bit: not supported */
#define S_IREAD         S_IRUSR         /* backward compatability */
#define S_IWRITE        S_IWUSR         /* backward compatability */
#define S_IEXEC         S_IXUSR         /* backward compatability */
#endif
#endif  /* !S_IFMT */

/*
 * [XSI] The following macros shall be provided to test whether a file is
 * of the specified type.  The value m supplied to the macros is the value
 * of st_mode from a stat structure.  The macro shall evaluate to a non-zero
 * value if the test is true; 0 if the test is false.
 */
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)     /* block special */
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)     /* char special */
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)     /* directory */
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)     /* fifo or socket */
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)     /* regular file */
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)     /* symbolic link */
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)    /* socket */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define S_ISWHT(m)      (((m) & S_IFMT) == S_IFWHT)     /* OBSOLETE: whiteout */
#endif

/*
 * [XSI] The implementation may implement message queues, semaphores, or
 * shared memory objects as distinct file types.  The following macros
 * shall be provided to test whether a file is of the specified type.
 * The value of the buf argument supplied to the macros is a pointer to
 * a stat structure.  The macro shall evaluate to a non-zero value if
 * the specified object is implemented as a distinct file type and the
 * specified file type is contained in the stat structure referenced by
 * buf.  Otherwise, the macro shall evaluate to zero.
 *
 * NOTE:	The current implementation does not do this, although
 *		this may change in future revisions, and co currently only
 *		provides these macros to ensure source compatability with
 *		implementations which do.
 */
#define S_TYPEISMQ(buf)         (0)     /* Test for a message queue */
#define S_TYPEISSEM(buf)        (0)     /* Test for a semaphore */
#define S_TYPEISSHM(buf)        (0)     /* Test for a shared memory object */

/*
 * [TYM] The implementation may implement typed memory objects as distinct
 * file types, and the following macro shall test whether a file is of the
 * specified type.  The value of the buf argument supplied to the macros is
 * a pointer to a stat structure.  The macro shall evaluate to a non-zero
 * value if the specified object is implemented as a distinct file type and
 * the specified file type is contained in the stat structure referenced by
 * buf.  Otherwise, the macro shall evaluate to zero.
 *
 * NOTE:	The current implementation does not do this, although
 *		this may change in future revisions, and co currently only
 *		provides this macro to ensure source compatability with
 *		implementations which do.
 */
#define S_TYPEISTMO(buf)        (0)     /* Test for a typed memory object */


#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define ACCESSPERMS     (S_IRWXU|S_IRWXG|S_IRWXO)       /* 0777 */
                                                        /* 7777 */
#define ALLPERMS        (S_ISUID|S_ISGID|S_ISTXT|S_IRWXU|S_IRWXG|S_IRWXO)
/* 0666 */
#define DEFFILEMODE     (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

#define S_BLKSIZE       512             /* block size used in the stat struct */

/*
 * Definitions of flags stored in file flags word.
 *
 * Super-user and owner changeable flags.
 */
#define UF_SETTABLE     0x0000ffff      /* mask of owner changeable flags */
#define UF_NODUMP       0x00000001      /* do not dump file */
#define UF_IMMUTABLE    0x00000002      /* file may not be changed */
#define UF_APPEND       0x00000004      /* writes to file may only append */
#define UF_OPAQUE       0x00000008      /* directory is opaque wrt. union */
/*
 * The following bit is reserved for FreeBSD.  It is not implemented
 * in Mac OS X.
 */
/* #define UF_NOUNLINK	0x00000010 */	/* file may not be removed or renamed */
#define UF_COMPRESSED   0x00000020      /* file is compressed (some file-systems) */

/* UF_TRACKED is used for dealing with document IDs.  We no longer issue
 *  notifications for deletes or renames for files which have UF_TRACKED set. */
#define UF_TRACKED      0x00000040

#define UF_DATAVAULT    0x00000080      /* entitlement required for reading */
                                        /* and writing */

/* Bits 0x0100 through 0x4000 are currently undefined. */
#define UF_HIDDEN       0x00008000      /* hint that this item should not be */
                                        /* displayed in a GUI */
/*
 * Super-user changeable flags.
 */
#define SF_SUPPORTED    0x009f0000      /* mask of superuser supported flags */
#define SF_SETTABLE     0x3fff0000      /* mask of superuser changeable flags */
#define SF_SYNTHETIC    0xc0000000      /* mask of system read-only synthetic flags */
#define SF_ARCHIVED     0x00010000      /* file is archived */
#define SF_IMMUTABLE    0x00020000      /* file may not be changed */
#define SF_APPEND       0x00040000      /* writes to file may only append */
#define SF_RESTRICTED   0x00080000      /* entitlement required for writing */
#define SF_NOUNLINK     0x00100000      /* Item may not be removed, renamed or mounted on */

/*
 * The following two bits are reserved for FreeBSD.  They are not
 * implemented in Mac OS X.
 */
/* #define SF_SNAPSHOT	0x00200000 */	/* snapshot inode */
/* NOTE: There is no SF_HIDDEN bit. */

#define SF_FIRMLINK     0x00800000      /* file is a firmlink */

/*
 * Synthetic flags.
 *
 * These are read-only.  We keep them out of SF_SUPPORTED so that
 * attempts to set them will fail.
 */
#define SF_DATALESS     0x40000000     /* file is dataless object */


#endif

#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
/*
 * Extended flags ("EF") returned by ATTR_CMNEXT_EXT_FLAGS from getattrlist/getattrlistbulk
 */
#define EF_MAY_SHARE_BLOCKS     0x00000001      /* file may share blocks with another file */
#define EF_NO_XATTRS            0x00000002      /* file has no xattrs at all */
#define EF_IS_SYNC_ROOT         0x00000004      /* file is a sync root for iCloud */
#define EF_IS_PURGEABLE         0x00000008      /* file is purgeable */
#define EF_IS_SPARSE            0x00000010      /* file has at least one sparse region */
#define EF_IS_SYNTHETIC         0x00000020      /* a synthetic directory/symlink */
#define EF_SHARES_ALL_BLOCKS    0x00000040      /* file shares all of its blocks with another file */
#endif

#if defined(ENABLE_EXCLAVE_STORAGE)

__BEGIN_DECLS

int     fstat(int, struct stat *);
int     lstat(const char *, struct stat *);
int     stat(const char *, struct stat *);

/* No-op shims for compatibility */
int     chmod(const char *, mode_t);
int     fchmod(int, mode_t);
int     mkdir(const char *, mode_t);
int     mkfifo(const char *, mode_t);
int     mknod(const char *, mode_t, dev_t);

__END_DECLS

#endif /* ENABLE_EXCLAVE_STORAGE */

#endif /* !_SYS_STAT_H_ */
