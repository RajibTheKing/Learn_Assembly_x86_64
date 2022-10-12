section .text
global getsum_x86_64    ; parameters are in rcx and rdx
getsum_x86_64:
    mov rax, [rcx]  ; store value of rcx into rax
    add rax, [rdx]  ; add value of rdx with rax
	ret