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
        ///< Assembly Implementation with SSE instructions
        int result;

        ASM(    ///< Prepare constant CHARACTER_RANGE
                movq        $0x5A41,        %%r12;                  // A: 0x41, Z: 0x5A --> defining Range
                movq        %%r12,          %%xmm12;                // CHARACTER_RANGE: xmm12, move quad word(64 bit) r12 to xmm12 to perform SSE instructions

                ///< Prepare constant CHARACTER_DIFF
                movq        $0x20202020,    %%r12;                  // Diff = 'a' - 'A' = 32 = x020, Prepare a doubleword value
                movq        %%r12,          %%xmm13;                // moving diff to SSE register
                pshufd      $0,             %%xmm13,    %%xmm13;    // CHARACTER_DIFF: xmm13, double word (32 bit) will be broadcasts to 128 bit like 0x20202020 20202020 20202020 20202020

            head_loop:
                cmp         $16,            %3;                     // check if all characters are compared
                jl          tail_loop;                              // all characters were matched so far. i.e. len = 37, 32 bytes are checked inside head_loop, 5 bytes will be checked inside tail_loop
                sub         $16,            %3;                     // substruct len 16, meaning 16 bytes are going to be checked inside this head_loop
                movdqu      (%1),           %%xmm10;                // move double quad word (128 bit) of lhs to xmm10 register(SSE)
                movdqu      (%2),           %%xmm11;                // move double quad word (128 bit) of rhs to xmm11 register(SSE)
                add         $16,            %1;                     // skip the pointer by 16 bytes
                add         $16,            %2;                     // skip the pointer by 16 bytes

                ///< Converting lhs to Lower
                pcmpistrm   $0x44,          %%xmm10,    %%xmm12;    // MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
                pand        %%xmm13,        %%xmm0;                 // TO_ADD: CHARACTER_DIFF & MASK
                movdqu      %%xmm10,        %%xmm14;                // COPY: xmm10 --> xmm14
                paddb       %%xmm0,         %%xmm14;                // LOWER: TO_ADD + COPY --> xmm14

                ///< Converting rhs to Lower
                pcmpistrm   $0x44,          %%xmm11,    %%xmm12;    // MASK: Compare characters is in range A <= c <= Z and store result bytes in xmm0
                pand        %%xmm13,        %%xmm0;                 // TO_ADD: CHARACTER_DIFF & MASK
                movdqu      %%xmm11,        %%xmm15;                // COPY: xmm11 --> xmm15
                paddb       %%xmm0,         %%xmm15;                // LOWER: TO_ADD + COPY --> xmm15


                ///< Check lhs and rhs are identical or not
                pcmpistrm   $0x18,          %%xmm14,    %%xmm15;    // compare two sse register completely equal or not
                movq        %%xmm0,         %%r8;                   // move the result of xmm0 register to r8 register (temp) to perform cmp instruction
                cmp         $0x00,          %%r8;                   // check the output after pcmpistrm compare
                je          head_loop;
                jne         prepare_intermediate_mismatch;          // Here we already found a mismatch inside head_loop, need to check one by one again to return the result


            prepare_intermediate_mismatch:
                sub         $16,            %1;
                sub         $16,            %2;
                add         $16,            %3;
                jmp tail_loop;

            tail_loop:
                cmp         $0,             %3;
                je return_result_match;
                movq        (%1),           %%r13;
                movq        (%2),           %%r14;
                sub         $1,             %3;
                add         $1,             %1;
                add         $1,             %2;

                ///< convert single byte from str1 to lower
            str1:
                cmp         $0x41,          %%r13b;
                jl str2;
                cmp         $0x5A,          %%r13b;
                jg str2;
                add         $32,            %%r13b;

                ///< convert single byte from str2 to lower
            str2:
                cmp         $0x41,          %%r14b;
                jl compare;
                cmp         $0x5A,          %%r14b;
                jg compare;
                add         $32,            %%r14b;

            compare:
                sub         %%r14b,         %%r13b;
                cmp         $0,             %%r13b;
                jne return_result_mismatch;
                je tail_loop;

            return_result_mismatch:
                and         $0xFF,          %%r13;
                movl        %%r13d,         %0;
                jmp end_func;

            return_result_match:
                movl        $0,             %0;
            end_func:
            )
            : ///< output operands
                "=r" (result)                                       // %0
            : ///< input operands
                "r" (lhs),                                          // %1
                "r" (rhs),                                          // %2
                "r" (len)                                           // %3
            : ///< clobber
                "%xmm0", "%xmm10", "%xmm11", "%xmm12", "%xmm13","%xmm14", "%xmm15", "%r8", "%r12", "%r14", "%r13", "memory"

        );
        ///< Signed byte Value Range: -128...0....+127, following statement is going to convert unsigned value to signed value
        return result > 127 ? result - 256 : result;
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

- Here given the execution time (in Nanoseconds) among different assembly approaches:
    | Number of Characters | C++ Naive Solution | pcmpistrm + pcmpestrm + ordering | pcmpistrm + pcmpestrm | pcmpestrm | 2 x 64 bit sub |
    | ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
    | 10012 |33796 |1393 |1406 |2730 |1419 |
    | 60004 |185529 |7865 |9200 |16089 |8032 |
    | 110005 |339536 |14397 |15821 |29392 |14758 |
    | 160009 |499373 |23215 |56133 |46707 |23379 |
    | 210000 |647952 |29041 |30562 |59862 |29969 |
    | 260009 |835732 |35452 |35443 |70903 |36820 |
    | 310009 |987607 |41358 |41357 |86030 |43114 |
    | 360011 |1162616 |52462 |52499 |109473 |51417 |
    | 410010 |1473949 |62397 |59793 |125513 |59804 |
    | 460001 |1537261 |65315 |85971 |197387 |73234 |
    | 510003 |1853757 |78217 |74661 |156109 |74402 |
    | 560005 |1831387 |80941 |78069 |156001 |84166 |
    | 610002 |2037613 |87543 |85591 |167310 |84165 |
    | 660013 |2144812 |92327 |100186 |197296 |89992 |
    | 710004 |2340992 |121609 |107594 |207726 |98957 |
    | 760004 |2551621 |108839 |105926 |211882 |105911 |
    | 810000 |2704179 |113157 |112933 |225635 |112893 |
    | 860002 |2823383 |120339 |120186 |239600 |119863 |
    | 910015 |3001616 |126800 |127125 |253511 |126869 |
    | 960012 |3179601 |157050 |158309 |301372 |155960 |
    | 1010004 |3316689 |164511 |141413 |290171 |154164 |
    | 1060008 |3488996 |149176 |147328 |295276 |147700 |
    | 1110006 |3672442 |158302 |154639 |309165 |154745 |
    | 1160008 |3793725 |161633 |161592 |323084 |161652 |
    | 1210001 |3982512 |168758 |218927 |356958 |169269 |
    | 1260000 |4150492 |186602 |175521 |350909 |175647 |
    | 1310012 |4302909 |184822 |183908 |384107 |185452 |
    | 1360005 |4483147 |204617 |210776 |397734 |189508 |
    | 1410000 |4635331 |207706 |192094 |403353 |192176 |
    | 1460003 |4798553 |203407 |203350 |426923 |203408 |
    | 1510003 |4963367 |221901 |210279 |441570 |210360 |
    | 1560000 |5138797 |224787 |217274 |456190 |217331 |
    | 1610000 |5278028 |226857 |224237 |448376 |224300 |
    | 1660003 |5488933 |231386 |229108 |475035 |226311 |
    | 1710002 |5619473 |240909 |238196 |476203 |238253 |
    | 1760014 |5787385 |245532 |258086 |514473 |313864 |
    | 1810015 |5959917 |267851 |252164 |544604 |252172 |
    | 1860002 |6145780 |259686 |259022 |518017 |270711 |
    | 1910000 |6526869 |275974 |266308 |578520 |266056 |
    | 1960014 |6463486 |284717 |272976 |558730 |273049 |
    | 2010005 |6641132 |280152 |279926 |559762 |280004 |
    | 2060012 |6817613 |286884 |286929 |602366 |286949 |
    | 2110007 |6966120 |293915 |294066 |617188 |294444 |
    | 2160015 |7138652 |301681 |300851 |601539 |300917 |
    | 2210006 |7274705 |301190 |301083 |629874 |314868 |
    | 2260014 |7501311 |320556 |307903 |615697 |313161 |
    | 2310013 |7685989 |321739 |322350 |660751 |317634 |
    | 2360005 |7770792 |330954 |328669 |688742 |328728 |
    | 2410001 |7928731 |336132 |335625 |695281 |335699 |
    | 2460014 |8157837 |343524 |342594 |685049 |342648 |
    | 2510012 |8270194 |349754 |349552 |699439 |349717 |
    | 2560012 |8439882 |357735 |356540 |712869 |357027 |
    | 2610001 |8680644 |406899 |364682 |726805 |363113 |
    | 2660004 |8804007 |365836 |363909 |737748 |370467 |
    | 2710011 |8963003 |378562 |378336 |792390 |377467 |
    | 2760000 |9084995 |385505 |382627 |773245 |381660 |
    | 2810008 |9261239 |393922 |391588 |784061 |391389 |
    | 2860002 |9475947 |406257 |404477 |836225 |396017 |
    | 2910000 |9646684 |444438 |437118 |842993 |405870 |
    | 2960001 |9787096 |412447 |412297 |841528 |412310 |
    | 3010013 |9942297 |420224 |419250 |836211 |410182 |
    | 3060015 |10049696 |426437 |426091 |896052 |426188 |
    | 3110003 |10369715 |489876 |458528 |906350 |443657 |
    | 3160002 |10482601 |441341 |441679 |879964 |440192 |
    | 3210000 |10696552 |501549 |454141 |905502 |447229 |
    | 3260008 |10852094 |511479 |456064 |953141 |454028 |
    | 3310006 |11053594 |530952 |483764 |921740 |461069 |
    | 3360015 |11107935 |553648 |469297 |1092607 |486220 |
    | 3410011 |11341840 |547220 |475750 |949539 |475122 |
    | 3460014 |11520446 |548760 |499101 |974943 |530443 |
    | 3510006 |11753944 |505405 |490094 |1001860 |484771 |
    | 3560002 |11837658 |513891 |496400 |1032765 |495944 |
    | 3610008 |11939241 |542141 |502894 |1012094 |533662 |
    | 3660015 |12375200 |528114 |509858 |1123331 |519701 |
    | 3710004 |12276424 |583759 |517339 |1033080 |516851 |
    | 3760000 |12464007 |529055 |524506 |1099356 |524433 |
    | 3810005 |12606176 |537286 |533143 |1060972 |528430 |
    | 3860012 |12784624 |540748 |538953 |1130108 |537740 |
    | 3910011 |12933875 |555269 |544924 |1088757 |544754 |
    | 3960004 |13088446 |559522 |552681 |1160747 |546561 |
    | 4010006 |13186449 |588906 |559389 |1172538 |552040 |
    | 4060007 |13474393 |583911 |571995 |1187071 |565678 |
    | 4110004 |13631193 |652104 |594714 |1212909 |578318 |
    | 4160005 |13790760 |604852 |580633 |1158496 |601362 |
    | 4210004 |13885494 |633091 |586879 |1230947 |588249 |
    | 4260007 |14108083 |595449 |616033 |1181062 |580567 |
    | 4310003 |14262835 |612354 |606514 |1296533 |600583 |
    | 4360003 |14562963 |639776 |608402 |1284084 |601803 |
    | 4410003 |14593409 |645919 |759906 |1274541 |634252 |
    | 4460004 |14748460 |671399 |625049 |1243520 |621784 |
    | 4510003 |14978415 |636890 |631765 |1320337 |629388 |
    | 4560010 |15020727 |662683 |638605 |1333516 |635545 |
    | 4610001 |15169938 |655568 |644576 |1320385 |629259 |
    | 4660008 |15401668 |666975 |664754 |1347813 |635961 |
    | 4710006 |15521919 |674601 |658763 |1311926 |644162 |
    | 4760005 |15654182 |669724 |666564 |1392830 |656747 |
    | 4810012 |15909193 |676190 |673159 |1341534 |663415 |
    | 4860011 |16023863 |690975 |679057 |1354484 |693097 |
    | 4910004 |16256061 |719711 |711726 |1436185 |689894 |
    | 4960005 |16425828 |705967 |705029 |1408814 |677117 |

  
- Here showing the graph using Naive and pcmpistrm + pcmpestrm + ordering
![alt text](../images/Graph_Execution_Time.png)

- Here showing the graph with all approaches
![alt text](../images/Graph_Execution_Time_17112022.png)

- Here showing the graph with all approaches except Naive Solution
![alt text](../images/Graph_Execution_time_without_Naive_17112022.png)
