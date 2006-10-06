/*
 * Copyright (c) 1999-2003 Apple Computer, Inc. All rights reserved.
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
 * Copyright (c) 1995 NeXT Computer, Inc. All Rights Reserved
 */
#include <SYS.h>

/*
 * If __current_pid >= 0, we want to put a -1 in there
 * otherwise we just decrement it
 */

LEAF(_vfork, 0)
	movq		__current_pid@GOTPCREL(%rip), %rax
	movl		(%rax), %eax
0:
	xorl		%ecx, %ecx
	testl		%eax, %eax
	cmovs		%eax, %ecx
	subl		$1, %ecx
	movq		__current_pid@GOTPCREL(%rip), %rdx
	lock
	cmpxchgl	%ecx, (%rdx)
	jne		0b
	popq		%rdi			// return address in %rdi
	movq		$ SYSCALL_CONSTRUCT_UNIX(SYS_vfork), %rax	// code for vfork -> rax
	UNIX_SYSCALL_TRAP			// do the system call
	jnb		L1					// jump if CF==0
	movq		__current_pid@GOTPCREL(%rip), %rcx
	lock
	addq		$1, (%rcx)
	movq		(%rcx), %rdi
	BRANCH_EXTERN(cerror)

L1:
	testl		%edx, %edx		// CF=OF=0,  ZF set if zero result
	jz		L2			// parent, since r1 == 0 in parent, 1 in child
	xorq		%rax, %rax		// zero rax
	jmp		*%rdi

L2:
	movq		__current_pid@GOTPCREL(%rip), %rdx
	lock
	addq		$1, (%rdx)
	jmp		*%rdi
