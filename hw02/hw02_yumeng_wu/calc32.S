.global main
.data
    usage_msg: .string "Usage: ./calc N op N\n"
    output_one_string: .string "%s "
    output_number: .string "= %ld\n"
.text

/* calculate */
calc:
    enter $0, $0
    /* get first number */
    mov 8(%ebp), %ebx 
    /* get second number */
    mov 12(%ebp), %ecx 
    /* get operator */
    mov 16(%ebp), %edx 
    cmp $43, %edx 
    je calcPlus 
    cmp $45, %edx 
    je calcSub 
    cmp $42, %edx 
    je calcMul 
    cmp $47, %edx 
    je calcDiv 
calcPlus:
    mov %ebx, %eax 
    add %ecx, %eax 
    jmp calcEnd 
calcSub:
    mov %ebx, %eax 
    sub %ecx, %eax 
    jmp calcEnd 
calcMul:
    mov %ebx, %eax 
    imul %ecx, %eax 
    jmp calcEnd 
calcDiv:
    mov $0, %edx 
    mov %ebx, %eax 
    idiv %ecx 
calcEnd:
    leave 
    ret 

/* Check if given string is a valid number. A valid number can only cantain 0~9. */
checkNumber:
    enter $0, $0
    mov 8(%ebp), %edx
    mov $0, %ecx 
checkNumberLoopStart:
    mov (%edx, %ecx, 1), %al 
    cmp $0, %al 
    je checkNumberTrue 
    cmp $48, %al 
    jl checkNumberFalse 
    cmp $57, %al 
    jg checkNumberFalse 
    inc %ecx 
    jmp checkNumberLoopStart 
checkNumberTrue:
    mov $1, %eax 
    jmp checkNumberEnd 
checkNumberFalse:
    mov $0, %eax 
    jmp checkNumberEnd 
checkNumberEnd:
    leave 
    ret 

/* Check if given string is a valid operator. */
checkOp:
    enter $0, $0
    mov 8(%ebp), %edx 
    mov 1(%edx), %al 
    /* Check if this is a single char */
    cmp $0, %al 
    jne checkOpFalse 
    mov 0(%edx), %al 
    /* Check + operator */
    cmp $43, %al 
    je checkOpTrue 
    /* Check - operator */
    cmp $45, %al 
    je checkOpTrue 
    /* Check * operator */
    cmp $42, %al 
    je checkOpTrue 
    /* Check / operator */
    cmp $47, %al 
    je checkOpTrue 
    jmp checkOpFalse 
checkOpTrue:
    mov $1, %eax 
    jmp checkOpEnd 
checkOpFalse:
    mov $0, %eax 
checkOpEnd:
    leave 
    ret 

main:
    enter $0, $0
    /* get argc */
    mov 8(%ebp), %eax 
    cmp $4, %eax 
    jne output_usage 
    /* get argv */
    mov 12(%ebp), %edi
    push 4(%edi)  
    /* check the first number */
    call checkNumber 
    add $4, %esp 
    cmp $0, %eax 
    je output_usage 
    push 12(%edi) 
    /* check the second number */
    call checkNumber 
    add $4, %esp 
    cmp $0, %eax 
    je output_usage 
    push 8(%edi)
    /* check operator */
    call checkOp 
    add $4, %esp 
    cmp $0, %eax 
    je output_usage 
    push 4(%edi) 
    /* convert the first number */
    call atoi 
    add $4, %esp 
    mov %eax, %esi 
    push 12(%edi) 
    /* convert the second number */
    call atoi 
    add $4, %esp 
    mov $0, %ecx 
    mov 8(%edi), %edx 
    mov 0(%edx), %cl 
    push %ecx 
    push %eax 
    push %esi 
    call calc 
    add $12, %esp 
    mov %eax, %esi 
    push 4(%edi) 
    push $output_one_string 
    call printf 
    add $8, %esp 
    push 8(%edi) 
    push $output_one_string 
    call printf 
    add $8, %esp 
    push 12(%edi) 
    push $output_one_string 
    call printf 
    add $8, %esp 
    push %esi 
    push $output_number 
    call printf 
    add $8, %esp 
    mov $0, %eax 
    jmp main_end 
output_usage:
    push $usage_msg 
    call printf 
    pop %eax 
    mov $1, %eax 
main_end:
    leave
    ret

