; math4: 32-bit signed arithmetic
;         var4 = (var1 * -5) / (-var2 % var3)
;         note: overflowed part should be truncated
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
;       var4 @ 0x60000c-600010
; ======

; (var1 * -5)
mov eax, [0x600000]
mov ebx, 0x05
neg ebx
imul ebx
mov [0x60000c], eax  ; var4 = (var1 * -5)
; (-var2 % var3)
mov eax, [0x600004]
neg eax
cdq  ;  CDQ: extend EAX ==> EDX:EAX
mov ebx, [0x600008]
idiv ebx
mov ebx, edx  ; EDX is remainder
; (var1 * -5) / (-var2 % var3)
mov eax, [0x60000c]
cdq  ;  CDQ: extend EAX ==> EDX:EAX
idiv ebx
mov [0x60000c], eax