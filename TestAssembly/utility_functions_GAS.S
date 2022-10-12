.global getsum_x86_64_GAS    # parameters are in rcx and rdx
.text
getsum_x86_64_GAS:
    mov (%rcx), %rax  # store value of rcx into rax
    add (%rdx), %rax  # add value of rdx with rax
    add $5, %rax      # add extra 5 rax only in GAS to differentiate
	ret