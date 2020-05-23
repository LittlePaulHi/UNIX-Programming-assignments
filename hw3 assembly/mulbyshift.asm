; mulbyshift: multiply val1 by 26 and store the result in val2
; ======
;       val1 @ 0x600000-600004
;       val2 @ 0x600004-600008
; ======

mov eax, [0x600000]
shl eax, 4  ; EAX*16
mov [0x600004], eax
mov eax, [0x600000]
shl eax, 3  ; EAX*8
add [0x600004], eax
mov eax, [0x600000]
shl eax, 1  ; EAX*2
add [0x600004], eax