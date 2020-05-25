; bubble: bubble sort for 10 integers
; ======
;       a[0] @ 0x600000-600004
;       a[1] @ 0x600004-600008
;       a[2] @ 0x600008-60000c
;       a[3] @ 0x60000c-600010
;       a[4] @ 0x600010-600014
;       a[5] @ 0x600014-600018
;       a[6] @ 0x600018-60001c
;       a[7] @ 0x60001c-600020
;       a[8] @ 0x600020-600024
;       a[9] @ 0x600024-600028
; ======

mov ecx, 10
BUBBLE:
    mov edx, 9
    lea esi, [0x600000]

    BLOOP:
        mov eax, [esi]
        cmp eax, [esi + 4]
        jg SWAP
        jmp NEXT

        SWAP:
            xchg eax, [esi + 4]
            mov [esi], eax

        NEXT:
            lea esi, [esi + 4]
            dec edx
            jnz BLOOP
            loop BUBBLE