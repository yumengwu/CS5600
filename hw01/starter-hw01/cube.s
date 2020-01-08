.data
.text
.global cube
cube:
    enter $0, $0
    mov %edi, %eax
    imul %edi
    imul %edi
    leave
    ret

