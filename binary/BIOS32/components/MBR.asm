org 0x7C00

mov ax, cs
mov ds, ax
mov ss, ax
mov es, ax

cld
mov si, start_load_FAT32_code
mov di, 0x7E00
mov ax, end_load_FAT32_code - start_load_FAT32_code
copy_next_byte:
	cmp ax, 0
	jz jump_to_load_FAT32_sector
	movsb
	dec ax
	jmp copy_next_byte

jump_to_load_FAT32_sector:
	jmp 0x7E00


start_load_FAT32_code:

	mov ah, 42h
	mov dl, 0x80
	mov si, 0x7E00 + LBA_packet - start_load_FAT32_code
	int 0x13
	jc read_error
	jmp 0:0x7C00

	read_error:
		mov ax, 0xB800
		mov es, ax
		mov al, 1
		mov ah, 12
		mov bx, 0
		mov [es:bx], ax
		hlt
		jmp $


	LBA_packet: 
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 1
		buffer_address_offset  dw 0x7C00
		buffer_address_segment dw 0
		start_sector           dq 1

end_load_FAT32_code: