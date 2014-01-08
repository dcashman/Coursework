/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: S. Bharadwaj Yadavalli
*/

/* ===================================================================== */
/*! @file
 *  This file contains the assembly source of Pin unit test repcmpsz_tool
 */
.data
one:
	.string	"IAMHEREE"
	strlen = . - one
two:	
	.string	"IWASHERE"
.LC2:
	.string	"DIFFERENT\n"
	lc2 = . - .LC2
.LC3:
	.string	"SAME\n"
	lc3 = . - .LC3
.text
.globl _start

_start:

# Test different string comparison
	lea	one, %esi
	lea	two, %edi
	mov     $strlen,%ecx
	cld
	rep cmpsl 

# Test same string comparison
	lea	one, %esi
	lea	one, %edi
	mov     $strlen,%ecx
	cld
	rep cmpsl 

# Test scasd
	movl	one, %eax
	lea	two, %edi
	cld
	scasw
	mov     %eax,%ecx
	
# and exit

	movl	$0,%ebx		# first argument: exit code
	movl	$1,%eax		# system call number (sys_exit)
	int	$0x80		# call kernel

.data
msg:
	.ascii	"Hello, world!\n"	# our dear string
	len = . - msg			# length of our dear string
