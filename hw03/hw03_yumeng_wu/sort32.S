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
    mov 8(%ebp), %eax
    cmp $0, %eax
    jge has_error_ret
    /* if return value of sys call < 0, this function will
       will print error message and call sys_exit */
    push $error_msg
    call perror
    add $4, %esp
    mov $1, %ebx
    mov $0x1, %eax
    int $0x80
has_error_ret:
    mov $0, %eax
    leave
    ret

/*
 * merge sort:
 * void sort(int * array, long length);
 */
sort:
    enter $0, $0
    push 0(%ebp)
    push $test_int
    call printf
    add $8, %esp
    leave
    ret

main:
    enter $0, $0
    /* compare argc */
    mov 8(%ebp), %eax
    cmp $3, %eax
    jne output_usage
    /* use system call sys_open to open file argv[1] */
    mov $0, %ecx
    mov $0, %edx
    mov 12(%ebp), %eax
    mov 4(%eax), %ebx
    mov $0x5, %eax
    int $0x80
    mov %eax, fd
    mov $fd, %ebx
    push 0(%ebx)
    call has_error
    add $4, %esp
    /* use sys_lseek to calculate file size */
    mov $0, %ecx
    mov $2, %edx
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov $0x13, %eax
    int $0x80
    mov %eax, file_size
    push %eax
    call has_error
    add $4, %esp
    /* use sys_lseek to reset offset */
    mov $0, %ecx
    mov $0, %edx
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov $0x13, %eax
    int $0x80
    push %eax
    call has_error
    add $4, %esp
    /* allocate space on the stack */
    mov $file_size, %eax
    mov 0(%eax), %edx
    sub %edx, %esp
    lea 0(%esp), %edi
    /* use sys_read to read file */
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov %edi, %ecx
    mov $file_size, %eax
    mov 0(%eax), %edx
    mov $0x3, %eax
    int $0x80
    push %eax
    call has_error
    add $4, %esp
    /* use sys_close to close file */
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov $0x6, %eax
    int $0x80
    push %eax
    call has_error
    add $4, %esp
    mov $file_size, %eax
    mov 0(%eax), %eax
    /* call merge sort */
    mov $0, %edx
    mov $4, %ecx
    idiv %ecx
    push %eax
    push %edi
    call sort
    add $8, %esp
    /* use sys_open to open output file */
    mov 12(%ebp), %eax
    mov 8(%eax), %ebx
    mov $601, %ecx
    mov $0644, %edx
    mov $0x5, %eax
    int $0x80
    mov %eax, fd
    push %eax
    call has_error
    add $4, %esp
    /* write output file */
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov %edi, %ecx
    mov $file_size, %eax
    mov 0(%eax), %edx
    mov $0x4, %eax
    int $0x80
    mov $file_size, %ebx
    mov 0(%ebx), %ebx
    cmp %eax, %ebx
    je write_correct
write_has_error:
    push $-1
    call has_error
write_correct:
    mov $fd, %eax
    mov 0(%eax), %ebx
    mov $0x6, %eax
    int $0x80
    push %eax
    call has_error
    add $4, %esp
    /* free memory on the stack */
    mov $file_size, %eax
    mov 0(%eax), %edx
    add %edx, %esp
    mov $0, %eax
    jmp main_end
output_usage:
    push $usage_msg
    call printf
    add $4, %esp
    jmp main_end
main_end:
    leave
    ret

