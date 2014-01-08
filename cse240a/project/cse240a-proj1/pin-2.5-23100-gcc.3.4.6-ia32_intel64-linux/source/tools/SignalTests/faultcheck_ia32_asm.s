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
    mov     $0x0, %eax
    idiv    %eax
    ret


/*
 * void DoSqrtNeg1()
 */
.text
    .align 4
.globl DoSqrtNeg1
DoSqrtNeg1:
    subl    $4, %esp

    fnstcw  0(%esp)         /* get current FP control word */
    mov     0(%esp), %ax    /* save, so it can be restored later */
    btrw    $0, 0(%esp)     /* enable "invalid operation" exceptions */
    fldcw   0(%esp)         /* store new FP control word */

    fld1                    /* st(0) = 1 */
    fchs                    /* st(0) = -1 */
    fsqrt                   /* raise exception, but remains pending */
    fwait                   /* exception delivered here */

    mov     %ax, 0(%esp)    /* restore original FP control word */
    fldcw   0(%esp)

    addl    $4, %esp
    ret


/*
 * void DoSSEDiv0()
 */
.text
    .align 4
.globl DoSSEDiv0
DoSSEDiv0:
    subl        $4, %esp

    stmxcsr     0(%esp)
    btrw        $9, 0(%esp)     /* enable "zero divide" exceptions */
    ldmxcsr     0(%esp)

    movl        $0, %eax
    cvtsi2ss    %eax, %xmm0     /* %xmm0 = 0.0 */
    movl        $1, %eax
    cvtsi2ss    %eax, %xmm1     /* %xmm1 = 1.0 */
    divss       %xmm0, %xmm1    /* raise "zero divide" exception, delivered at this PC */

    addl        $4, %esp
    ret


/*
 * void DoSSEPrec()
 */
.text
    .align 4
.globl DoSSEPrec
DoSSEPrec:
    subl        $4, %esp

    stmxcsr     0(%esp)
    btrw        $12, 0(%esp)     /* enable "precision" exceptions */
    ldmxcsr     0(%esp)

    movl        $1, %eax
    cvtsi2ss    %eax, %xmm0     /* %xmm0 = 1.0 */
    movl        $2, %eax
    cvtsi2ss    %eax, %xmm1     /* %xmm1 = 2.0 */
    divss       %xmm1, %xmm0    /* %xmm0 = 0.5 */
    cvtss2si    %xmm0, %eax     /* raise "precision" exception, delivered at this PC */

    addl        $4, %esp
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


/*
 * void DoIntO()
 */
.text
    .align 4
.globl DoIntO
DoIntO:
    mov     $0x7f, %al
    add     $1, %al
    into
    ret


/*
 * void DoBound()
 */
.text
    .align 4
.globl DoBound
DoBound:
    subl    $8, %esp

    movl    $0, 0(%esp)     /* set the "bounds" to (0,0) */
    movl    $0, 4(%esp)
    mov     $0x80, %eax     /* set the "index" to 0x80 */
    boundl  %eax, 0(%esp)   /* this should raise an exception */

    addl    $8, %esp
    ret
