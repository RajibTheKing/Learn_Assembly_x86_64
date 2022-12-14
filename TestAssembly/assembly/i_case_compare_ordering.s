/**
    pcmpistrm + pcmpestrm
    sutiable for ordering/sorting texts
*/

.section .text
.global ___i_case_compare_ordering                          /* parameters are in *str1 = rdi, *str2 = rsi, len = rdx */
___i_case_compare_ordering:
    /* prepare some constant */
    movq        $0x5A41,        %r11                        /* A: 0x41, Z: 0x5A --> defining Range */
    movq        %r11,           %xmm12                      /* CHARACTER_RANGE */

    movq        $0x20202020,                    %r11        /* diff = 'a' - 'A' = 32 = x020, prepare a doubleword value */
    movq        %r11,           %xmm13                      /* moving diff to SSE register */
    pshufd      $0,             %xmm13,         %xmm13      /* CHARACTER_DIFF */

    cmp         $0,             %rdx
    jle         return_result_match

head_loop:
    sub         $16,            %rdx                        /* check if all characters are compared */
    jle         prepare_explicit_length                     /* ensures that all characters were matched */

    movdqu      (%rdi),         %xmm10                      /* move 128 bit of str1 to xmm10 register(SSE) */
    movdqu      (%rsi),         %xmm11                      /* move 128 bit of str2 to xmm11 register(SSE) */
    add         $16,            %rdi                        /* increament the pointer by 16 bytes */
    add         $16,            %rsi                        # increament the pointer by 16 bytes

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
    jc          prepare_intermediate_mismatch

prepare_explicit_length:
    add         $16,            %rdx
    jmp         explicit_length_compare

explicit_length_compare:
    movdqu      (%rdi),         %xmm10                      /* move first 128 bit of str1 to xmm10 register(SSE) */
    movdqu      (%rsi),         %xmm11                      /* move first 128 bit of str2 to xmm11 register(SSE) */

    movq        $2,             %rax
    pcmpestrm   $0x44,          %xmm10,         %xmm12
    pand        %xmm13,         %xmm0
    paddb       %xmm0,          %xmm10

    pcmpestrm   $0x44,          %xmm11,         %xmm12
    pand        %xmm13,         %xmm0
    paddb       %xmm0,          %xmm11

    movq        %rdx,           %rax
    pcmpestrm   $0x18,          %xmm10,         %xmm11      /* compare two sse register completely equal or not */
    jnc         return_result_match
    jc          tail_loop

prepare_intermediate_mismatch:
    sub         $16,            %rdi
    sub         $16,            %rsi
    add         $16,            %rdx
    jmp         tail_loop

tail_loop:
    movb        (%rdi),         %r13b
    movb        (%rsi),         %r14b
    add         $1,             %rdi
    add         $1,             %rsi

    /* convert single byte from str1 to lower */
str1:
    cmp         $0x41,          %r13b
    jl          str2
    cmp         $0x5A,          %r13b
    jg          str2
    add         $32,            %r13b

    /* convert single byte from str2 to lower */
str2:
    cmp         $0x41,          %r14b
    jl          compare
    cmp         $0x5A,          %r14b
    jg          compare
    add         $32,            %r14b

compare:
    sub         %r14b,          %r13b
    jnz         return_result_mismatch
    sub         $1,             %rdx
    jnz         tail_loop
    jz          return_result_match

return_result_mismatch:
    movq        $0x00,          %rax
    mov         %r13b,          %al
    ret

return_result_match:
    movq        $0x00,          %rax
    ret

