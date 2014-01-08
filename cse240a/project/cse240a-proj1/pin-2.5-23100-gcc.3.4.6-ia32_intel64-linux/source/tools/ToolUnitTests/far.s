

farret:
mov $5,  %eax
lret

.type FarCallTest, @function
.global FarCallTest
FarCallTest:
push %cs
call farret
ret


