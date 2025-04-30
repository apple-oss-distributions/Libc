/*
 * Copyright (c) 2013 Apple Inc. All rights reserved.
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

#ifndef __LIBC_PRIVATE_H__
#define __LIBC_PRIVATE_H__

#include <_bounds.h>
#include <mach/vm_types.h>
#include <os/availability.h>
#include <sys/cdefs.h>
#include <stddef.h>

_LIBC_SINGLE_BY_DEFAULT()

__BEGIN_DECLS

/* Hello and welcome to libc_private.h. This header file has four jobs to do:
 * 1. Included by 54 C source files within the Libc project, for unclear
 *    historical reasons. Most don't use anything from it at all.
 * 2. Declares LIBC_ABORT macro, used by 18 C source files within the Libc
 *    project. This declaration is present when the this file is included from
 *    the Libc project source but is stripped out when the header is installed
 *    to the SDK. (This is fragile and bad.)
 * 3. Declares abort_report_np() used by a handful of projects, chiefly the
 *    many copies of SoftLinking.h that abound. Because SoftLinking uses the
 *    include path <libc_private.h>, we may never be able to move this to
 *    another header that isn't also doing jobs 1 and 2.
 * 4. Declares _atexit_receipt() used by Foundation.
 */

extern int
_atexit_receipt(void);

API_AVAILABLE(macos(10.11), ios(9.0), tvos(9.0), watchos(2.0))
extern void
abort_report_np(const char *, ...) __dead2 __cold __printflike(1, 2);

//Begin-Libc
/* f must be a literal string */
#define LIBC_ABORT(f,...)	abort_report_np("%s:%s:%u: " f, __FILE_NAME__, __func__, __LINE__, ## __VA_ARGS__)
//End-Libc

API_AVAILABLE(macos(12.3), ios(15.4), tvos(15.0), watchos(8.5))
extern void environ_lock_np(void);

API_AVAILABLE(macos(12.3), ios(15.4), tvos(15.4), watchos(8.5))
extern void environ_unlock_np(void);


typedef void (*backtrace_get_pcs_func_t)(vm_address_t *_LIBC_COUNT(max) buffer,
               unsigned max, unsigned *nb, unsigned skip, void *startfp);

API_AVAILABLE(macos(13.3), ios(16.4), tvos(16.4), watchos(9.5))
extern void backtrace_set_pcs_func(backtrace_get_pcs_func_t func);

__END_DECLS

#endif // __LIBC_PRIVATE_H__
