/*
 * Copyright (c) 2008 Apple Inc. All rights reserved.
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

#include <TargetConditionals.h>
#if !TARGET_OS_DRIVERKIT
#include <notify.h>
#include <notify_keys.h>
#else
#define notify_post(...)
#endif
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "_simple.h"

#ifndef kNotifyClockSet
#define kNotifyClockSet "com.apple.system.clock_set"
#endif

int __settimeofday(const struct timeval *tp, const struct timezone *tzp);

/*
 * settimeofday stub, legacy version
 */
int
settimeofday(const struct timeval *tp, const struct timezone *tzp)
{
	int ret = __settimeofday(tp, tzp);
	int serror = 0;
	if (ret == 0) {
		notify_post(kNotifyClockSet);
	} else {
		serror = errno;
	}

	if (tp) {
		char *msg = NULL;
		asprintf(&msg, "settimeofday({%#lx,%#x}) == %d", tp->tv_sec, tp->tv_usec, ret);
		_simple_asl_log(ASL_LEVEL_NOTICE, "com.apple.settimeofday", msg);
		free(msg);
	}

	errno = serror;
	return ret;
}
