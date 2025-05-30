/*
 * Copyright (c) 2000, 2005, 2007, 2009, 2010, 2023 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */
/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
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
 *	@(#)stdio.h	8.5 (Berkeley) 4/29/95
 */

/*
 * Common header for stdio.h and xlocale/_stdio.h
 */

#ifndef	_STDIO_H_
#define	_STDIO_H_
#define	__STDIO_H_

#include <_bounds.h>
#include <sys/cdefs.h>
#include <Availability.h>

#include <_types.h>

/* DO NOT REMOVE THIS COMMENT: fixincludes needs to see:
 * __gnuc_va_list and include <stdarg.h> */
#include <sys/_types/_va_list.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_null.h>

#ifndef UNIFDEF_DRIVERKIT
#include <sys/stdio.h>
#include <_printf.h>

_LIBC_SINGLE_BY_DEFAULT()

typedef __darwin_off_t		fpos_t;

#define	_FSTDIO			/* Define for new stdio with functions. */

/*
 * NB: to fit things in six character monocase externals, the stdio
 * code uses the prefix `__s' for stdio objects, typically followed
 * by a three-character attempt at a mnemonic.
 */

/* stdio buffers */
struct __sbuf {
	unsigned char *_LIBC_COUNT(_size)	_base;
	int		_size;
};

/* hold a buncha junk that would grow the ABI */
struct __sFILEX;

/*
 * stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 *
 * NB: see WARNING above before changing the layout of this structure!
 */
typedef	struct __sFILE {
	unsigned char *_LIBC_UNSAFE_INDEXABLE	_p;	/* current position in (some) buffer */
	int	_r;		/* read space left for getc() */
	int	_w;		/* write space left for putc() */
	short	_flags;		/* flags, below; this FILE is free if 0 */
	short	_file;		/* fileno, if Unix descriptor, else -1 */
	struct	__sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
	int	_lbfsize;	/* 0 or -_bf._size, for inline putc */

	/* operations */
	void	*_cookie;	/* cookie passed to io functions */
	int	(* _Nullable _close)(void *);
	int	(* _Nullable _read) (void *, char *_LIBC_COUNT(__n), int __n);
	fpos_t	(* _Nullable _seek) (void *, fpos_t, int);
	int	(* _Nullable _write)(void *, const char *_LIBC_COUNT(__n), int __n);

	/* separate buffer for long sequences of ungetc() */
	struct	__sbuf _ub;	/* ungetc buffer */
	struct __sFILEX *_extra; /* additions to FILE to not break ABI */
	int	_ur;		/* saved _r when _r is counting ungetc data */

	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */

	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	__sbuf _lb;	/* buffer for fgetln() */

	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
	fpos_t	_offset;	/* current lseek offset (see WARNING) */
} FILE;

#include <sys/_types/_seek_set.h>

__BEGIN_DECLS
extern FILE *__stdinp __swift_nonisolated_unsafe;
extern FILE *__stdoutp __swift_nonisolated_unsafe;
extern FILE *__stderrp __swift_nonisolated_unsafe;
__END_DECLS

#define	__SLBF	0x0001		/* line buffered */
#define	__SNBF	0x0002		/* unbuffered */
#define	__SRD	0x0004		/* OK to read */
#define	__SWR	0x0008		/* OK to write */
	/* RD and WR are never simultaneously asserted */
#define	__SRW	0x0010		/* open for reading & writing */
#define	__SEOF	0x0020		/* found EOF */
#define	__SERR	0x0040		/* found error */
#define	__SMBF	0x0080		/* _buf is from malloc */
#define	__SAPP	0x0100		/* fdopen()ed in append mode */
#define	__SSTR	0x0200		/* this is an sprintf/snprintf string */
#define	__SOPT	0x0400		/* do fseek() optimisation */
#define	__SNPT	0x0800		/* do not do fseek() optimisation */
#define	__SOFF	0x1000		/* set iff _offset is in fact correct */
#define	__SMOD	0x2000		/* true => fgetln modified _p text */
#define __SALC  0x4000		/* allocate string space dynamically */
#define __SIGN  0x8000		/* ignore this file in _fwalk */

/*
 * The following three definitions are for ANSI C, which took them
 * from System V, which brilliantly took internal interface macros and
 * made them official arguments to setvbuf(), without renaming them.
 * Hence, these ugly _IOxxx names are *supposed* to appear in user code.
 *
 * Although numbered as their counterparts above, the implementation
 * does not rely on this.
 */
#define	_IOFBF	0		/* setvbuf should set fully buffered */
#define	_IOLBF	1		/* setvbuf should set line buffered */
#define	_IONBF	2		/* setvbuf should set unbuffered */

#define	BUFSIZ	1024		/* size of buffer used by setbuf */
#define	EOF	(-1)

				/* must be == _POSIX_STREAM_MAX <limits.h> */
#define	FOPEN_MAX	20	/* must be <= OPEN_MAX <sys/syslimits.h> */
#define	FILENAME_MAX	1024	/* must be <= PATH_MAX <sys/syslimits.h> */

/* System V/ANSI C; this is the wrong way to do this, do *not* use these. */
#ifndef _ANSI_SOURCE
#define	P_tmpdir	"/var/tmp/"
#endif
#define	L_tmpnam	1024	/* XXX must be == PATH_MAX */
#define	TMP_MAX		308915776

#define	stdin	__stdinp
#define	stdout	__stdoutp
#define	stderr	__stderrp

#ifdef _DARWIN_UNLIMITED_STREAMS
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
#error "_DARWIN_UNLIMITED_STREAMS specified, but -miphoneos-version-min version does not support it."
#elif defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6
#error "_DARWIN_UNLIMITED_STREAMS specified, but -mmacosx-version-min version does not support it."
#endif
#endif

/* ANSI-C */

__BEGIN_DECLS
void	 clearerr(FILE *);
int	 fclose(FILE *);
int	 feof(FILE *);
int	 ferror(FILE *);
int	 fflush(FILE *);
int	 fgetc(FILE *);
int	 fgetpos(FILE * __restrict, fpos_t *);
char *_LIBC_CSTR	fgets(char * __restrict _LIBC_COUNT(__size), int __size, FILE *);
#if defined(_DARWIN_UNLIMITED_STREAMS) || defined(_DARWIN_C_SOURCE)
FILE	*fopen(const char * __restrict __filename, const char * __restrict __mode) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_3_2, __DARWIN_EXTSN(fopen));
#else /* !_DARWIN_UNLIMITED_STREAMS && !_DARWIN_C_SOURCE */
//Begin-Libc
#ifndef LIBC_ALIAS_FOPEN
//End-Libc
FILE	*fopen(const char * __restrict __filename, const char * __restrict __mode) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_2_0, __DARWIN_ALIAS(fopen));
//Begin-Libc
#else /* LIBC_ALIAS_FOPEN */
FILE	*fopen(const char * __restrict __filename, const char * __restrict __mode) LIBC_ALIAS(fopen);
#endif /* !LIBC_ALIAS_FOPEN */
//End-Libc
#endif /* (DARWIN_UNLIMITED_STREAMS || _DARWIN_C_SOURCE) */
int	 fprintf(FILE * __restrict, const char * __restrict, ...) __printflike(2, 3);
int	 fputc(int, FILE *);
//Begin-Libc
#ifndef LIBC_ALIAS_FPUTS
//End-Libc
int	 fputs(const char * __restrict, FILE * __restrict) __DARWIN_ALIAS(fputs);
//Begin-Libc
#else /* LIBC_ALIAS_FPUTS */
int	 fputs(const char * __restrict, FILE * __restrict) LIBC_ALIAS(fputs);
#endif /* !LIBC_ALIAS_FPUTS */
//End-Libc
size_t	 fread(void * __restrict _LIBC_SIZE(__size * __nitems) __ptr, size_t __size, size_t __nitems, FILE * __restrict __stream);
//Begin-Libc
#ifndef LIBC_ALIAS_FREOPEN
//End-Libc
FILE	*freopen(const char * __restrict, const char * __restrict,
				 FILE * __restrict) __DARWIN_ALIAS(freopen);
//Begin-Libc
#else /* LIBC_ALIAS_FREOPEN */
FILE	*freopen(const char * __restrict, const char * __restrict,
				 FILE * __restrict) LIBC_ALIAS(freopen);
#endif /* !LIBC_ALIAS_FREOPEN */
//End-Libc
int	 fscanf(FILE * __restrict, const char * __restrict, ...) __scanflike(2, 3);
int	 fseek(FILE *, long, int);
int	 fsetpos(FILE *, const fpos_t *);
long	 ftell(FILE *);
//Begin-Libc
#ifndef LIBC_ALIAS_FWRITE
//End-Libc
size_t	 fwrite(const void * __restrict _LIBC_SIZE(__size * __nitems) __ptr, size_t __size, size_t __nitems, FILE * __restrict __stream) __DARWIN_ALIAS(fwrite);
//Begin-Libc
#else /* LIBC_ALIAS_FWRITE */
size_t	 fwrite(const void * __restrict _LIBC_SIZE(__size * __nitems) __ptr, size_t __size, size_t __nitems, FILE * __restrict __stream) LIBC_ALIAS(fwrite);
#endif /* !LIBC_ALIAS_FWRITE */
//End-Libc
int	 getc(FILE *);
int	 getchar(void);

#if !defined(_POSIX_C_SOURCE)
__deprecated_msg("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of gets(3), it is highly recommended that you use fgets(3) instead.")
#endif
char *_LIBC_CSTR	gets(char *_LIBC_UNSAFE_INDEXABLE) _LIBC_PTRCHECK_REPLACED(fgets);

void	 perror(const char *) __cold;
int	 putc(int, FILE *);
int	 putchar(int);
int	 puts(const char *);
int	 remove(const char *);
int	 rename (const char *__old, const char *__new);
void	 rewind(FILE *);
int	 scanf(const char * __restrict, ...) __scanflike(1, 2);
void	 setbuf(FILE * __restrict, char * __restrict _LIBC_COUNT_OR_NULL(BUFSIZ));
int	 setvbuf(FILE * __restrict, char * __restrict _LIBC_COUNT_OR_NULL(__size), int, size_t __size);

__swift_unavailable("Use snprintf instead.")
_LIBC_PTRCHECK_REPLACED("snprintf")
#if !defined(_POSIX_C_SOURCE)
__deprecated_msg("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of sprintf(3), it is highly recommended that you use snprintf(3) instead.")
#endif
int	 sprintf(char * __restrict _LIBC_UNSAFE_INDEXABLE, const char * __restrict, ...) __printflike(2, 3) _LIBC_PTRCHECK_REPLACED(snprintf);

int	 sscanf(const char * __restrict, const char * __restrict, ...) __scanflike(2, 3);
FILE	*tmpfile(void);

__swift_unavailable("Use mkstemp(3) instead.")
#if !defined(_POSIX_C_SOURCE)
__deprecated_msg("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of tmpnam(3), it is highly recommended that you use mkstemp(3) instead.")
#endif
char *_LIBC_CSTR	tmpnam(char *_LIBC_COUNT(L_tmpnam));

int	 ungetc(int, FILE *);
int	 vfprintf(FILE * __restrict, const char * __restrict, va_list) __printflike(2, 0);
int	 vprintf(const char * __restrict, va_list) __printflike(1, 0);

__swift_unavailable("Use vsnprintf instead.")
_LIBC_PTRCHECK_REPLACED("vsnprintf")
#if !defined(_POSIX_C_SOURCE)
__deprecated_msg("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of sprintf(3), it is highly recommended that you use vsnprintf(3) instead.")
#endif
int	 vsprintf(char * __restrict _LIBC_UNSAFE_INDEXABLE, const char * __restrict, va_list) __printflike(2, 0) _LIBC_PTRCHECK_REPLACED(vsnprintf);
__END_DECLS



/* Additional functionality provided by:
 * POSIX.1-1988
 */

#if __DARWIN_C_LEVEL >= 198808L

#include <_ctermid.h>

__BEGIN_DECLS

#if defined(_DARWIN_UNLIMITED_STREAMS) || defined(_DARWIN_C_SOURCE)
FILE	*fdopen(int, const char *) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_3_2, __DARWIN_EXTSN(fdopen));
#else /* !_DARWIN_UNLIMITED_STREAMS && !_DARWIN_C_SOURCE */
//Begin-Libc
#ifndef LIBC_ALIAS_FDOPEN
//End-Libc
FILE	*fdopen(int, const char *) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_2_0, __DARWIN_ALIAS(fdopen));
//Begin-Libc
#else /* LIBC_ALIAS_FDOPEN */
FILE	*fdopen(int, const char *) LIBC_ALIAS(fdopen);
#endif /* !LIBC_ALIAS_FDOPEN */
//End-Libc
#endif /* (DARWIN_UNLIMITED_STREAMS || _DARWIN_C_SOURCE) */
int	 fileno(FILE *);
__END_DECLS
#endif /* __DARWIN_C_LEVEL >= 198808L */


/* Additional functionality provided by:
 * POSIX.2-1992 C Language Binding Option
 */

#if __DARWIN_C_LEVEL >= 199209L
__BEGIN_DECLS
int	 pclose(FILE *) __swift_unavailable("Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)");
#if defined(_DARWIN_UNLIMITED_STREAMS) || defined(_DARWIN_C_SOURCE)
FILE	*popen(const char *, const char *) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_3_2, __DARWIN_EXTSN(popen)) __swift_unavailable("Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)");
#else /* !_DARWIN_UNLIMITED_STREAMS && !_DARWIN_C_SOURCE */
//Begin-Libc
#ifndef LIBC_ALIAS_POPEN
//End-Libc
FILE	*popen(const char *, const char *) __DARWIN_ALIAS_STARTING(__MAC_10_6, __IPHONE_2_0, __DARWIN_ALIAS(popen)) __swift_unavailable("Use posix_spawn APIs or NSTask instead. (On iOS, process spawning is unavailable.)");
//Begin-Libc
#else /* LIBC_ALIAS_POPEN */
FILE	*popen(const char *, const char *) LIBC_ALIAS(popen);
#endif /* !LIBC_ALIAS_POPEN */
//End-Libc
#endif /* (DARWIN_UNLIMITED_STREAMS || _DARWIN_C_SOURCE) */
__END_DECLS
#endif /* __DARWIN_C_LEVEL >= 199209L */

/* Additional functionality provided by:
 * POSIX.1c-1995,
 * POSIX.1i-1995,
 * and the omnibus ISO/IEC 9945-1: 1996
 */

#if __DARWIN_C_LEVEL >= 199506L

/* Functions internal to the implementation. */
__BEGIN_DECLS
int	__srget(FILE *);
int	__svfscanf(FILE *, const char *, va_list) __scanflike(2, 0);
int	__swbuf(int, FILE *);
__END_DECLS

/*
 * The __sfoo macros are here so that we can
 * define function versions in the C library.
 */
#define	__sgetc(p) (--(p)->_r < 0 ? __srget(p) : (int)(*(p)->_p++))
#if defined(__GNUC__) && defined(__STDC__)
__header_always_inline int __sputc(int _c, FILE *_p) {
	if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
		return (*_p->_p++ = _c);
	else
		return (__swbuf(_c, _p));
}
#else
/*
 * This has been tuned to generate reasonable code on the vax using pcc.
 */
#define	__sputc(c, p) \
	(--(p)->_w < 0 ? \
		(p)->_w >= (p)->_lbfsize ? \
			(*(p)->_p = (c)), *(p)->_p != '\n' ? \
				(int)*(p)->_p++ : \
				__swbuf('\n', p) : \
			__swbuf((int)(c), p) : \
		(*(p)->_p = (c), (int)*(p)->_p++))
#endif

#define	__sfeof(p)	(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)	(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)	((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define	__sfileno(p)	((p)->_file)

__BEGIN_DECLS
void	 flockfile(FILE *);
int	 ftrylockfile(FILE *);
void	 funlockfile(FILE *);
int	 getc_unlocked(FILE *);
int	 getchar_unlocked(void);
int	 putc_unlocked(int, FILE *);
int	 putchar_unlocked(int);

/* Removed in Issue 6 */
#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
int	 getw(FILE *);
int	 putw(int, FILE *);
#endif

__swift_unavailable("Use mkstemp(3) instead.")
#if !defined(_POSIX_C_SOURCE)
__deprecated_msg("This function is provided for compatibility reasons only.  Due to security concerns inherent in the design of tempnam(3), it is highly recommended that you use mkstemp(3) instead.")
#endif
//Begin-Libc
#ifndef LIBC_ALIAS_TEMPNAM
//End-Libc
char *_LIBC_CSTR	tempnam(const char *__dir, const char *__prefix) __DARWIN_ALIAS(tempnam);
//Begin-Libc
#else /* LIBC_ALIAS_TEMPNAM */
char *_LIBC_CSTR	tempnam(const char *__dir, const char *__prefix) LIBC_ALIAS(tempnam);
#endif /* !LIBC_ALIAS_TEMPNAM */
//End-Libc
__END_DECLS

#ifndef lint
#define	getc_unlocked(fp)	__sgetc(fp)
#define putc_unlocked(x, fp)	__sputc(x, fp)
#endif /* lint */

#define	getchar_unlocked()	getc_unlocked(stdin)
#define	putchar_unlocked(x)	putc_unlocked(x, stdout)
#endif /* __DARWIN_C_LEVEL >= 199506L */



/* Additional functionality provided by:
 * POSIX.1-2001
 * ISO C99
 */

#if __DARWIN_C_LEVEL >= 200112L
#include <sys/_types/_off_t.h>

__BEGIN_DECLS
int	 fseeko(FILE * __stream, off_t __offset, int __whence);
off_t	 ftello(FILE * __stream);
__END_DECLS
#endif /* __DARWIN_C_LEVEL >= 200112L */

#if __DARWIN_C_LEVEL >= 200112L || defined(_C99_SOURCE) || defined(__cplusplus)
__BEGIN_DECLS
int	 snprintf(char * __restrict _LIBC_COUNT(__size) __str, size_t __size, const char * __restrict __format, ...) __printflike(3, 4);
int	 vfscanf(FILE * __restrict __stream, const char * __restrict __format, va_list) __scanflike(2, 0);
int	 vscanf(const char * __restrict __format, va_list) __scanflike(1, 0);
int	 vsnprintf(char * __restrict _LIBC_COUNT(__size) __str, size_t __size, const char * __restrict __format, va_list) __printflike(3, 0);
int	 vsscanf(const char * __restrict __str, const char * __restrict __format, va_list) __scanflike(2, 0);
__END_DECLS
#endif /* __DARWIN_C_LEVEL >= 200112L || defined(_C99_SOURCE) || defined(__cplusplus) */



/* Additional functionality provided by:
 * POSIX.1-2008
 */

#if __DARWIN_C_LEVEL >= 200809L
#include <sys/_types/_ssize_t.h>

__BEGIN_DECLS
int	dprintf(int, const char * __restrict, ...) __printflike(2, 3) __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);
int	vdprintf(int, const char * __restrict, va_list) __printflike(2, 0) __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);
ssize_t getdelim(char *_LIBC_COUNT(*__linecapp) *__restrict __linep, size_t * __restrict __linecapp, int __delimiter, FILE * __restrict __stream) __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);
ssize_t getline(char *_LIBC_COUNT(*__linecapp) *__restrict __linep, size_t * __restrict __linecapp, FILE * __restrict __stream) __OSX_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_4_3);
FILE *fmemopen(void * __restrict __buf _LIBC_SIZE(__size), size_t __size, const char * __restrict __mode) __API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));
FILE *open_memstream(char *_LIBC_COUNT(*__sizep) *__bufp, size_t *__sizep) __API_AVAILABLE(macos(10.13), ios(11.0), tvos(11.0), watchos(4.0));
__END_DECLS
#endif /* __DARWIN_C_LEVEL >= 200809L */



/* Darwin extensions */

#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
__BEGIN_DECLS
extern __const int sys_nerr;		/* perror(3) external variables */
extern __const char *__const sys_errlist[];

int	 asprintf(char *_LIBC_CSTR *__restrict, const char * __restrict, ...) __printflike(2, 3);
char *_LIBC_CSTR	ctermid_r(char *_LIBC_COUNT(L_ctermid));
char *_LIBC_COUNT(*__len)	fgetln(FILE *, size_t *__len);
__const char *fmtcheck(const char *, const char *) __attribute__((format_arg(2)));
int	 fpurge(FILE *);
void	 setbuffer(FILE *, char *_LIBC_COUNT_OR_NULL(__size), int __size);
int	 setlinebuf(FILE *);
int	 vasprintf(char *_LIBC_CSTR *__restrict, const char * __restrict, va_list) __printflike(2, 0);


/*
 * Stdio function-access interface.
 */
FILE	*funopen(const void *,
				 int (* _Nullable)(void *, char *_LIBC_COUNT(__n), int __n),
				 int (* _Nullable)(void *, const char *_LIBC_COUNT(__n), int __n),
				 fpos_t (* _Nullable)(void *, fpos_t, int),
				 int (* _Nullable)(void *));
__END_DECLS
#define	fropen(cookie, fn) funopen(cookie, fn, 0, 0, 0)
#define	fwopen(cookie, fn) funopen(cookie, 0, fn, 0, 0)

#define	feof_unlocked(p)	__sfeof(p)
#define	ferror_unlocked(p)	__sferror(p)
#define	clearerr_unlocked(p)	__sclearerr(p)
#define	fileno_unlocked(p)	__sfileno(p)

#endif /* __DARWIN_C_LEVEL >= __DARWIN_C_FULL */

#else /* UNIFDEF_DRIVERKIT */
#define	EOF	(-1)

__BEGIN_DECLS
int	 sscanf(const char * __restrict, const char * __restrict, ...) __scanflike(2, 3);
#if __DARWIN_C_LEVEL >= 200112L || defined(_C99_SOURCE) || defined(__cplusplus)
int	 snprintf(char * __restrict _LIBC_COUNT(__size) __str, size_t __size, const char * __restrict __format, ...) __printflike(3, 4);
int	 vsnprintf(char * __restrict _LIBC_COUNT(__size) __str, size_t __size, const char * __restrict __format, va_list) __printflike(3, 0);
int	 vsscanf(const char * __restrict __str, const char * __restrict __format, va_list) __scanflike(2, 0);
#endif /* __DARWIN_C_LEVEL >= 200112L || defined(_C99_SOURCE) || defined(__cplusplus) */
#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
int	 asprintf(char *_LIBC_CSTR *__restrict, const char * __restrict, ...) __printflike(2, 3);
int	 vasprintf(char *_LIBC_CSTR *__restrict, const char * __restrict, va_list) __printflike(2, 0);
#endif /* __DARWIN_C_LEVEL >= __DARWIN_C_FULL */
__END_DECLS
#endif /* UNIFDEF_DRIVERKIT */

#if defined (__GNUC__) && _FORTIFY_SOURCE > 0 && !defined (__cplusplus)
/* Security checking functions.  */
#include <secure/_stdio.h>
#endif

#endif /* _STDIO_H_ */
