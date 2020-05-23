; math5: 32-bit signed arithmetic
;         var3 = (var1 * -var2) / (var3 - ebx)
;         note: overflowed part should be truncated
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
; ======

; (var1 * -var2)
mov eax, [0x600000]
mov ecx, [0x600004]
neg ecx
imul ecx
; (var3 - ebx)
mov ecx, [0x600008]
sub ecx, ebx
; (var1 * -var2) / (var3 - ebx)
cdq  ;  CDQ: extend EAX ==> EDX:EAX
idiv ecx
mov [0x600008], eax