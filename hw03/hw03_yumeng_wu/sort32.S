    .global main
    .data
usage_msg:  .string "Usage: ./sort input output"

    .text
main:
    enter $0, $0

    push $usage_msg
    call puts // FIXME: that's not a syscall
    add $4, %esp

    mov $2, %eax
    leave
    ret
