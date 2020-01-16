.global main
.data
    output_fmt:  .string "fib(%ld) = %ld\n"
    usage_string: .string "Usage: ./fib N, where N >= 0\n"
.text

/* fibonacci function */
fib:
    enter $0, $0
    mov 8(%ebp), %eax
    /* if (num == 0) */
    cmp $0, %eax 
    je fibret 
    /* if (num == 1) */
    cmp $1, %eax 
    je fibret 
    push %ebx
    push %ecx  
    mov %eax, %ebx 
    dec %ebx 
    push %ebx 
    /* calculate fib(num - 1) */
    call fib 
    mov %eax, %ecx 
    add $4, %esp 
    dec %ebx 
    push %ebx 
    /* calculate fib(num - 2) */
    call fib 
    add $4, %esp 
    add %ecx, %eax 
    pop %ecx 
    pop %ebx 
fibret:
    leave
    ret
main:
    enter $0, $0
    mov 8(%ebp), %edx
    cmp $2, %edx
    jne print_usage
    mov 12(%ebp), %ecx
    mov 4(%ecx), %eax
    push %eax 
    call atoi
    add $4, %esp
    mov %eax, %edx 
    cmp $0, %eax 
    jl print_usage
    push %edx 
    push %eax 
    call fib 
    add $8, %esp
    push %eax
    push %edx  
    push $output_fmt 
    call printf 
    add $8, %esp 
    mov $0, %eax 
    jmp end
print_usage:
    push $usage_string
    call printf
    mov $1, %eax 
end:
    leave
    ret
