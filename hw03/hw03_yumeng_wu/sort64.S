.global main
.data
    usage_msg: .string "Usage: ./sort input-file output-file\n"
    error_msg: .string "Error"
    fd: .int 0
    file_size: .long 0
    test_int: .string "%d\n"
    test_long: .string "%ld\n"
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

/* merge sort
 * void sort(int * array, long length);
 */
sort:
    enter $0, $0
    cmp $1, %rsi
    jle sort_return
    push %r12   // right
    push %r13   // array
    push %r14   // left
    push %r15   // right
    /* save parameters */
    mov %rsi, %r12
    mov %rdi, %r13
    /* allocate left subarray */
    mov %r12, %rdx
    shl $1, %rdx
    sub %rdx, %rsp
    lea 0(%rsp), %r14
    /* copy left subarray */
    shr $2, %rdx
    mov $0, %rcx
sort_copy_left_loop:
    cmp %rdx, %rcx
    je sort_copy_left_end
    mov (%r13, %rcx, 4), %eax
    mov %eax, (%r14, %rcx, 4)
    inc %rcx
    jmp sort_copy_left_loop
sort_copy_left_end:
    /* allocate right subarray */
    mov %r12, %rdx
    mov %r12, %rax
    shr $1, %rax
    sub %rax, %rdx
    shl $2, %rdx
    sub %rdx, %rsp
    lea 0(%rsp), %r15
    /* copy right subarray */
    shr $2, %rdx
    mov $0, %rcx
    mov %r12, %rdi
    shr $1, %rdi
sort_copy_right_loop:
    cmp %rdx, %rcx
    je sort_copy_right_end
    mov %rdi, %rsi
    add %rcx, %rsi
    mov (%r13, %rsi, 4), %eax
    mov %eax, (%r15, %rcx, 4)
    inc %rcx
    jmp sort_copy_right_loop
sort_copy_right_end:
    /* recursively solve left subarray */
    mov %r14, %rdi
    mov %r12, %rsi
    shr $1, %rsi
    call sort
    /* recursively solve right subarray */
    mov %r15, %rdi
    mov %r12, %rsi
    mov %r12, %rdx
    shr $1, %rdx
    sub %rdx, %rsi
    call sort
    /* write both left and right back */
    mov %r12, %rcx
    shr $1, %rcx
    mov %r12, %rdx
    sub %rcx, %rdx
sort_write_both_back_loop:
    cmp $0, %rcx
    je sort_write_both_back_end
    cmp $0, %rdx
    je sort_write_both_back_end
    mov 0(%r14), %edi
    mov 0(%r15), %esi
    cmp %esi, %edi
    jle sort_write_both_back_left
    jmp sort_write_both_back_right
sort_write_both_back_left:
    mov %edi, 0(%r13)
    add $4, %r13
    add $4, %r14
    dec %rcx
    jmp sort_write_both_back_loop
sort_write_both_back_right:
    mov %esi, 0(%r13)
    add $4, %r13
    add $4, %r15
    dec %rdx
    jmp sort_write_both_back_loop
sort_write_both_back_end:
    /* sort write left back */
    cmp $0, %rcx
    jg sort_write_left_back_loop
    cmp $0, %rdx
    jg sort_write_right_back_loop
    jmp sort_write_back_end
sort_write_left_back_loop:
    cmp $0, %rcx
    je sort_write_back_end
    mov 0(%r14), %edi
    mov %edi, 0(%r13)
    add $4, %r13
    add $4, %r14
    dec %rcx
    jmp sort_write_left_back_loop
sort_write_right_back_loop:
    cmp $0, %rdx
    je sort_write_back_end
    mov 0(%r15), %edi
    mov %edi, 0(%r13)
    add $4, %r13
    add $4, %r15
    dec %rdx
    jmp sort_write_right_back_loop
sort_write_back_end:
    /* free left subarray */
    mov %r12, %rdx
    shl $1, %rdx
    add %rdx, %rsp
    /* free right subarray */
    mov %r12, %rdx
    mov %r12, %rax
    shr $1, %rax
    sub %rax, %rdx
    shl $2, %rdx
    add %rdx, %rsp
    /* finish */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
sort_return:
    leave
    ret

/* main function */
main:
    enter $0, $0
    cmp $3, %rdi
    jne output_usage
    /* save argv.
     * argv will be stored in r12
     */
    mov %rsi, %r12
    /* use sys_open to open argv[1] */
    mov 8(%r12), %rdi
    mov $0, %rsi
    mov $2, %rax
    syscall
    mov %rax, fd
    mov %rax, %rdi
    call has_error
    /* use sys_lseek to get file size */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov $0, %rsi
    mov $2, %rdx
    mov $8, %rax
    syscall
    mov %rax, file_size
    mov %rax, %rdi
    call has_error
    /* use sys_lseek to reset offset */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov $0, %rsi
    mov $0, %rdx
    mov $8, %rax
    syscall
    /* allocate space */
    mov $file_size, %rax
    mov 0(%rax), %rdx
    /* file size will be stored in r13 */
    mov %rdx, %r13
    sub %rdx, %rsp
    /* array will be stored in r14 */
    lea 0(%rsp), %r14
    /* use sys_read to read file */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov %r14, %rsi
    mov $file_size, %rax
    mov 0(%rax), %rdx
    mov $0, %rax
    syscall
    mov %rax, %rdi
    call has_error
    /* use sys_close to close argv[1] */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov $3, %rax
    syscall
    mov %rax, %rdi
    call has_error
    /* call merge sort function */
    mov $file_size, %rdx
    mov 0(%rdx), %rsi
    shr $2, %rsi
    mov %r14, %rdi
    call sort
    /* use sys_open to open output file */
    mov 16(%r12), %rdi
    mov $601, %rsi
    mov $0644, %rdx
    mov $2, %rax
    syscall
    mov %eax, fd
    mov %rax, %rdi
    call has_error
    /* use sys_write to write output file */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov %r14, %rsi
    mov $file_size, %rax
    mov 0(%rax), %rdx
    mov $1, %rax
    syscall
    mov $file_size, %rdx
    mov 0(%rdx), %rdx
    cmp %rax, %rdx
    je write_file_correct
    mov $-1, %rdi
    call has_error
write_file_correct:
    /* use sys_close to close output file */
    mov $fd, %rax
    mov $0, %rdi
    mov 0(%rax), %edi
    mov $3, %rax
    syscall
    mov %rax, %rdi
    call has_error
    /* free space */
    add %r13, %rsp
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
