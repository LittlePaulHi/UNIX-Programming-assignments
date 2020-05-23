; posneg: test if registers are positive or negative.
;         if ( eax >= 0 ) { var1 = 1 } else { var1 = -1 }
;         if ( ebx >= 0 ) { var2 = 1 } else { var2 = -1 }
;         if ( ecx >= 0 ) { var3 = 1 } else { var3 = -1 }
;         if ( edx >= 0 ) { var4 = 1 } else { var4 = -1 }
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
;       var4 @ 0x60000c-600010
; ======

IF1:  ; if ( eax >= 0 )
    cmp eax, 0
    jge L1
    mov eax, -1  ; eax < 0
    jmp IF2
L1:  ; eax >= 0
    mov eax, 1
IF2:  ; if ( ebx >= 0 )
    mov [0x600000], eax
    cmp ebx, 0
    jge L2
    mov ebx, -1  ; ebx < 0
    jmp IF3
L2:  ; ebx >= 0
    mov ebx, 1
IF3:  ; if ( ecx >= 0 )
    mov [0x600004], ebx
    cmp ecx, 0
    jge L3
    mov ecx, -1  ; ecx < 0
    jmp IF4
L3:  ; ecx >= 0
    mov ecx, 1
IF4:  ; if ( edx >= 0 )
    mov [0x600008], ecx
    cmp edx, 0
    jge L4
    mov edx, -1  ; edx < 0
    jmp END
L4:  ; edx >= 0
    mov edx, 1
END:
    mov [0x60000c], edx