; boot.asm
[BITS 16]          ; Set the bitness to 16 for boot sector
[ORG 0x7C00]      ; Bootloader starts at memory location 0x7C00

start:
    ; Clear screen
    mov ax, 0xB800
    mov es, ax
    xor di, di
    mov cx, 80 * 25  ; 80 columns * 25 rows
    mov ax, 0x0F00   ; Light grey on black
    rep stosw        ; Fill the screen with the attribute byte

    ; Print a string
    mov si, msg
    call print_string

    ; Hang the system
hang:
    jmp hang

print_string:
    mov ah, 0x0E
.next_char:
    lodsb            ; Load byte at DS:SI into AL
    cmp al, 0
    je .done         ; If null terminator, exit
    int 0x10        ; BIOS interrupt to print character
    jmp .next_char
.done:
    ret

msg db 'Hello, World!', 0

times 510 - ($ - $$) db 0  ; Fill the rest of the sector with zeros
dw 0xAA55                  ; Boot signature

