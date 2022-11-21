/*
2 x 64 bit sub
*/

.section .data
S:
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
value:
    .long 0x200

.section .text
.global i_case_compare_v4                          # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
i_case_compare_v4:
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
    add         $16,         %rax               # skip the pointer by 16 bytes
    add         $16,         %rsi               # skip the pointer by 16 bytes

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
    movq        %xmm10,     %r12
    movq        %xmm11,     %r13
    sub         %r12,       %r13
    jnz         return_result_mismatch

    /* move high 64 bits to low 64 bits. */
    movhlps     %xmm10,     %xmm10
    movhlps     %xmm11,     %xmm11

    /* Now check upper 64 bit after mov */
    movq        %xmm10,     %r12
    movq        %xmm11,     %r13
    sub         %r12,       %r13
    jnz         return_result_mismatch
    jz          head_loop


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

    /* Restore the value of %rax from the stack */
    pop %rax

    cmp         $8,         %rdx
    jl part2
part1:
    /* Firstly check lower 64 bit */
    movq        %xmm10,     %r12
    movq        %xmm11,     %r13
    sub         %r12,       %r13
    jnz         return_result_mismatch
    sub $8, %rdx
    jz return_result_match
    /* move high 64 bits to low 64 bits. */
    movhlps     %xmm10,     %xmm10
    movhlps     %xmm11,     %xmm11

part2:
    cmp $1, %rdx
    jz shift_7;
    cmp $2, %rdx
    jz shift_6;
    cmp $3, %rdx
    jz shift_5;
    cmp $4, %rdx
    jz shift_4;
    cmp $5, %rdx
    jz shift_3;
    cmp $6, %rdx
    jz shift_2;
    cmp $7, %rdx
    jz shift_1;

shift_0:
    pslldq     $0,   %xmm10
    pslldq     $0,   %xmm11
    jmp calculation
shift_1:
    pslldq     $1,   %xmm10
    pslldq     $1,   %xmm11
    jmp calculation
shift_2:
    pslldq     $2,   %xmm10
    pslldq     $2,   %xmm11
    jmp calculation
shift_3:
    pslldq     $3,   %xmm10
    pslldq     $3,   %xmm11
    jmp calculation
shift_4:
    pslldq     $4,   %xmm10
    pslldq     $4,   %xmm11
    jmp calculation
shift_5:
    pslldq     $5,   %xmm10
    pslldq     $5,   %xmm11
    jmp calculation
shift_6:
    pslldq     $6,   %xmm10
    pslldq     $6,   %xmm11
    jmp calculation
shift_7:
    pslldq     $7,   %xmm10
    pslldq     $7,   %xmm11
    jmp calculation

calculation:
    /* Now check upper 64 bit after mov */
    movq        %xmm10,     %r12
    movq        %xmm11,     %r13
    sub         %r12,       %r13
    jnz         return_result_mismatch
    jz          return_result_match




    /*Some Testing Code to use pslldq using constant data Array*/
#   values(, %edi, 4)
#    movl        value,          %ecx
#    movb        $3,             %r15b
#    mov        S,  %r14b
#    movb $S, %r15b
#    pslldq      S(, %edi, 1),   %xmm10


return_result_mismatch:
    movq        $0x01,      %rax
    ret

return_result_match:
    movq        $0x00,      %rax
    ret

