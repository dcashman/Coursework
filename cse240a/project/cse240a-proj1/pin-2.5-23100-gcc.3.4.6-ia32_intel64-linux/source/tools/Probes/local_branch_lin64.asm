# this routine should generate an error because the target of a local branch
# is within the probe space.
#
.text
.global do_nothing
.type do_nothing, function

do_nothing:
    mov %rax, 0x0
lab1:
    xor %rax, %rax
    test %rax, %rax
    je lab2

    xor %rax, %rax
    xor %rax, %rax
    jmp lab1

lab2:
    xor %rax, %rax
    xor %rax, %rax

    ret


# This code pattern will result in an error because the call 
# is less than the size of the probe, and when relocated, the 
# push of the original IP will cause a branch into the probe.
#
.global call_function
.type call_function, function

call_function:
    call do_nothing
    push %rbx
    pop %rbx
    ret



# This function should generate an error because the unconditional jump
# is in the middle of the probe area.
#	
.global bad_jump
.type bad_jump, function

bad_jump:
    xchg %rax, %rax
    jmp lab3

    push %rbx
    pop %rbx

lab3:
    xor %rax, %rax
    xor %rax, %rax

    ret

