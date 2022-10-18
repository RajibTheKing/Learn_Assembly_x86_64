.global getsum_x86_64_GAS                   # parameters are in rcx and rdx
.text
getsum_x86_64_GAS:
    mov         (%rcx),     %rax            # store value of rcx into rax
    add         (%rdx),     %rax            # add value of rdx with rax
    ret


.global packed_suturation_test
.text
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



.global search_substr                        # parameters are in rcx and rdx
.text
search_substr:
    movq        (%rcx),     %xmm9           # move first 64 bit to xmm9 register(SSE)
    add         $8,         %rcx            # rcx is pointer of inData
                                            # this instruction will skip first 8 byte of inData
    movq        (%rcx),     %xmm11          # move second 64 bit to xmm11 register(SSE), xmm11 is used as temp storage
    pslldq      $8,         %xmm11          # shift left logical by 8 bytes = 64bit(moving data from lowerbits 64bit to upper 64bit
    por         %xmm11,     %xmm9

    movq        (%rdx),     %xmm10           # move first 64 bit to xmm9 register(SSE)

    pcmpistrm  $0x4C,  %xmm9,     %xmm10

    movq %xmm0, %rax
    ret

