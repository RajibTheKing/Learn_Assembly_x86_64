/**
    pcmpistrm + pcmpestrm
    \attention not sutiable for ordering/sorting texts
*/

.section .text
.global ___i_case_compare                                   /* parameters are in *str1 = rdi, *str2 = rsi, len = rdx */
___i_case_compare:
    /* prepare some constant */
    movq        $0x5A41,        %r12                        /* A: 0x41, Z: 0x5A --> defining Range */
    movq        %r12,           %xmm12                      /* CHARACTER_RANGE */

    movq        $0x20202020,    %r12                        /* diff = 'a' - 'A' = 32 = x020, prepare a doubleword value */
    movq        %r12,           %xmm13                      /* moving diff to SSE register */
    pshufd      $0,             %xmm13,         %xmm13      /* CHARACTER_DIFF */

    cmp         $0,             %rdx
    jle         return_result_match

head_loop:
    sub         $16,            %rdx                        /* check if all characters are compared */
    jle         prepare_explicit_length                     /* ensures that all characters were matched */

    movdqu      (%rdi),         %xmm10                      /* move 128 bit of str1 to xmm10 register(SSE) */
    movdqu      (%rsi),         %xmm11                      /* move 128 bit of str2 to xmm11 register(SSE) */
    add         $16,            %rdi                        /* increament the pointer by 16 bytes */
    add         $16,            %rsi                        /* increament the pointer by 16 bytes */

    /* converting str1 to Lower */
    pcmpistrm   $0x44,          %xmm10,         %xmm12      /* MASK: compare characters is in range A <= c <= Z and store result bytes in xmm0 */
    pand        %xmm13,         %xmm0                       /* TO_ADD: CHARACTER_DIFF & MASK */
    paddb       %xmm0,          %xmm10                      /* LOWER: TO_ADD + COPY --> xmm10 */

    /* converting str2 to Lower */
    pcmpistrm   $0x44,          %xmm11,         %xmm12      /* MASK: compare characters is in range A <= c <= Z and store result bytes in xmm0 */
    pand        %xmm13,         %xmm0                       /* TO_ADD: CHARACTER_DIFF & MASK */
    paddb       %xmm0,          %xmm11                      /* LOWER: TO_ADD + COPY --> xmm11 */

    /* check str1 and str2 are identical or not */
    pcmpistrm   $0x18,          %xmm10,         %xmm11      /* compare two sse register completely equal or not */
    jnc         head_loop
    jc          return_result_mismatch

prepare_explicit_length:
    add         $16,            %rdx
    jmp         explicit_length_compare

explicit_length_compare:
    movdqu      (%rdi),         %xmm10                      /* move 128 bit of str1 to xmm10 register(SSE) */
    movdqu      (%rsi),         %xmm11                      /* move 128 bit of str2 to xmm11 register(SSE) */

    movq        $2,             %rax                        /* length of %xmm12=0x5A41 is stored in %rdi and length of %xmm10 is stored in %rdx, pcmpestrm is ready to execute */
    pcmpestrm   $0x44,          %xmm10,         %xmm12
    pand        %xmm13,         %xmm0
    paddb       %xmm0,          %xmm10

    pcmpestrm   $0x44,          %xmm11,         %xmm12
    pand        %xmm13,         %xmm0
    paddb       %xmm0,          %xmm11

    movq        %rdx,           %rax
    pcmpestrm   $0x18,          %xmm10,         %xmm11      /* compare two sse register completely equal or not */

    jnc         return_result_match
    jc          return_result_mismatch

return_result_mismatch:
    movq        $0x01,          %rax
    ret

return_result_match:
    movq        $0x00,          %rax
    ret
