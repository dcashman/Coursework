# this routine should generate an error because the target of a local branch
# is within the probe space.
#
.global do_nothing
.type do_nothing, function

do_nothing:
    xor %eax, %eax
lab1:
    xor %eax, %eax
    test %eax, %eax
    je lab2

    xor %eax, %eax
    xor %eax, %eax
    jmp lab1

lab2:
    xor %eax, %eax
    xor %eax, %eax

    ret


# This code pattern will result in an error because the call 
# is less than the size of the probe, and when relocated, the 
# push of the original IP will cause a branch into the probe.
#
.global call_function
.type call_function, function

call_function:
    call do_nothing
    push %ebx
    pop %ebx
    ret


# This function should generate an error because the unconditional jump
# is in the middle of the probe area.
#	
.global bad_jump
.type bad_jump, function

bad_jump:
    xchg %eax, %eax
    jmp lab3

    push %ebx
    pop %ebx

lab3:
    xor %eax, %eax
    xor %eax, %eax

    ret

