.global i_case_compare                          # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
.text
i_case_compare:
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
    pcmpistrm   $0x44,      %xmm10, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    movdqu      %xmm10,     %xmm14              # COPY: xmm10 --> xmm14
    paddb       %xmm0,      %xmm14              # LOWER: TO_ADD + COPY --> xmm14

    /* Converting str2 to Lower */
    pcmpistrm   $0x44,      %xmm11, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    movdqu      %xmm11,     %xmm15              # COPY: xmm11 --> xmm15
    paddb       %xmm0,      %xmm15              # LOWER: TO_ADD + COPY --> xmm15



    /* Check str1 and str2 are identical or not */
    pcmpistrm   $0x18,      %xmm14, %xmm15      # compare two sse register completely equal or not
    movq        %xmm0,      %r8                 # move the result of xmm0 register to r8 register (temp) to perform cmp instruction
    sub         $0x00,      %r8                 # check the output after pcmpistrm comparison
    jz          head_loop
    jnz         return_result_mismatch

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
    movdqu      %xmm10,     %xmm14
    paddb       %xmm0,      %xmm14

    pcmpestrm   $0x44,      %xmm11, %xmm12
    pand        %xmm13,     %xmm0
    movdqu      %xmm11,     %xmm15
    paddb       %xmm0,      %xmm15

    movq %rdx, %rax
    pcmpestrm   $0x18,      %xmm14, %xmm15      # compare two sse register completely equal or not
    movq        %xmm0,      %r8                 # move the result of xmm0 register to r8 register (temp) to perform cmp instruction
    pop %rax
    sub         $0x00,      %r8
    jz          return_result_match
    jnz         return_result_mismatch

return_result_mismatch:
    movq        $0x01,      %rax
    ret

return_result_match:
    movq        $0x00,      %rax
    ret
