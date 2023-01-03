.section .text
.global sub_check_x86_64
sub_check_x86_64:
    mov $15, %rdx
    sub $16, %rdx
    ret


.global getsum_x86_64_GAS                   # parameters are in rcx and rdx
.text
getsum_x86_64_GAS:
    mov         (%rcx),     %rax            # store value of rcx into rax
    add         (%rdx),     %rax            # add value of rdx with rax
    ret

.section .text
.global packed_suturation_test
packed_suturation_test:
    #load 128 bit inData into register xmm9
    movq        (%rcx),     %xmm9           # move first 64 bit to xmm9 register(SSE)
    add         $8,         %rcx            # rcx is pointer of inData
                                            # this instruction will skip first 8 byte of inData
    movq        (%rcx),     %xmm11          # move second 64 bit to xmm11 register(SSE), xmm11 is used as temp storage
    pslldq      $8,         %xmm11          # shift left logical by 8 bytes = 64bit(moving data from lowerbits 64bit to upper 64bit
    por         %xmm11,     %xmm9

    #load 128 bit outData into register xmm10
    movq        (%rdx),     %xmm10           # move first 64 bit to xmm10 register(SSE)

    add         $8,         %rdx            # rdx is pointer of outData
                                            # this instruction will skip first 8 byte of outData
    movq        (%rdx),     %xmm11          # move second 64 bit to xmm11 register(SSE), xmm11 is used as temp storage
    pslldq      $8,         %xmm11           # shift left logical by 8 bytes = 64bit(moving data from lowerbits 64bit to upper 64bit
    por         %xmm11,     %xmm10

    #shufps $0x1b, %xmm10,%xmm10

    movq %xmm10, %xmm11

    #movhlps     %xmm9, %xmm11

    #movhlps     xmm9,       xmm9            # move high 64 bits to low 64 bits.

    packsswb    %xmm9,      %xmm10
    movq %xmm10, (%rdx)
    psrldq      $8,         %xmm10          # shift right logical by 8 bytes = 64bit(moving data from upper 64bit to lower 64bit
    sub         $8,         %rdx
    movq %xmm10, (%rdx)

    ret



.global search_substr                        # parameters are in str = rcx and substr = rdx
.text
search_substr:
    movq        (%rcx),     %xmm9           # move first 64 bit to xmm9 register(SSE)
    add         $8,         %rcx            # will skip first 8 byte of inData
    movq        (%rcx),     %xmm11          # move second 64 bit to xmm11 register(SSE), xmm11 is used as temp storage
    pslldq      $8,         %xmm11          # shift left logical by 8 bytes = 64bit(moving data from lowerbits 64bit to upper 64bit
    por         %xmm11,     %xmm9

    movq        (%rdx),     %xmm10           # move first 64 bit to xmm9 register(SSE)

    pcmpistrm  $0x0C,  %xmm9,     %xmm10

    movq %xmm0, %rax
    ret


.section .text
.global compare_string                          # parameters are in *str1 = rax,  *str2 = rsi, str1len = rcx, str2len = rdx
compare_string:

loop_label:
    cmp         $0x00,      %rcx                # check if all characters are compared
    je          return_match                    # ensures that all characters were matched
    sub         $16,         %rcx                # substruct by 8
    movdqu      (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)
    add         $16,         %rax                # skip the pointer by 8 bytes
    add         $16,         %rsi                # skip the pointer by 8 bytes
    pcmpistrm   $0x18,      %xmm10,     %xmm11  # compare two sse register completely equal or not
    movq        %xmm0,      %r8                 # move the result of xmm0 register to r8 register (temp) to perform cmp instruction
    cmp         $0x00,      %r8                 # check the output after pcmpistrm comparison
    je          loop_label
    jne         return_mismatch

return_mismatch:
    movq        $0x01,      %rax
    ret

return_match:
    movq        $0x00,      %rax
    ret


.section .text
.global compare_string_case_insensitive         # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
compare_string_case_insensitive:
    /*Prepare some constant*/
    movq        $0x5A41,     %r12               #A: 0x41, Z: 0x5A --> defining Range
    movq        %r12,        %xmm12             #CHARACTER_RANGE

    movq        $0x20202020,       %r12         #Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
    movq        %r12,        %xmm13             #moving diff to SSE register
    pshufd      $0,          %xmm13,  %xmm13    #CHARACTER_DIFF

head_loop:
    sub         $16,        %rdx                # check if all characters are compared
    jle         prepare_explicit_length         # ensures that all characters were matched

    movdqu      (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)
    add         $16,         %rax               # skip the pointer by 8 bytes
    add         $16,         %rsi               # skip the pointer by 8 bytes

    /* Converting str1 to Lower */
    pcmpistrm   $0x44,      %xmm10, %xmm12       # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm10              # LOWER: TO_ADD + COPY --> xmm10

    /* Converting str2 to Lower */
    pcmpistrm   $0x44,      %xmm11, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm11              # LOWER: TO_ADD + COPY --> xmm11



    /* Check str1 and str2 are identical or not */
    pcmpistrm   $0x18,      %xmm10, %xmm11      # compare two sse register completely equal or not
    movq        %xmm0,      %r12                 # move the result of xmm0 register to r8 register (temp) to perform cmp instruction
    sub         $0x00,      %r12                 # check the output after pcmpistrm comparison
    jz          head_loop
    jnz         prepare_intermediate_mismatch

prepare_explicit_length:
    add         $16,        %rdx
    jmp explicit_length_compare

explicit_length_compare:
    movdqu      (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)

    push %rax
    movq $2, %rax

    pcmpestrm   $0x44,      %xmm10, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm10

    pcmpestrm   $0x44,      %xmm11, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm11

    movq %rdx, %rax
    pcmpestrm   $0x18,      %xmm10, %xmm11      # compare two sse register completely equal or not
    movq        %xmm0,      %r12                 # move the result of xmm0 register to r8 register (temp) to perform cmp instruction
    pop %rax
    sub         $0x00,      %r12
    jz          return_result_match
    jnz         tail_loop

prepare_intermediate_mismatch:
    sub         $16,         %rax
    sub         $16,         %rsi
    add         $16,         %rdx
    jmp tail_loop


tail_loop:
    movb (%rax), %r13b
    movb (%rsi), %r14b
    add         $1,         %rax
    add         $1,         %rsi

    /* convert single byte from str1 to lower*/
str1:
    cmp $0x41, %r13b
    jl str2
    cmp $0x5A, %r13b
    jg str2
    add $32, %r13b

    /* convert single byte from str2 to lower*/
str2:
    cmp $0x41, %r14b
    jl compare
    cmp $0x5A, %r14b
    jg compare
    add $32, %r14b

compare:
    sub %r14b, %r13b
    jnz return_result_mismatch
    sub $1, %rdx
    jnz tail_loop
    jz return_result_match



return_result_mismatch:
    movq        $0x00,      %rax
    mov %r13b, %al
    ret

return_result_match:
    movq        $0x00,      %rax
    ret



.section .text
.global test_movdqa_x86_64         # parameters are in *lhs = rax,  *rhs = rsi, len = rdx
test_movdqa_x86_64:
    movdqu (%rax), %xmm11
    movdqu (%rsi), %xmm12
#    movdq $0, %xmm11
#    loadss        $0x5A41,        %xmm12
    subps %xmm11, %xmm12
#    psllq $a,  %xmm10

#    mov     $format, %rdi
    mov $0, %rax
    ret
