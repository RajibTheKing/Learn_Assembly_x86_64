## Let's share some interesting facts what I tried in this project
---

 - ### How can we check if two strings are completely equal?
    - A naive implementation can be: 
    ```c++ 
    int compareCharByChar(char *str1, char* str2, int str1len, int str2len)
    {
        //not efficient implemnentation which is just a Naive solution

        for(int i=0; i<str1len; i++)
        {
            if(str1[i] != str2[i])
                return 1; //mismatch found
        }

        return 0; // no mismatch
    }
    ```

    - A optimization is possible implementing similar function in Assembly layer
    ```assembly
        .global compare_string                          # parameters are in *str1 = rax,  *str2 = rsi, str1len = rcx, str2len = rdx
        .text
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
    ```

    - using c++ standard library
    ```c++
    int strcmp ( const char * str1, const char * str2 );
    ```
 - Here given the execution time (in Nanoseconds) among those 3 approaches:
    | Number of Characters  | Naive Approach | Assembly implementation | Assembly (Inline)    | C++ Standard Library |
    | --------------------- | -------------- | ----------------------- | -------------------- | -------------------- |
    | 8 * 10000             | 459247         | 14613                   | 14032                | 5130                 |
    | 8 * 100000            | 3807261        | 143541                  | 141726               | 97937                |
    | 8 * 1000000           | 13491303       | 616371                  | 616164               | 565875               |

 - ### How can we check if two strings are identical? (basically case insensitive compare)
    - A naive implementation can be:
    ```c++
    unsigned int compareCharByCharCaseInsensitive(unsigned char *str1, unsigned char* str2, unsigned int len)
    {
        int i=0, j=0;
        while(len)
        {
            int diff = tolower(str1[i]) - tolower(str2[i]);
            if(diff != 0)
                return diff; //mismatch found
            i++;
            j++;
            len--;
        }

        return 0; // no mismatch
    }
    ```
    - Or, we can make things faster using the help of SSE registers and check 16 characters parallelly. [N.B. It's a complete and working implementation]
    ```assembly
        .global compare_string_case_insensitive         # parameters are in *str1 = rax,  *str2 = rsi, len = rdx
        .text
        compare_string_case_insensitive:
            /*Prepare some constant*/
            movq        $0x5A41,     %r12               #A: 0x41, Z: 0x5A --> defining Range
            movq        %r12,        %xmm12             #CHARACTER_RANGE

            movq        $0x20202020,       %r12         #Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
            movq        %r12,        %xmm13             #moving diff to SSE register
            pshufd      $0,          %xmm13,  %xmm13    #CHARACTER_DIFF

        head_loop:
            cmp         $16,        %rdx                # check if all characters are compared
            jl          tail_loop                       # ensures that all characters were matched
            sub         $16,         %rdx               # substruct by 8
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
            cmp         $0x00,      %r8                 # check the output after pcmpistrm comparison
            je          head_loop
            jne         prepare_intermediate_mismatch


        prepare_intermediate_mismatch:
            sub         $16,         %rax
            sub         $16,         %rsi
            add         $16,         %rdx
            jmp tail_loop

        tail_loop:
            cmp $0, %rdx
            je return_result_match
            mov (%rax), %r13
            mov (%rsi), %r14
            sub $1, %rdx
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
            cmp $0, %r13b
            jne return_result_mismatch
            je tail_loop



        return_result_mismatch:
            movq        $0x00,      %rax
            mov %r13b, %al
            ret

        return_result_match:
            movq        $0x00,      %rax
            ret
    ```

 - Let's try to convert whole string to Lower case characters using SSE Instruction. It's a demonstration how we can convert 16 bytes characters to lower parallely.  
    | Field                 | 00    | 01    | 02    | 03    | 04    | 05    | 06    | 07    | 08    | 09    | 10    | 11    | 12    | 13    | 14    | 15    |
    | ---                   | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    | --    |
    | Input                 | O     | n     | c     | e     |       | U     | p     | o     | N     |       | A     |       | T     | i     | m     | e     |
    |  >=A                  | 0xFF  | 0xFF  | 0xFF  | 0xFF  | 0x00  | 0xFF  | 0xFF  | 0xFF  | 0xFF  | 0x00  | 0xFF  | 0x00  | 0xFF  | 0xFF  | 0xFF  | 0xFF  |
    |  <=Z                  | 0xFF  | 0x00  | 0x00  | 0x00  | 0xFF  | 0xFF  | 0x00  | 0x00  | 0xFF  | 0xFF  | 0xFF  | 0xFF  | 0xFF  | 0x00  | 0x00  | 0x00  |
    | mask: >=A && <=Z      | 0xFF  | 0x00  | 0x00  | 0x00  | 0x00  | 0xFF  | 0x00  | 0x00  | 0xFF  | 0x00  | 0xFF  | 0x00  | 0xFF  | 0x00  | 0x00  | 0x00  |
    | diff: 'a' - 'A'       | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  | 0x20  |
    | toAdd: mask & diff    | 0x20  | 0x00  | 0x00  | 0x00  | 0x00  | 0x20  | 0x00  | 0x00  | 0x20  | 0x00  | 0x20  | 0x00  | 0x20  | 0x00  | 0x00  | 0x00  |
    | Output: Input + toAdd | o     | n     | c     | e     |       | u     | p     | o     | n     |       | a     |       | t     | i     | m     | e     |

- Here given the execution time (in Nanoseconds) among those 3 approaches:
    | Number of Characters  | Naive Approach | Assembly implementation |
    | --------------------- | -------------- | ----------------------- |
    | 8 * 10000             | 777282         | 33008                   |
    | 8 * 100000            | 2573739        | 108532                  |
    | 8 * 1000000           | 8914861        | 377431                  |