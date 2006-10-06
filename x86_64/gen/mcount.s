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
#import <architecture/i386/asm_help.h>

.text
        .globl mcount
mcount:
        pushq   %rbp            // setup mcount's frame
        movq    %rsp,%rbp

        // The compiler doesn't preserve registers when calling mcount
        // so we have to ensure that we don't trash any.
        subq	$8, %rsp		// maintain 16-byte alignment
        pushq	%rdi
        pushq	%rsi
        pushq	%rax
        
        movq    (%rbp),%rax     // load the frame pointer of mcount's caller
        movq    8(%rax),%rax    // load mcount's caller's return address
        movq    8(%rbp),%rdi    // set up the selfpc parameter for moncount()
        movq    %rax,%rsi       // set up the frompc parameter for moncount()
        CALL_EXTERN(_moncount)  // call moncount()
        
        popq	%rax
        popq	%rsi
        popq	%rdi
        // No need for an addq because we're restoring %rsp in the
        // next instruction.

        movq    %rbp,%rsp       // tear down mcount's frame
        popq    %rbp
        ret
