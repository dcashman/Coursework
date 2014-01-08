.intel_syntax noprefix
	
.globl clobberRegs
.type clobberRegs, function
clobberRegs:
	mov rax, 0xdeadbeef
	mov rcx, 0xdeadbeef
	mov rdx, 0xdeadbeef
	mov rsi, 0xdeadbeef
	mov rdi, 0xdeadbeef
	mov r8, 0xdeadbeef
	mov r9, 0xdeadbeef
	mov r10, 0xdeadbeef
	mov r11, 0xdeadbeef
	# to prevent the clobbers from being optimized
	add rax, rcx
	add rax, rdx
	add rax, rsi
	add rax, rdi
	add rax, r8
	add rax, r9
	add rax, r10
	add rax, r11
	ret
	
