/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
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

LEAF(_fork, 0)
	subl  $28, %esp   // Align the stack, with 16 bytes of extra padding that we'll need
	CALL_EXTERN(__cthread_fork_prepare)
#if defined(__DYNAMIC__)
// Just like __cthread_fork_prepare we need to prevent threads on the child's
// side from doing a mach call in the dynamic linker until __dyld_fork_child
// is run (see below).  So we call __dyld_fork_prepare which takes out the dyld
// lock to prevent all other threads but this one from entering dyld.
.cstring
LC1:
	.ascii "__dyld_fork_prepare\0"
.text
	// Put a pointer to 8(%esp) in 4(%esp) for _dyld_func_lookup to fill in.
	leal	0x8(%esp),%eax	// get the address where we're going to store the pointer
	movl	%eax, 0x4(%esp)	// copy the address of the pointer
	call	1f
1:	popl	%eax
	leal	LC1-1b(%eax),%eax
	movl 	%eax, 0x0(%esp)	// copy the name of the function to look up
	call 	__dyld_func_lookup
	movl	0x8(%esp),%eax	// move the value returned in address parameter
	call	*%eax		// call __dyld_fork_prepare indirectly
#endif

	movl 	$ SYS_fork,%eax; 	// code for fork -> eax
	UNIX_SYSCALL_TRAP		// do the system call
	jnc	L1			// jump if CF==0

#if defined(__DYNAMIC__)
// __dyld_fork_parent() is called by the parent process after a fork syscall.
// This releases the dyld lock acquired by __dyld_fork_prepare().  In this case
// we just use it to clean up after a fork error so the parent process can 
// dyld after fork() errors without deadlocking.
.cstring
LC2:
	.ascii "__dyld_fork_parent\0"
.text
	movl	%eax, 0xc(%esp)		// save the return value (errno)
	leal	0x8(%esp),%eax		// get the address where we're going to store the pointer
	movl	%eax, 0x4(%esp)		// copy the address of the pointer
	call	1f
1:	popl	%eax
	leal	LC2-1b(%eax),%eax
	movl 	%eax, 0x0(%esp)		// copy the name of the function to look up
	call 	__dyld_func_lookup
	movl	0x8(%esp),%eax		// move the value returned in address parameter
	call	*%eax		// call __dyld_fork_parent indirectly
	movl	0xc(%esp), %eax		// restore the return value (errno)
#endif
	CALL_EXTERN(cerror)
	CALL_EXTERN(__cthread_fork_parent)
	movl	$-1,%eax
	addl	$28, %esp   // restore the stack
	ret
	
L1:
	orl	%edx,%edx	// CF=OF=0,  ZF set if zero result	
	jz	L2		// parent, since r1 == 0 in parent, 1 in child
	
	//child here...
#if defined(__DYNAMIC__)
// Here on the child side of the fork we need to tell the dynamic linker that
// we have forked.  To do this we call __dyld_fork_child in the dyanmic
// linker.  But since we can't dynamically bind anything until this is done we
// do this by using the private extern __dyld_func_lookup() function to get the
// address of __dyld_fork_child (the 'C' code equivlent):
//
//	_dyld_func_lookup("__dyld_fork_child", &address);
//	address();
//
.cstring
LC0:
	.ascii "__dyld_fork_child\0"

.text
	leal	0x8(%esp),%eax		// get the address where we're going to store the pointer
	movl	%eax, 0x4(%esp)		// copy the address of the pointer
	call	1f
1:	popl	%eax
	leal	LC0-1b(%eax),%eax
	movl 	%eax, 0x0(%esp)		// copy the name of the function to look up
	call 	__dyld_func_lookup
	movl	0x8(%esp),%eax		// move the value returned in address parameter
	call	*%eax		// call __dyld_fork_child indirectly
#endif
	xorl	%eax, %eax
	REG_TO_EXTERN(%eax, __current_pid)
	CALL_EXTERN(__cthread_fork_child)
#if	defined(__DYNAMIC__)
.cstring
LC10:
	.ascii "__dyld_fork_child_final\0"

.text
	leal	0x8(%esp),%eax		// get the address where we're going to store the pointer
	movl	%eax, 0x4(%esp)		// copy the address of the pointer
	call	1f
1:	popl	%eax
	leal	LC10-1b(%eax),%eax
	movl 	%eax, 0x0(%esp)		// copy the name of the function to look up
	call 	__dyld_func_lookup
	movl	0x8(%esp),%eax		// move the value returned in address parameter
	call	*%eax		// call __dyld_fork_child_final indirectly
#endif
	xorl	%eax,%eax	// zero eax
	addl	$28, %esp   // restore the stack
	ret

	//parent here...
L2:
	movl	%eax, 0xc(%esp)		// save pid
#if	defined(__DYNAMIC__)
// __dyld_fork_parent() is called by the parent process after a fork syscall.
// This releases the dyld lock acquired by __dyld_fork_prepare().
	leal	0x8(%esp),%eax		// get the address where we're going to store the pointer
	movl	%eax, 0x4(%esp)		// copy the address of the allocated space
	call	1f
1:	popl	%eax
	leal	LC2-1b(%eax),%eax
	movl 	%eax, 0x0(%esp)		// copy the name of the function to look up
	call 	__dyld_func_lookup
	movl	0x8(%esp),%eax		// move the value returned in address parameter
	call	*%eax		// call __dyld_fork_parent indirectly
#endif
	CALL_EXTERN_AGAIN(__cthread_fork_parent)
	movl	0xc(%esp), %eax		// return pid
	addl	$28, %esp   // restore the stack
	ret		

