; loop15:
;         str1 is a string contains 15 lowercase and uppercase alphbets.
;         implement a loop to convert all alplabets to lowercase,
;         and store the result in str2.
; ======
;       str1 @ 0x600000-600010
;       str2 @ 0x600010-600020
; ======

mov ecx, 15
Loop15:
    mov al, [0x600000 + ecx - 1]
    cmp al, 0x5A  ; 0x5A = Z
    jle ToLower
    mov [0x600010 + ecx - 1], al
    loop Loop15
    jmp END
ToLower:
    xor al, 0x20
    mov [0x600010 + ecx - 1], al
    loop Loop15
END: