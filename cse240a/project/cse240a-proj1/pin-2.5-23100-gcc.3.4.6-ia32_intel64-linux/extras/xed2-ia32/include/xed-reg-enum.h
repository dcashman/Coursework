/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2008 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/// @file xed-reg-enum.h
/// @author Mark Charney <mark.charney@intel.com>

// This file was automatically generated.
// Do not edit this file.

#if !defined(_XED_REG_ENUM_H_)
# define _XED_REG_ENUM_H_
#include "xed-common-hdrs.h"
typedef enum {
  XED_REG_INVALID,
  XED_REG_ERROR,
  XED_REG_RAX,
  XED_REG_EAX,
  XED_REG_AX,
  XED_REG_AH,
  XED_REG_AL,
  XED_REG_RCX,
  XED_REG_ECX,
  XED_REG_CX,
  XED_REG_CH,
  XED_REG_CL,
  XED_REG_RDX,
  XED_REG_EDX,
  XED_REG_DX,
  XED_REG_DH,
  XED_REG_DL,
  XED_REG_RBX,
  XED_REG_EBX,
  XED_REG_BX,
  XED_REG_BH,
  XED_REG_BL,
  XED_REG_RSP,
  XED_REG_ESP,
  XED_REG_SP,
  XED_REG_SPL,
  XED_REG_RBP,
  XED_REG_EBP,
  XED_REG_BP,
  XED_REG_BPL,
  XED_REG_RSI,
  XED_REG_ESI,
  XED_REG_SI,
  XED_REG_SIL,
  XED_REG_RDI,
  XED_REG_EDI,
  XED_REG_DI,
  XED_REG_DIL,
  XED_REG_R8,
  XED_REG_R8D,
  XED_REG_R8W,
  XED_REG_R8B,
  XED_REG_R9,
  XED_REG_R9D,
  XED_REG_R9W,
  XED_REG_R9B,
  XED_REG_R10,
  XED_REG_R10D,
  XED_REG_R10W,
  XED_REG_R10B,
  XED_REG_R11,
  XED_REG_R11D,
  XED_REG_R11W,
  XED_REG_R11B,
  XED_REG_R12,
  XED_REG_R12D,
  XED_REG_R12W,
  XED_REG_R12B,
  XED_REG_R13,
  XED_REG_R13D,
  XED_REG_R13W,
  XED_REG_R13B,
  XED_REG_R14,
  XED_REG_R14D,
  XED_REG_R14W,
  XED_REG_R14B,
  XED_REG_R15,
  XED_REG_R15D,
  XED_REG_R15W,
  XED_REG_R15B,
  XED_REG_RIP,
  XED_REG_EIP,
  XED_REG_IP,
  XED_REG_FLAGS,
  XED_REG_EFLAGS,
  XED_REG_RFLAGS,
  XED_REG_CS,
  XED_REG_DS,
  XED_REG_ES,
  XED_REG_SS,
  XED_REG_FS,
  XED_REG_GS,
  XED_REG_XMM0,
  XED_REG_XMM1,
  XED_REG_XMM2,
  XED_REG_XMM3,
  XED_REG_XMM4,
  XED_REG_XMM5,
  XED_REG_XMM6,
  XED_REG_XMM7,
  XED_REG_XMM8,
  XED_REG_XMM9,
  XED_REG_XMM10,
  XED_REG_XMM11,
  XED_REG_XMM12,
  XED_REG_XMM13,
  XED_REG_XMM14,
  XED_REG_XMM15,
  XED_REG_MMX0,
  XED_REG_MMX1,
  XED_REG_MMX2,
  XED_REG_MMX3,
  XED_REG_MMX4,
  XED_REG_MMX5,
  XED_REG_MMX6,
  XED_REG_MMX7,
  XED_REG_ST0,
  XED_REG_ST1,
  XED_REG_ST2,
  XED_REG_ST3,
  XED_REG_ST4,
  XED_REG_ST5,
  XED_REG_ST6,
  XED_REG_ST7,
  XED_REG_CR0,
  XED_REG_CR1,
  XED_REG_CR2,
  XED_REG_CR3,
  XED_REG_CR4,
  XED_REG_CR5,
  XED_REG_CR6,
  XED_REG_CR7,
  XED_REG_CR8,
  XED_REG_CR9,
  XED_REG_CR10,
  XED_REG_CR11,
  XED_REG_CR12,
  XED_REG_CR13,
  XED_REG_CR14,
  XED_REG_CR15,
  XED_REG_DR0,
  XED_REG_DR1,
  XED_REG_DR2,
  XED_REG_DR3,
  XED_REG_DR4,
  XED_REG_DR5,
  XED_REG_DR6,
  XED_REG_DR7,
  XED_REG_DR8,
  XED_REG_DR9,
  XED_REG_DR10,
  XED_REG_DR11,
  XED_REG_DR12,
  XED_REG_DR13,
  XED_REG_DR14,
  XED_REG_DR15,
  XED_REG_ONE,
  XED_REG_STACKPUSH,
  XED_REG_STACKPOP,
  XED_REG_GDTR,
  XED_REG_LDTR,
  XED_REG_IDTR,
  XED_REG_TR,
  XED_REG_TSC,
  XED_REG_TSCAUX,
  XED_REG_MSRS,
  XED_REG_X87CONTROL,
  XED_REG_X87STATUS,
  XED_REG_X87TOP,
  XED_REG_X87TAG,
  XED_REG_X87PUSH,
  XED_REG_X87POP,
  XED_REG_X87POP2,
  XED_REG_MXCSR,
  XED_REG_TMP0,
  XED_REG_TMP1,
  XED_REG_TMP2,
  XED_REG_TMP3,
  XED_REG_TMP4,
  XED_REG_TMP5,
  XED_REG_TMP6,
  XED_REG_TMP7,
  XED_REG_TMP8,
  XED_REG_TMP9,
  XED_REG_TMP10,
  XED_REG_TMP11,
  XED_REG_TMP12,
  XED_REG_TMP13,
  XED_REG_TMP14,
  XED_REG_TMP15,
  XED_REG_LAST
} xed_reg_enum_t;

XED_DLL_EXPORT  xed_reg_enum_t
str2xed_reg_enum_t(const char* s);
XED_DLL_EXPORT  const char*
xed_reg_enum_t2str(const xed_reg_enum_t p);

#endif
