        .data
var:  .word   5
        .text
main:
    la $8, var
    lw $9, 0($8)
    addu $2, $0, $9
    jal sum
    j exit

sum: sltiu $1, $2, 1
    bne $1, $0, sum_exit
    addu $3, $3, $2
    addiu $2, $2, -1
    j sum
sum_exit:
    addu $4, $3, $0
    jr $31
exit: