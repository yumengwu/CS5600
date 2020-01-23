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
    /* use sys_open to open argv[1] */
    mov 8(%r12), %rdi
    mov $0, %rsi
    mov $2, %rax
    syscall
    mov %rax, fd
    mov $fd, %rax
    mov 0(%rax), %rsi
    mov $test_int, %rdi
    mov $0, %rax
    call printf
    /* use sys_close to close argv[1] */
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
