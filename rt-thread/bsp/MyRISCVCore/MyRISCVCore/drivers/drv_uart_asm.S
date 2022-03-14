	.global send_char
send_char:
    csrw 0x800, a0
    ret