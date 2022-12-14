/*
    2 x 64 bit sub
    \attention not sutiable for ordering/sorting texts
*/

.section .text
.global i_case_compare_v4                       # parameters are in *str1 = rdi,  *str2 = rsi, len = rdx
i_case_compare_v4:
    /* Prepare some constant */
    movq        $0x5A41,     %r11               # A: 0x41, Z: 0x5A --> defining Range
    movq        %r11,        %xmm12             # CHARACTER_RANGE

    movq        $0x20202020,       %r11         # Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
    movq        %r11,        %xmm13             # moving diff to SSE register
    pshufd      $0,          %xmm13,  %xmm13    # CHARACTER_DIFF

    cmp         $0,          %rdx
    jle         return_result_match

head_loop:
    sub         $16,        %rdx                # check if all characters are compared
    jle         prepare_explicit_length         # ensures that all characters were matched

    movdqu      (%rdi),     %xmm10              # move first 128 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)
    add         $16,         %rdi               # increament 128 pointer by 16 bytes
    add         $16,         %rsi               # increament the pointer by 16 bytes

    /* Converting str1 to Lower */
    pcmpistrm   $0x44,      %xmm10, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm10              # LOWER: TO_ADD + COPY --> xmm10

    /* Converting str2 to Lower */
    pcmpistrm   $0x44,      %xmm11, %xmm12      # MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
    pand        %xmm13,     %xmm0               # TO_ADD: CHARACTER_DIFF & MASK
    paddb       %xmm0,      %xmm11              # LOWER: TO_ADD + COPY --> xmm11



    /* Check str1 and str2 are identical or not */

    /* Firstly check lower 64 bit */
    movq        %xmm10,     %r10
    movq        %xmm11,     %r11
    sub         %r10,       %r11
    jnz         return_result_mismatch

    /* move high 64 bits to low 64 bits. */
    movhlps     %xmm10,     %xmm10
    movhlps     %xmm11,     %xmm11

    /* Now check upper 64 bit after mov */
    movq        %xmm10,     %r10
    movq        %xmm11,     %r11
    sub         %r10,       %r11
    jnz         return_result_mismatch
    jz          head_loop


prepare_explicit_length:
    add         $16,        %rdx
    jmp explicit_length_compare

explicit_length_compare:
    movdqu      (%rdi),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)


    movq $2, %rax                               # The length of %xmm12=0x5A41 is stored in %rdi
                                                # The length of %xmm10 is stored in %rdx, pcmpestrm is ready to execute

    pcmpestrm   $0x44,      %xmm10, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm10

    pcmpestrm   $0x44,      %xmm11, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm11


    cmp         $8,         %rdx
    jl part2
part1:
    /* Firstly check lower 64 bit */
    movq        %xmm10,     %r10
    movq        %xmm11,     %r11
    sub         %r10,       %r11
    jnz         return_result_mismatch
    sub $8, %rdx
    jz return_result_match
    /* move high 64 bits to low 64 bits. */
    movhlps     %xmm10,     %xmm10
    movhlps     %xmm11,     %xmm11

part2:

    movq        %xmm10,     %r10
    movq        %xmm11,     %r11

    /* Prepare count and shift logically left to handle garbage */
    movq        $8,         %rcx
    sub         %rdx,       %rcx
    shlq        $3,         %rcx

    shlq        %cl,        %r10
    shlq        %cl,        %r11
    sub         %r10,       %r11
    jnz         return_result_mismatch
    jz          return_result_match


return_result_mismatch:
    movq        $0x01,      %rax
    ret

return_result_match:
    movq        $0x00,      %rax
    ret

