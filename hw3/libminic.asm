%macro gensys 2
    global sys_%2:function
sys_%2:
    push r10
    mov r10, rcx
    mov rax, %1
    syscall
    pop r10
    ret
%endmacro

; RDI, RSI, RDX, RCX, R8, R9
section .data
section .text
    gensys 1, write
    gensys 13, rt_sigaction
    gensys 14, rt_sigprocmask
    gensys 15, rt_sigreturn
    gensys 37, alarm
    gensys 34, pause
    gensys 35, nanosleep
    gensys 60, exit

    global write:function
write:
    jmp sys_write

extern __jmp_mask
    global setjmp:function
setjmp:
    push rbx
    mov rbx, rdi
    ;
    xor rdi, rdi
    xor rsi, rsi
    mov rdx, QWORD __jmp_mask
    call sigprocmask ; sigprocmask(SIG_BLOCK, NULL, __jmp_mask)
    mov rdi, rbx
    pop rbx
    ; original
    pop rsi
    xor eax, eax
    mov QWORD [rdi], rbx
    mov QWORD [rdi+8], rsp
    push rsi
    mov QWORD [rdi+16], rbp
    mov QWORD [rdi+24], r12
    mov QWORD [rdi+32], r13
    mov QWORD [rdi+40], r14
    mov QWORD [rdi+48], r15
    mov QWORD [rdi+56], rsi
    ret

    global longjmp:function
longjmp:
    push rbp
    mov rbp, rsi
    push rbx
    mov rbx, rdi
    sub rsp, 8
    ;
    mov rdi, 2
    mov rsi, QWORD __jmp_mask
    xor rdx, rdx
    call sigprocmask ; sigprocmask(SIG_SETMASK, __jmp_mask, NULL)
    add rsp, 8
    mov rdi, rbx
    mov rsi, rbp
    pop rbx
    pop rbp
    ; original
    mov rax, rsi
    mov rbx, QWORD [rdi]
    mov rsp, QWORD [rdi+8]
    mov rbp, QWORD [rdi+16]
    mov r12, QWORD [rdi+24]
    mov r13, QWORD [rdi+32]
    mov r14, QWORD [rdi+40]
    mov r15, QWORD [rdi+48]
    jmp [rdi+56]
    ret

;    global sigaction:function
;sigaction:
;    mov rax, QWORD [sys_rt_sigreturn wrt ..gotpcrel]
;    or QWORD [rsi+24], 0x04000000
;    mov QWORD [rsi+32], rax ;sys_rt_sigreturn
;    mov rcx, 4
;    jmp sys_rt_sigaction

    global sigprocmask:function
sigprocmask:
    mov rcx, 4
    jmp sys_rt_sigprocmask

    global alarm:function
alarm:
    jmp sys_alarm

;    global pause:function
;pause:
;    jmp sys_pause

    global sleep:function
sleep:
    sub rsp, 16
    mov QWORD [rsp+8], 0
    mov [rsp], rdi
    xor rax, rax
    mov rdi, rsp
    xor rsi, rsi ; null
    call sys_nanosleep
    add rsp, 16
    ret

    global exit:function
exit:
    jmp sys_exit

