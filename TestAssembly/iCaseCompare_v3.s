/*
only pcmpestrm
*/

.section .text
.global i_case_compare_v3                          # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
i_case_compare_v3:
    /*Prepare some constant*/
    movq        $0x5A41,     %r12               #A: 0x41, Z: 0x5A --> defining Range
    movq        %r12,        %xmm12             #CHARACTER_RANGE

    movq        $0x20202020,       %r12         #Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
    movq        %r12,        %xmm13             #moving diff to SSE register
    pshufd      $0,          %xmm13,  %xmm13    #CHARACTER_DIFF

loop:
    movdqu      (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)
    add         $16,         %rax               # increament the pointer by 16 bytes
    add         $16,         %rsi               # increament the pointer by 16 bytes

    push %rax
    movq $2, %rax

    /* Converting str1 to Lower */
    pcmpestrm   $0x44,      %xmm10, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm10              # LOWER: TO_ADD + COPY --> xmm10

    /* Converting str2 to Lower */
    pcmpestrm   $0x44,      %xmm11, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm11              # LOWER: TO_ADD + COPY --> xmm11


    movq $16, %rax
    cmp %rdx, %rax
    jl skip
    movq %rdx, %rax
    skip:
    /* Check str1 and str2 are identical or not */
    pcmpestrm   $0x18,      %xmm10, %xmm11      # compare two sse register completely equal or not

    /* Restore the value of %rax from the stack */
    pop %rax
    jc         return_result_mismatch
    sub         $16,        %rdx
    jle         return_result_match
    jmp loop

return_result_mismatch:
    movq        $0x01,      %rax
    ret

return_result_match:
    movq        $0x00,      %rax
    ret
