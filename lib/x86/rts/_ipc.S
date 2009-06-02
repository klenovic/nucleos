/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// See src/kernel/ipc.h for C definitions
.set SEND,    1
.set RECEIVE, 2
.set SENDREC, 3
.set NOTIFY,  4
.set SENDNB,  5 
.set SYSVEC, 33    // trap to kernel

.set SRC_DST,   8  // source/ destination process
.set MESSAGE,  12  // message pointer

//========================================================================
//                           IPC assembly routines
//========================================================================
// all message passing routines save ebp, but destroy eax and ecx.
.section .text
.globl _send
_send:
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        mov     SRC_DST(%ebp), %eax      // eax = dest-src
        mov     MESSAGE(%ebp), %ebx      // ebx = message pointer
        mov     $SEND,%ecx               // _send(dest, ptr)
        int     $SYSVEC                  // trap to the kernel
        pop     %ebx
        pop     %ebp
        ret

.globl _receive
_receive:
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        mov     SRC_DST(%ebp), %eax     // eax = dest-src
        mov     MESSAGE(%ebp), %ebx     // ebx = message pointer
        mov     $RECEIVE, %ecx          // _receive(src, ptr)
        int     $SYSVEC                 // trap to the kernel
        pop     %ebx
        pop     %ebp
        ret

.globl _sendrec 
_sendrec:
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        mov     SRC_DST(%ebp), %eax      // eax = dest-src
        mov     MESSAGE(%ebp), %ebx      // ebx = message pointer
        mov     $SENDREC, %ecx           // _sendrec(srcdest, ptr)
        int     $SYSVEC                  // trap to the kernel
        pop     %ebx
        pop     %ebp
        ret

.globl _notify
_notify:
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        mov     SRC_DST(%ebp), %eax      // ebx = destination 
        mov     $NOTIFY, %ecx            // _notify(srcdst)
        int     $SYSVEC                  // trap to the kernel
        pop     %ebx
        pop     %ebp
        ret

.globl _sendnb
_sendnb:
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        mov     SRC_DST(%ebp),%eax        // eax = dest-src
        mov     MESSAGE(%ebp),%ebx        // ebx = message pointer
        mov     $SENDNB, %ecx               // _sendnb(dest, ptr)
        int     $SYSVEC                  // trap to the kernel
        pop     %ebx
        pop     %ebp
        ret
