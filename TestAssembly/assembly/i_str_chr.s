.section .text
.global ___i_str_chr                                        /* parameters are in *str = rdi, target = rsi */
___i_str_chr:
    /* prepare some constant variables */
    movq        $0,             %rdx
    movq        $0x5A41,        %r11                        /* A: 0x41, Z: 0x5A --> defining Range */
    movq        %r11,           %xmm12                      /* CHARACTER_RANGE */

    movq        $0x20202020,                    %r11        /* diff = 'a' - 'A' = 32 = x020, prepare a doubleword value */
    movq        %r11,           %xmm13                      /* moving diff to SSE register */
    pshufd      $0,             %xmm13,         %xmm13      /* CHARACTER_DIFF */

    /* set toLower(target) in each byte of xmm1 */
    movq        %rsi,           %r11                        /* load param target = rsi to a register %r11 temporarily */
    cmp         $0x41,          %r11b
    jl          lower_converted
    cmp         $0x5A,          %r11b
    jg          lower_converted
    add         $32,            %r11b
lower_converted:
    movq        %r11,           %xmm1
    movq        %r11,           %xmm2
    pslldq      $1,             %xmm1
    por         %xmm2,          %xmm1
    pslldq      $1,             %xmm1
    por         %xmm2,          %xmm1
    pslldq      $1,             %xmm1
    por         %xmm2,          %xmm1
    pshufd      $0,             %xmm1,          %xmm1

head_loop:
    movdqu      (%rdi),         %xmm4
    add         $16,            %rdi
    add         $16,            %rdx

    /* converting str to Lower */
    pcmpistrm   $0x44,          %xmm4,         %xmm12       /* MASK: compare characters is in range A <= c <= Z and store result bytes in xmm0 */
    pand        %xmm13,         %xmm0                       /* TO_ADD: CHARACTER_DIFF & MASK */
    paddb       %xmm0,          %xmm4                       /* LOWER: TO_ADD + COPY --> xmm4 */

    /* firstly check if target exists in current 16 byte block */
    pxor        %xmm3,          %xmm3
    por         %xmm1,          %xmm3
    pcmpeqb     %xmm4,          %xmm3
    pmovmskb    %xmm3,          %eax
    test        %eax,           %eax
    jne         return_found

    /* also check null terminating chracter */
    pxor        %xmm3,          %xmm3
    pcmpeqb     %xmm4,          %xmm3
    pmovmskb    %xmm3,          %eax
    test        %eax,           %eax
    jz          head_loop
    jmp         return_not_found

return_not_found:
    movq        $0x00,          %rax
    ret

return_found:
    sub         $16,            %rdx
    sub         $16,            %rdi
    tzcnt       %eax,           %eax
    add         %rax,           %rdi
    add         %edx,           %eax
    movq        %rdi,           %rax
    ret

