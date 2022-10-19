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
        .global compare_string                          #*str1=rax, *str2=rsi, str1len=rcx, str2len=rdx
        .text
        compare_string:

        loop_label:
            cmp         $0x00,      %rcx                # check if all characters are compared
            je          return_match                    # ensures that all characters were matched
            sub         $8,         %rcx                # substruct by 8
            movq        (%rax),     %xmm10              # move first 64 bit of str1 to xmm10 register(SSE)
            movq        (%rsi),     %xmm11              # move first 64 bit of str2 to xmm11 register(SSE)
            add         $8,         %rax                # skip the pointer by 8 bytes
            add         $8,         %rsi                # skip the pointer by 8 bytes
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
    | Number of Characters  | Naive Approach | Assembly implementation | C++ Standard Library |
    | --------------------- | -------------- | ----------------------- | -------------------- |
    | 8 * 10000             | 459247         | 28927                   | 5130                 |
    | 8 * 100000            | 3807261        | 287960                  | 33832                |
    | 8 * 1000000           | 13491303       | 1048467                 | 626018               |