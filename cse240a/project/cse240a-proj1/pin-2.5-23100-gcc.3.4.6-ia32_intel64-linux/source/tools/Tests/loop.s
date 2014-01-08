	.intel_syntax noprefix
	.text
.globl main
	.type	main, @function
main:
	mov ecx, 16
loop1:
	nop
	loop  loop1

	mov ecx, 16
	mov eax, 0
	cmp eax,0
loop2:
	nop
	loope  loop2

	mov ecx, 16
	mov eax, 1
	cmp eax, 0	
loop3:	
	nop
	loopne  loop3

	mov eax, 0
	ret
