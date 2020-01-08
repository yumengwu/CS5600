.data
.text
.global add1
add1:
    enter $0, $0
    push %ebp
    mov 8(%ebp), %eax
    inc %eax
    pop %ebp
    leave
    ret
