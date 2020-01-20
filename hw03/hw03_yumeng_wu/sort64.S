    .global main
    .data
usage_msg:  .string "Usage: ./sort input output"

    .text
main:
    enter $0, $0

    mov $usage_msg, %rdi
    call puts  // FIXME: That's not a syscall

    mov $2, %rax
    leave
    ret
