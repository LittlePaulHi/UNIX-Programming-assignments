; dispbin:
;         given a number in AX, store the corresponding bit string in str1.
;         for example, if AX = 0x1234, the result should be:
;         str1 = 0001001000111000
; ======
;       str1 @ 0x600000-600014
; ======

mov cx, 16
HexToBin:
    rcr eax, 1
    ; Method 1
    setc bl  ; bl = 0 or 1 by CF (is set) (Intel 80386 instruction)
    add bl, '0'  ; turn that 0/1 into '0'/'1' ASCII char
    ; Method 2
    ; jc Carry
    ; mov bl, '0'
    mov [0x600000 + ecx - 1], bl
    loop HexToBin
    jmp END

Carry:
    mov bl, '1'
    mov [0x600000 + ecx - 1], bl
    loop HexToBin

END: