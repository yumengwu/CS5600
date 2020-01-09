  .global main

  .text
main:
  enter $0, $0
/*
  mov 8(%rsi), %rdi
  call atol

  mov %rax, %rsi
  mov $output_fmt, %rdi
  mov $0, %al
  call printf
*/ 
    
  leave
  ret

  .data
output_fmt:  .string "fib(3) = %ld\n"
