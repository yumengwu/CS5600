.global main
.data
    usage_msg: .string "Usage: ./ccalc N op N\n"
    output_one_string: .string "%s "
    output_number: .string "= %d\n"
.text

/* calculate */
calc:
    enter $0, $0
    /* compare operator */
    cmp $43, %rdx 
    je calcPlus 
    cmp $45, %rdx 
    je calcSub 
    cmp $42, %rdx 
    je calcMul 
    cmp $47, %rdx 
    je calcDiv 
calcPlus:
    mov %rdi, %rax 
    add %rsi, %rax 
    jmp calcEnd 
calcSub:
    mov %rdi, %rax 
    sub %rsi, %rax 
    jmp calcEnd 
calcMul:
    mov %rdi, %rax 
    imul %rsi, %rax 
    jmp calcEnd 
calcDiv:
    mov $0, %rdx 
    mov %rdi, %rax 
    idiv %rsi 
calcEnd:
    leave 
    ret 

/* Check if given string is a valid number. A valid number can only cantain 0~9. */
checkNumber:
    enter $0, $0
    mov $0, %rcx 
checkNumberLoop:
    mov (%rdi, %rcx, 1), %al 
    cmp $0, %al 
    je checkNumberTrue 
    cmp $48, %al 
    jl checkNumberFalse 
    cmp $57, %al 
    jg checkNumberFalse 
    inc %rcx 
    jmp checkNumberLoop 
checkNumberTrue:
    mov $1, %rax 
    jmp checkNumberEnd 
checkNumberFalse:
    mov $0, %rax 
checkNumberEnd:
    leave 
    ret 

/* Check if given string is a valid operator. */
checkOp:
    enter $0, $0
    mov $0, %rax 
    mov 1(%rdi), %al 
    cmp $0, %al 
    jne checkOpFalse 
    mov 0(%rdi), %al 
    /* + */
    cmp $43, %al 
    je checkOpTrue 
    /* - */
    cmp $45, %al 
    je checkOpTrue 
    /* * */
    cmp $42, %al 
    je checkOpTrue 
    /* / */
    cmp $47, %al 
    je checkOpTrue 
    jmp checkOpFalse 
checkOpTrue:
    mov $1, %rax 
    jmp checkOpEnd 
checkOpFalse:
    mov $0, %rax 
checkOpEnd:
    leave 
    ret 

main:
    enter $0, $0
    cmp $4, %rdi 
    jne output_usage 
    mov %rsi, %r12 
    mov 8(%r12), %rdi 
    /* check the first number */
    call checkNumber 
    cmp $0, %rax 
    je output_usage 
    mov 24(%r12), %rdi 
    /* check the second number */
    call checkNumber 
    cmp $0, %rax 
    je output_usage 
    mov 16(%r12), %rdi 
    /* check operator */
    call checkOp 
    cmp $0, %rax 
    je output_usage 
    mov 8(%r12), %rdi 
    /* convert the first number */
    call atol
    mov %rax, %r13 
    mov 24(%r12), %rdi 
    /* convert the second number */
    call atol 
    mov %rax, %r14 
    mov 16(%r12), %rcx 
    mov $0, %rdx 
    mov 0(%rcx), %dl 
    mov %r13, %rdi 
    mov %r14, %rsi 
    call calc 
    mov %rax, %r15
    mov $output_one_string, %rdi 
    mov 8(%r12), %rsi 
    mov $0, %rax 
    call printf 
    mov $output_one_string, %rdi 
    mov 16(%r12), %rsi 
    mov $0, %rax 
    call printf 
    mov $output_one_string, %rdi 
    mov 24(%r12), %rsi 
    mov $0, %rax 
    call printf 
    mov $output_number, %rdi 
    mov %r15, %rsi 
    mov $0, %rax 
    call printf 
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
