.global main
.data
    usage_msg: .string "Usage: ./sort input-file output-file\n"
    error_msg: .string "Error"
    fd: .int 0
    file_size: .long 0
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
    /* use sys_exit to exit with status 1 */
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
 * left will be zero, right will be length
 */
sort:
    enter $0, $0
    push %edi
    push %esi
    push %ebx
    /* if (right <= left + 1) return; */
    mov 12(%ebp), %eax
    cmp $1, %eax
    jle sort_ret
    /* calculate mid = (right + left) / 2 */
    shr $1, %eax
    /* mid will be in ebx */
    mov %eax, %ebx
    /* allocate left subarray, left subarray will be stored in edi */
    mov %ebx, %edx
    shl $2, %edx
    sub %edx, %esp
    lea 0(%esp), %edi
    /* copy left subarray */
    mov $0, %ecx
sort_copy_left_array_loop:
    cmp %ebx, %ecx
    je sort_copy_left_array_end
    push %ebx
    mov 8(%ebp), %ebx
    mov (%ebx, %ecx, 4), %eax
    pop %ebx
    mov %eax, (%edi, %ecx, 4)
    inc %ecx
    jmp sort_copy_left_array_loop
sort_copy_left_array_end:
    /* allocate right subarray, right subarray will be stored in esi */
    mov 12(%ebp), %edx
    sub %ebx, %edx
    shl $2, %edx
    sub %edx, %esp
    lea 0(%esp), %esi
    /* copy right subarray */
    mov %ebx, %ecx
sort_copy_right_array_loop:
    cmp 12(%ebp), %ecx
    je sort_copy_right_array_end
    push %ebx
    mov 8(%ebp), %ebx
    mov (%ebx, %ecx, 4), %eax
    pop %ebx
    sub %ebx, %ecx
    mov %eax, (%esi, %ecx, 4)
    inc %ecx
    add %ebx, %ecx
    jmp sort_copy_right_array_loop
sort_copy_right_array_end:
    /* recursively solve left part */
    push %ebx
    push %edi
    call sort
    add $8, %esp
    /* recursively solve right part */
    mov 12(%ebp), %eax
    sub %ebx, %eax
    push %eax
    push %esi
    call sort
    add $8, %esp
    /* copy back both left and right */
    mov %ebx, %ecx
    mov 12(%ebp), %edx
    sub %ebx, %edx
    push %ebx
    mov 8(%ebp), %ebx
sort_copy_back_both_loop:
    cmp $0, %ecx
    je sort_copy_back_both_end
    cmp $0, %edx
    je sort_copy_back_both_end
    mov 0(%edi), %eax
    cmp 0(%esi), %eax
    jle sort_copy_back_both_left
    jmp sort_copy_back_both_right
sort_copy_back_both_left:
    mov 0(%edi), %eax
    mov %eax, 0(%ebx)
    add $4, %ebx
    add $4, %edi
    dec %ecx
    jmp sort_copy_back_both_loop
sort_copy_back_both_right:
    mov 0(%esi), %eax
    mov %eax, 0(%ebx)
    add $4, %ebx
    add $4, %esi
    dec %edx
    jmp sort_copy_back_both_loop
sort_copy_back_both_end:
    mov %ebx, 8(%ebp)
    pop %ebx
    /* copy other numbers back */
    cmp $0, %ecx
    jg sort_copy_back_left_loop
    cmp $0, %edx
    jg sort_copy_back_right_loop
    jmp sort_copy_back_finish
sort_copy_back_left_loop:
    cmp $0, %ecx
    je sort_copy_back_finish
    mov 0(%edi), %eax
    mov 8(%ebp), %esi
    mov %eax, 0(%esi)
    add $4, %edi
    add $4, %esi
    mov %esi, 8(%ebp)
    dec %ecx
    jmp sort_copy_back_left_loop
sort_copy_back_right_loop:
    cmp $0, %edx
    je sort_copy_back_finish
    mov 0(%esi), %eax
    mov 8(%ebp), %edi
    mov %eax, 0(%edi)
    add $4, %esi
    add $4, %edi
    mov %edi, 8(%ebp)
    dec %edx
    jmp sort_copy_back_right_loop
sort_copy_back_finish:
    /* free left subarray */
    mov %ebx, %edx
    shl $2, %edx
    add %edx, %esp
    /* free right subarray */
    mov 12(%ebp), %edx
    sub %ebx, %edx
    shl $2, %edx
    add %edx, %esp
sort_ret:
    pop %ebx
    pop %esi
    pop %edi
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
