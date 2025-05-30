/*
 * Copyright (c) 2023 Apple Computer, Inc. All rights reserved.
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
/*
 * Copyright (c) 1989, 1993
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
 *	@(#)time.h	8.3 (Berkeley) 1/21/94
 */

#ifndef _TIME_H_
#define	_TIME_H_

#include <_types.h>
#include <sys/cdefs.h>
#include <_bounds.h>
#include <Availability.h>
#ifndef UNIFDEF_DRIVERKIT
#include <sys/_types/_clock_t.h>
#include <sys/_types/_null.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_time_t.h>
#include <sys/_types/_timespec.h>

_LIBC_SINGLE_BY_DEFAULT()

struct tm {
	int	tm_sec;		/* seconds after the minute [0-60] */
	int	tm_min;		/* minutes after the hour [0-59] */
	int	tm_hour;	/* hours since midnight [0-23] */
	int	tm_mday;	/* day of the month [1-31] */
	int	tm_mon;		/* months since January [0-11] */
	int	tm_year;	/* years since 1900 */
	int	tm_wday;	/* days since Sunday [0-6] */
	int	tm_yday;	/* days since January 1 [0-365] */
	int	tm_isdst;	/* Daylight Savings Time flag */
	long	tm_gmtoff;	/* offset from UTC in seconds */
	char	*_LIBC_CSTR	tm_zone;	/* timezone abbreviation */
};

#if __DARWIN_UNIX03
#define CLOCKS_PER_SEC  ((clock_t)1000000)	/* [XSI] */
#else /* !__DARWIN_UNIX03 */
#include <machine/_limits.h>	/* Include file containing CLK_TCK. */

#define CLOCKS_PER_SEC  ((clock_t)(__DARWIN_CLK_TCK))
#endif /* __DARWIN_UNIX03 */

#ifndef _ANSI_SOURCE
extern char *_LIBC_CSTR	tzname[_LIBC_COUNT(2)];
#endif

extern int getdate_err;
#if __DARWIN_UNIX03
//Begin-Libc
#ifndef LIBC_ALIAS_TIMEZONE
//End-Libc
extern long timezone __DARWIN_ALIAS(timezone);
//Begin-Libc
#else /* LIBC_ALIAS_TIMEZONE */
extern long timezone LIBC_ALIAS(timezone);
#endif /* !LIBC_ALIAS_TIMEZONE */
//End-Libc
#endif /* __DARWIN_UNIX03 */
extern int daylight;
#else /* UNIFDEF_DRIVERKIT */
#include <sys/_types/_time_t.h>
#endif /* UNIFDEF_DRIVERKIT */

__BEGIN_DECLS
#ifndef UNIFDEF_DRIVERKIT
char *_LIBC_CSTR	asctime(const struct tm *);
//Begin-Libc
#ifndef LIBC_ALIAS_CLOCK
//End-Libc
clock_t clock(void) __DARWIN_ALIAS(clock);
//Begin-Libc
#else /* LIBC_ALIAS_CLOCK */
clock_t clock(void) LIBC_ALIAS(clock);
#endif /* !LIBC_ALIAS_CLOCK */
//End-Libc
char *_LIBC_CSTR	ctime(const time_t *);
double difftime(time_t, time_t);
struct tm *getdate(const char *);
struct tm *gmtime(const time_t *);
struct tm *localtime(const time_t *);
//Begin-Libc
#ifndef LIBC_ALIAS_MKTIME
//End-Libc
time_t mktime(struct tm *) __DARWIN_ALIAS(mktime);
//Begin-Libc
#else /* LIBC_ALIAS_MKTIME */
time_t mktime(struct tm *) LIBC_ALIAS(mktime);
#endif /* !LIBC_ALIAS_MKTIME */
//End-Libc
//Begin-Libc
#ifndef LIBC_ALIAS_STRFTIME
//End-Libc
size_t strftime(char *_LIBC_COUNT(__maxsize) __restrict, size_t __maxsize, const char * __restrict, const struct tm * __restrict) __DARWIN_ALIAS(strftime);
//Begin-Libc
#else /* LIBC_ALIAS_STRFTIME */
size_t strftime(char *_LIBC_COUNT(__maxsize) __restrict, size_t __maxsize, const char * __restrict, const struct tm * __restrict) LIBC_ALIAS(strftime);
#endif /* !LIBC_ALIAS_STRFTIME */
//End-Libc
//Begin-Libc
#ifndef LIBC_ALIAS_STRPTIME
//End-Libc
char *_LIBC_CSTR	strptime(const char * __restrict, const char * __restrict, struct tm * __restrict) __DARWIN_ALIAS(strptime);
//Begin-Libc
#else /* LIBC_ALIAS_STRPTIME */
char *_LIBC_CSTR	strptime(const char * __restrict, const char * __restrict, struct tm * __restrict) LIBC_ALIAS(strptime);
#endif /* !LIBC_ALIAS_STRPTIME */
//End-Libc
time_t time(time_t *);

#ifndef _ANSI_SOURCE
void tzset(void);
#endif /* not ANSI */

/* [TSF] Thread safe functions */
char *_LIBC_CSTR asctime_r(const struct tm * __restrict, char * __restrict _LIBC_COUNT(26));
char *_LIBC_CSTR ctime_r(const time_t *, char *_LIBC_COUNT(26));
struct tm *gmtime_r(const time_t * __restrict, struct tm * __restrict);
struct tm *localtime_r(const time_t * __restrict, struct tm * __restrict);

#if !defined(_ANSI_SOURCE) && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
time_t posix2time(time_t);
#if !__DARWIN_UNIX03
char *_LIBC_CSTR timezone(int, int);
#endif /* !__DARWIN_UNIX03 */
void tzsetwall(void);
time_t time2posix(time_t);
time_t timelocal(struct tm * const);
time_t timegm(struct tm * const);
#endif /* neither ANSI nor POSIX */

#if !defined(_ANSI_SOURCE)
//Begin-Libc
#ifndef LIBC_ALIAS_NANOSLEEP
//End-Libc
int nanosleep(const struct timespec *__rqtp, struct timespec *__rmtp) __DARWIN_ALIAS_C(nanosleep);
//Begin-Libc
#else /* LIBC_ALIAS_NANOSLEEP */
int nanosleep(const struct timespec *__rqtp, struct timespec *__rmtp) LIBC_ALIAS_C(nanosleep);
#endif /* !LIBC_ALIAS_NANOSLEEP */
//End-Libc
#endif
#endif /* UNIFDEF_DRIVERKIT */

#if !defined(_DARWIN_FEATURE_CLOCK_GETTIME) || _DARWIN_FEATURE_CLOCK_GETTIME != 0
#if __DARWIN_C_LEVEL >= 199309L
#if __has_feature(enumerator_attributes)
#define __CLOCK_AVAILABILITY __OSX_AVAILABLE(10.12) __IOS_AVAILABLE(10.0) __TVOS_AVAILABLE(10.0) __WATCHOS_AVAILABLE(3.0)
#else
#define __CLOCK_AVAILABILITY
#endif

typedef enum {
_CLOCK_REALTIME __CLOCK_AVAILABILITY = 0,
#define CLOCK_REALTIME _CLOCK_REALTIME
_CLOCK_MONOTONIC __CLOCK_AVAILABILITY = 6,
#define CLOCK_MONOTONIC _CLOCK_MONOTONIC
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
_CLOCK_MONOTONIC_RAW __CLOCK_AVAILABILITY = 4,
#define CLOCK_MONOTONIC_RAW _CLOCK_MONOTONIC_RAW
_CLOCK_MONOTONIC_RAW_APPROX __CLOCK_AVAILABILITY = 5,
#define CLOCK_MONOTONIC_RAW_APPROX _CLOCK_MONOTONIC_RAW_APPROX
_CLOCK_UPTIME_RAW __CLOCK_AVAILABILITY = 8,
#define CLOCK_UPTIME_RAW _CLOCK_UPTIME_RAW
_CLOCK_UPTIME_RAW_APPROX __CLOCK_AVAILABILITY = 9,
#define CLOCK_UPTIME_RAW_APPROX _CLOCK_UPTIME_RAW_APPROX
#endif
_CLOCK_PROCESS_CPUTIME_ID __CLOCK_AVAILABILITY = 12,
#define CLOCK_PROCESS_CPUTIME_ID _CLOCK_PROCESS_CPUTIME_ID
_CLOCK_THREAD_CPUTIME_ID __CLOCK_AVAILABILITY = 16
#define CLOCK_THREAD_CPUTIME_ID _CLOCK_THREAD_CPUTIME_ID
} clockid_t;

#ifndef UNIFDEF_DRIVERKIT
__CLOCK_AVAILABILITY
int clock_getres(clockid_t __clock_id, struct timespec *__res);

__CLOCK_AVAILABILITY
int clock_gettime(clockid_t __clock_id, struct timespec *__tp);

#endif /* UNIFDEF_DRIVERKIT */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
__CLOCK_AVAILABILITY
__uint64_t clock_gettime_nsec_np(clockid_t __clock_id);
#endif

#ifndef UNIFDEF_DRIVERKIT
__OSX_AVAILABLE(10.12) __IOS_PROHIBITED
__TVOS_PROHIBITED __WATCHOS_PROHIBITED
int clock_settime(clockid_t __clock_id, const struct timespec *__tp);

#endif /* UNIFDEF_DRIVERKIT */
#undef __CLOCK_AVAILABILITY
#endif /* __DARWIN_C_LEVEL */
#endif /* _DARWIN_FEATURE_CLOCK_GETTIME */

#ifndef UNIFDEF_DRIVERKIT
#if (__DARWIN_C_LEVEL >= __DARWIN_C_FULL) || \
        (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
        (defined(__cplusplus) && __cplusplus >= 201703L)
/* ISO/IEC 9899:201x 7.27.2.5 The timespec_get function */
#define TIME_UTC	1	/* time elapsed since epoch */
__API_AVAILABLE(macosx(10.15), ios(13.0), tvos(13.0), watchos(6.0))
int timespec_get(struct timespec *ts, int base);
#endif

#endif /* UNIFDEF_DRIVERKIT */
__END_DECLS

#endif /* !_TIME_H_ */
