; recur: implement a recursive function

;    r(n) = 0, if n <= 0
;         = 1, if n == 1
;         = 2*r(n-1) + 3*r(n-2), otherwise

;    please call r(21) and store the result in ebx
; ======
; ======

mov edi, 21
call RECUR
jmp END

RECUR:
    push rbp
    mov rbp, rsp  ; set new base address of stack frame (frame pointer)
    
    sub rsp, 0xc  ; for local

    mov DWORD PTR [rbp - 0x4], edi
    cmp DWORD PTR [rbp - 0x4], 0x1
    jle LEONE  ; if n <= 1

    ; 2*r(n-1)
    mov edi, DWORD PTR [rbp - 0x4]
    sub edi, 0x1
    call RECUR

    mov DWORD PTR [rbp - 0x8], eax
    add DWORD PTR [rbp - 0x8], eax

    ; 3*r(n-2)
    mov edi, DWORD PTR [rbp - 0x4]
    sub edi, 0x2
    call RECUR

    add DWORD PTR [rbp - 0x8], eax
    add DWORD PTR [rbp - 0x8], eax
    add DWORD PTR [rbp - 0x8], eax

    mov eax, DWORD PTR [rbp - 0x8]
    jmp RETURN

LEONE:
    cmp edi, 0x1
    jl LONE
    mov eax, 0x1
    jmp RETURN

LONE:
    mov eax, 0x0

RETURN:
    leave  ; mov rsp, rbp
           ; pop rbp
    ret  ; pop eip

END: