        .data
array:  .word   3
        .word   123
        .word   4346
array2: .word   0x12345678
        .word   0xFFFFFFFF
        .text
main:
        addiu   $2, $0, 1024
        addu    $3, $2, $2
        or      $4, $3, $2
        addiu   $5, $0, 1234
        sll     $6, $5, 16
        addiu   $7, $6, 9999
        subu    $8, $7, $2
        nor     $9, $4, $3
        ori     $10, $2, 255
        srl     $11, $6, 5
        srl     $12, $6, 4
        la      $4, array2
        lb      $2, 1($4)
        sb      $2, 6($4)
        and     $13, $11, $5
        andi    $14, $4, 100
        subu    $15, $0, $10
        lui     $17, 100
        addiu   $2, $0, 0xa