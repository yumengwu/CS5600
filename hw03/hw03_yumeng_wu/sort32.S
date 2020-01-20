.global main
.data
    usage_msg:  .string "Usage: ./sort input output\n"
    error_msg:  .string "Error: "
    fd: .int 0
    file_start_off: .long 0
    file_end_off: .long 0
    len: .long 0
    test_int: .string "%d\n"
.text
has_error:
    enter $0, $0
    mov 8(%ebp), %eax 
    cmp $0, %eax 
    jge has_error_end 
    push $error_msg 
    call perror 
    add $4, %esp 
    mov $1, %ebx 
    mov $0x1, %eax 
    int $0x80 
has_error_end:
    mov $0, %eax 
    leave 
    ret
main:
    enter $0, $0
    mov 8(%ebp), %eax 
    cmp $3, %eax 
    jne output_usage 
    mov 12(%ebp), %eax 
    mov 4(%eax), %ebx 
    mov $0, %ecx 
    mov $0, %edx 
    mov $0x5, %eax 
    int $0x80 
    cmp $0, %eax 
    mov %eax, fd 
    mov $fd, %eax 
    push 0(%eax)
    call has_error 
    add $4, %esp 
    mov $0, %ecx 
    mov $0, %edx 
    mov $fd, %eax 
    mov 0(%eax), %ebx 
    mov $0x13, %eax 
    int $0x80 
    mov %eax, file_start_off 
    mov $0, %ecx 
    mov $2, %edx 
    mov $fd, %eax 
    mov 0(%eax), %ebx 
    mov $0x13, %eax 
    int $0x80 
    mov %eax, file_end_off
    mov $file_start_off, %ebx 
    mov 0(%ebx), %ebx 
    sub %ebx, %eax 
    mov %eax, len 
    mov $0, %eax 
    jmp main_end 
output_usage:
    push $usage_msg 
    call printf 
    add $4, %esp 
main_end:
    leave
    ret
