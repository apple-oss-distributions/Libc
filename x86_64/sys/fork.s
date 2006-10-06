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
	subq  $24, %rsp   // Align the stack, plus room for local storage
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
	// Give a pointer to 8(%rsp) to _dyld_func_lookup for it to fill in.
	leaq	8(%rsp),%rsi	// get the address where we're going to store the pointer
	leaq	LC1(%rip), %rdi	// copy the name of the function to look up
	call 	__dyld_func_lookup
	movq	8(%rsp),%rax	// move the value returned in address parameter
	call	*%rax		// call __dyld_fork_prepare
#endif

	movl 	$ SYSCALL_CONSTRUCT_UNIX(SYS_fork),%eax; // code for fork -> rax
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
	movq	%rax, 16(%rsp)		// save the return value (errno)
	leaq	8(%rsp),%rsi		// get the address where we're going to store the pointer
	leaq	LC2(%rip), %rdi		// copy the name of the function to look up
	call 	__dyld_func_lookup
	call	*8(%rsp)			// call __dyld_fork_parent indirectly
	movq	16(%rsp), %rax		// restore the return value (errno)
#endif
	CALL_EXTERN(cerror)
	CALL_EXTERN(__cthread_fork_parent)
	movq	$-1, %rax
	addq	$24, %rsp   // restore the stack
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
	leaq	8(%rsp),%rsi		// get the address where we're going to store the pointer
	leaq 	LC0(%rip), %rdi		// copy the name of the function to look up
	call 	__dyld_func_lookup
	call	*8(%rsp)		// call __dyld_fork_child indirectly
#endif
	xorq	%rax, %rax
	REG_TO_EXTERN(%rax, __current_pid)
	CALL_EXTERN(__cthread_fork_child)
#if	defined(__DYNAMIC__)
.cstring
LC10:
	.ascii "__dyld_fork_child_final\0"

.text
	leaq	8(%rsp),%rsi		// get the address where we're going to store the pointer
	leaq 	LC10(%rip), %rdi		// copy the name of the function to look up
	call 	__dyld_func_lookup
	call	*8(%rsp)		// call __dyld_fork_child_final indirectly
#endif
	xorq	%rax,%rax	// zero rax
	addq	$24, %rsp   // restore the stack
	ret

	//parent here...
L2:
	movl	%eax, 16(%rsp)		// save pid
#if	defined(__DYNAMIC__)
// __dyld_fork_parent() is called by the parent process after a fork syscall.
// This releases the dyld lock acquired by __dyld_fork_prepare().
	leaq	8(%rsp),%rsi		// get the address where we're going to store the pointer
	leaq 	LC2(%rip), %rdi		// copy the name of the function to look up
	call 	__dyld_func_lookup
	call	*8(%rsp)		// call __dyld_fork_parent indirectly
#endif
	CALL_EXTERN_AGAIN(__cthread_fork_parent)
	movl	16(%rsp), %eax		// return pid
	addq	$24, %rsp   // restore the stack
	ret
