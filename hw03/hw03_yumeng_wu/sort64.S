.global main
.data
    usage_msg: .string "Usage: ./sort input-file output-file\n"
    error_msg: .string "Error"
    fd: .int 0
    file_size: .long 0
    test_int: .string "%d\n"
.text
/* check if sys call has error */
has_error:
    enter $0, $0
    cmp $0, %rdi
    jge has_error_ret
    mov $error_msg, %rdi
    call perror
    mov $1, %rdi
    mov $60, %rax
    syscall
has_error_ret:
    leave
    ret

/* main function */
main:
    enter $0, $0
    cmp $3, %rdi
    jne output_usage
    /* save argv */
    mov %rsi, %r12
    /* open argv[1] */
    mov 8(%r12), %rdi
    mov $0, %rsi
    mov $2, %rax
    syscall
    mov %rax, fd
    mov %rax, %rdi
    call has_error
    /* use sys_lseek to get file size */
    mov $fd, %rax
    mov 0(%rax), %rdi
    mov $0, %rsi
    mov $2, %rdx
    mov $8, %rax
    syscall
    mov %rax, file_size
    mov %rax, %rdi
    call has_error
    /* use sys_lseek to reset offset */
    mov $fd, %rax
    mov 0(%rax), %rdi
    mov $0, %rsi
    mov $0, %rdx
    mov $8, %rax
    syscall
    call has_error
    /* allocate memory */
    mov $file_size, %rdx
    mov %rdx, %rax
    and $7, %rax
    cmp $0, %rax
    je alloc_after_align
    add $4, %rdx
alloc_after_align:
    sub %rdx, %rsp
    lea 0(%rsp), %r13
    /* use sys_read to read file */
    mov $fd, %rax
    mov 0(%rax), %rsi
    mov $test_int, %rdi
    call printf
    /* close argv[1] */
    mov $fd, %rax
    mov 0(%rax), %rdi
    mov $3, %rax
    syscall
    mov %rax, %rdi
    call has_error
    /* free memory */
    mov $file_size, %rdx
    mov %rdx, %rax
    and $7, %rax
    cmp $0, %rax
    je free_after_align
    add $4, %rdx
free_after_align:
    add %rdx, %rsp
    mov $0, %rax
    jmp main_end
output_usage:
    mov $usage_msg, %rdi
    mov $0, %rax
    call printf
    mov $1, %rax
main_end:
    leave
    ret
