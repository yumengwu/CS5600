.global main
.data
    usage_msg: .string "Usage: ./sort input-file output-file\n"
    error_msg: .string "Error"
    fd: .int 0
    file_size: .long 0
    test_int: .string "%d\n"
    test_allo: .string "alloc %d\n"
    test_free: .string "free %d\n"
.text
// check if sys call has error 
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

// merge sort 
// void merge(int * array, int left, int right)
merge_sort:
    enter $0, $0
    push %edi 
    push %esi 
    push %ebx 
    // if (right - left < 1) return 
    mov 16(%ebp), %eax // right 
    mov 12(%ebp), %ecx // left 
    sub %ecx, %eax 
    cmp $1, %eax 
    jle merge_sort_ret 
    shr $1, %eax 
    // mid = left + (right - left) / 2
    mov %eax, %ebx 
    add 12(%ebp), %ebx 
    // allocate left array 
    mov %ebx, %edx 
    sub 12(%ebp), %edx 
    imul $4, %edx 
    sub %edx, %esp 
    lea 0(%esp), %edi 
    // copy left array 
    mov %ebx, %edx 
    sub 12(%ebp), %edx 
    mov $0, %ecx 
    push %ebx 
    mov 8(%ebp), %ebx 
    add 12(%ebp), %ecx 
    add 12(%ebp), %edx 
merge_copy_left_loop_start:
    cmp %edx, %ecx 
    je merge_copy_left_loop_end 
    mov (%ebx, %ecx, 4), %eax 
    sub 12(%ebp), %ecx 
    mov %eax, (%edi, %ecx, 4) 
    add 12(%ebp), %ecx 
    inc %ecx 
    jmp merge_copy_left_loop_start 
merge_copy_left_loop_end:
    pop %ebx 
    // recursively sort left array 
    push %ebx 
    push $0 
    push %edi 
    call merge_sort 
    add $12, %esp 
    // allocate right array 
    mov 16(%ebp), %edx 
    sub %ebx, %edx 
    imul $4, %edx 
    sub %edx, %esp 
    lea 0(%esp), %esi 
    // copy right array 
    push %edi 
    mov $0, %ecx 
    mov 8(%ebp), %edi 
    mov 16(%ebp), %edx 
    sub %ebx, %edx 
merge_copy_right_loop_start:
    cmp %edx, %ecx 
    je merge_copy_right_end 
    add %ebx, %ecx 
    mov (%edi, %ecx, 4), %eax 
    sub %ebx, %ecx 
    mov %eax, (%esi, %ecx, 4) 
    inc %ecx 
    jmp merge_copy_right_loop_start 
merge_copy_right_end:
    pop %edi 
    // recursively solve right 
    mov 16(%ebp), %edx 
    sub %ebx, %edx 
    push %edx 
    push $0 
    push %esi 
    call merge_sort 
    add $12, %esp 
    // merge back 
    mov %ebx, %ecx 
    sub 12(%esp), %ecx 
    mov 16(%esp), %edx 
    sub %ebx, %edx 
    push %ebx 
    mov $0, %ebx 
merge_back_both_start:
push $usage_msg 
call printf 
add $4, %esp 
    cmp $0, %ecx 
    je merge_back_both_end 
    cmp $0, %edx 
    je merge_back_both_end 
    mov 0(%edi), %eax 
    cmp 0(%esi), %eax 
    jle merge_both_left 
    jmp merge_both_right 
merge_both_left:
push 0(%edi)
push $test_int 
call printf 
add $8, %esp 
    mov 0(%edi), %eax 
    push %esi 
    mov 8(%ebp), %esi 
    mov %eax, (%esi, %ebx, 4) 
    pop %esi 
    dec %ecx 
    add $4, %edi 
    inc %ebx 
    jmp merge_back_both_start 
merge_both_right:
push 0(%esi) 
push $test_int 
call printf 
add $8, %esp 
    mov 0(%esi), %eax 
    push %edi 
    mov 8(%ebp), %edi 
    mov %eax, (%edi, %ebx, 4) 
    pop %edi 
    dec %edx 
    add $4, %esi 
    inc %ebx 
    jmp merge_back_both_start 
merge_back_both_end:
/*
    cmp $0, %ecx 
    jg merge_left_back 
    cmp $0, %edx 
    jg merge_right_back 
    jmp merge_back_end 
merge_left_back:
    cmp $0, %ecx 
    je merge_back_end 
    mov 0(%edi), %eax 
    mov 8(%ebp), %edx 
    mov %eax, (%edx, %ebx, 4) 
    dec %ecx 
    inc %ebx 
    add $4, %edi 
    jmp merge_left_back 
merge_right_back:
    cmp $0, %edx 
    je merge_back_end 
    mov 0(%esi), %eax 
    mov 8(%ebp), %ecx 
    mov %eax, (%ecx, %ebx, 4) 
    dec %edx 
    inc %ebx 
    add $4, %esi 
    jmp merge_right_back */ 
merge_back_end:
    pop %ebx 
    // free left array 
    mov %ebx, %edx 
    imul $4, %edx 
    sub 12(%ebp), %edx 
    add %edx, %esp 
    // free right array 
    mov 16(%ebp), %edx 
    sub %ebx, %edx 
    imul $4, %edx 
    add %edx, %esp 
merge_sort_ret: 
    pop %ebx 
    pop %esi 
    pop %edi
    leave 
    ret 

// merge sort entrance
// void sort(int * array, int bytes)
sort:
    enter $0, $0
    push %edi 
    mov 12(%ebp), %eax 
    mov $0, %edx 
    mov $4, %ecx 
    idiv %ecx 
    push %eax 
    push $0 
    push 8(%ebp) 
    call merge_sort 
    add $12, %esp 
    pop %edi 
    leave 
    ret 

main:
    enter $0, $0
    // compare argc
    mov 8(%ebp), %eax 
    cmp $3, %eax 
    jne output_usage 
    // use system call sys_open to open file argv[1]
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
    // use sys_lseek to calculate file size 
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
    // use sys_lseek to reset offset 
    mov $0, %ecx 
    mov $0, %edx 
    mov $fd, %eax 
    mov 0(%eax), %ebx 
    mov $0x13, %eax 
    int $0x80 
    push %eax 
    call has_error 
    add $4, %esp 
    // allocate space on the stack 
    mov $file_size, %eax 
    mov 0(%eax), %edx 
    sub %edx, %esp 
    lea 0(%esp), %edi 
    // use sys_read to read file 
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
    // use sys_close to close file 
    mov $fd, %eax 
    mov 0(%eax), %ebx 
    mov $0x6, %eax 
    int $0x80 
    push %eax 
    call has_error 
    add $4, %esp 
    mov $file_size, %eax 
    // call merge sort 
    push 0(%eax) 
    push %edi 
    call sort 
    add $8, %esp 
    // use sys_open to open output file 
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
    // write output file 
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
    // free memory on the stack 
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

