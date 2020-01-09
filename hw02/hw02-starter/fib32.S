    .global main

    .text
fib:
    enter $0, $0
    mov 8(%ebp), %eax
    cmp $0, %eax 
    je fibret 
    cmp $1, %eax 
    je fibret 
    push %ebx
    push %ecx  
    mov %eax, %ebx 
    dec %ebx 
    push %ebx 
    call fib 
    mov %eax, %ecx 
    add $4, %esp 
    dec %ebx 
    push %ebx 
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
/*
    mov 12(%ebp), %ecx
    mov 4(%ecx), %eax
    push %eax
    call atoi
    add $4, %esp

    push %eax
    push $output_fmt
    call printf
    add $8, %esp
*/
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
    jmp end
print_usage:
    push $usage_string
    call printf
end:
    mov $0, %eax
    leave
    ret

    .data
output_fmt:  .string "fib(%ld) = %ld\n"
usage_string: .string "Usage: ./fib N, where N >= 0\n"
