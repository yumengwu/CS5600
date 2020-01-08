  .global main

  .text
main:
  mov %rdi, %r10    # argc
  mov %rsi, %r11    # argv

  cmp $2, %r10
  jne usage

  mov 8(%r11), %rdi   # atol(argv[1])
  call atol
  mov %rax, %rdi

  call square
  mov %rax, %rsi

  mov $output_msg, %rdi
  mov $0, %al
  call printf

  ret

  # This is a label, but not a function.
usage:
  mov $usage_msg, %rdi
  call puts
  ret


  .text
usage_msg:  .string "Usage: ./square N"
output_msg: .asciz  "result = %ld\n"

