  .global main

  .text
fib:
    enter $0, $0
    cmp $0, %rdi 
    jle fib_le_0 
    cmp $1, %rdi
    je fib_e_1 
    push %rbx 
    push %rcx 
    mov %rdi, %rcx 
    dec %rcx 
    mov %rcx, %rdi  
    call fib 
    mov %rax, %rbx 
    dec %rcx 
    mov %rcx, %rdi 
    call fib 
    add %rbx, %rax 
    pop %rcx 
    pop %rbx
    jmp fibend 
fib_le_0:
    mov $0, %rax 
    jmp fibend
fib_e_1:
    mov $1, %rax 
    jmp fibend
fibend:
    leave 
    ret 
main:
    enter $0, $0
/*
  mov 8(%rsi), %rdi
  call atol

  mov %rax, %rsi
  mov $output_fmt, %rdi
  mov $0, %al
  call printf
*/ 
    cmp $2, %rdi 
    jne output_error
    mov 8(%rsi), %rdi 
    call atol
    cmp $0, %rax 
    jl output_error 
    mov %rax, %r12
    cmp $0, %r12 
    jl output_error
    mov %r12, %rdi 
    call fib
    mov $output_fmt, %rdi 
    mov %r12, %rsi 
    mov %rax, %rdx
    mov $0, %rax 
    call printf
    mov $1, %rax  
    jmp end
output_error:
    mov $output_err, %rdi 
    mov $0, %rax
    call printf
    mov $1, %rax 
end:
    leave
    ret

  .data
output_fmt:  .string "fib(%ld) = %ld\n"
output_err:  .string "Usage: ./fib N, where N >= 0\n"
