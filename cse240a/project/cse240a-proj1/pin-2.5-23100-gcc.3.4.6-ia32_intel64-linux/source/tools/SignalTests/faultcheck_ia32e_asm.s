/*
 * void DoUD2()
 */
.text
    .align 4
.globl DoUD2
DoUD2:
    ud2
    ret


/*
 * void DoIdiv0()
 */
.text
    .align 4
.globl DoIdiv0
DoIdiv0:
    mov     $0x0, %rax
    idiv    %rax
    ret


/*
 * void DoSqrtNeg1()
 */
.text
    .align 4
.globl DoSqrtNeg1
DoSqrtNeg1:
    subq    $8, %rsp

    fnstcw  0(%rsp)         /* get current FP control word */
    mov     0(%rsp), %ax    /* save, so it can be restored later */
    btrw    $0, 0(%rsp)     /* enable "invalid operation" exceptions */
    fldcw   0(%rsp)         /* store new FP control word */

    fld1                    /* st(0) = 1 */
    fchs                    /* st(0) = -1 */
    fsqrt                   /* raise exception, but remains pending */
    fwait                   /* exception delivered here */

    mov     %ax, 0(%rsp)    /* restore original FP control word */
    fldcw   0(%rsp)

    addq    $8, %rsp
    ret


/*
 * void DoSSEDiv0()
 */
.text
    .align 4
.globl DoSSEDiv0
DoSSEDiv0:
    subq        $8, %rsp

    stmxcsr     0(%rsp)
    btrw        $9, 0(%rsp)     /* enable "zero divide" exceptions */
    ldmxcsr     0(%rsp)

    movl        $0, %eax
    cvtsi2ss    %eax, %xmm0     /* %xmm0 = 0.0 */
    movl        $1, %eax
    cvtsi2ss    %eax, %xmm1     /* %xmm1 = 1.0 */
    divss       %xmm0, %xmm1    /* raise "zero divide" exception, delivered at this PC */

    addq        $8, %rsp
    ret


/*
 * void DoSSEPrec()
 */
.text
    .align 4
.globl DoSSEPrec
DoSSEPrec:
    subq        $8, %rsp

    stmxcsr     0(%rsp)
    btrw        $12, 0(%rsp)     /* enable "precision" exceptions */
    ldmxcsr     0(%rsp)

    movl        $1, %eax
    cvtsi2ss    %eax, %xmm0     /* %xmm0 = 1.0 */
    movl        $2, %eax
    cvtsi2ss    %eax, %xmm1     /* %xmm1 = 2.0 */
    divss       %xmm1, %xmm0    /* %xmm0 = 0.5 */
    cvtss2si    %xmm0, %eax     /* raise "precision" exception, delivered at this PC */

    addq        $8, %rsp
    ret


/*
 * void DoInt3()
 */
.text
    .align 4
.globl DoInt3
DoInt3:
    .byte 0xcc
    ret
