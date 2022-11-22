/*
pcmpistrm + pcmpestrm
*/

.section .text
.global i_case_compare_v2                          # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
i_case_compare_v2:
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
    add         $16,         %rax               # increament the pointer by 16 bytes
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
    pcmpistrm   $0x18,      %xmm10, %xmm11      # compare two sse register completely equal or not
    jnc         head_loop
    jc          return_result_mismatch

prepare_explicit_length:
    add         $16,        %rdx
    jmp explicit_length_compare

explicit_length_compare:
    movdqu      (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
    movdqu      (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)


    push %rax                                   # Saving %rax in the Stack, because %rax,%rdx register is used for exlplicit length compare
    movq $2, %rax                               # The length of %xmm12=0x5A41 is stored in %rax
                                                # The length of %xmm10 is stored in %rdx, pcmpestrm is ready to execute

    pcmpestrm   $0x44,      %xmm10, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm10

    pcmpestrm   $0x44,      %xmm11, %xmm12
    pand        %xmm13,     %xmm0
    paddb       %xmm0,      %xmm11

    movq %rdx, %rax
    pcmpestrm   $0x18,      %xmm10, %xmm11      # compare two sse register completely equal or not
    /* Restore the value of %rax from the stack */
    pop %rax

    jnc         return_result_match
    jc          return_result_mismatch

return_result_mismatch:
    movq        $0x01,      %rax
    ret

return_result_match:
    movq        $0x00,      %rax
    ret

/*
To compare two <=16 bytes parallely there can be two ways
    - pcmpistrm/pcmpestrm (16 byte comparisons, can be implicit or explicit length)
    - subps (128 bit subtraction, can be done between two %xmm/%ymm registers)

After research, I found several reasons why subps instruction cannot be feasible to use
    - SUBPS... it doesn't set the Condition FLAG, After operation %xmm register need to be checked manually
    - there is no standard instruction available that checks %xmm/%ymm register is empty/zero
            source: https://stackoverflow.com/questions/42317528/faster-way-to-test-if-xmm-ymm-register-is-zero
    - SUBPS... while explicit length, bytes might need to be shifted logically left/right within SSE registers.
        pslldq only works with immediate values.. which means... 
        pslldq    $13,   %xmm10     ///< this statement is valid
        pslldq    %xmm0, %xmm10     ///< this statement is invalid
            source: https://stackoverflow.com/questions/74435943/how-to-perform-logical-shiftpslldq-within-128-bit-sse-registers-in-gas-assembl
*/
