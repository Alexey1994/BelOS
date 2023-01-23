%define FAT               0x0500
%define FAT_data          0x0700
%define kernel_address    0x8000

;локальные переменные
%define data_destination              4
%define start_sector_of_FAT           8
%define start_sector_of_data          12
%define current_cluster               16
%define current_cluster_offset        20
%define current_sector_of_FAT         24

;переменные заголовка FAT
%define sectors_in_cluster             13
%define number_of_reserved_sectors     14
%define number_of_FAT_tables           16
%define number_of_hidden_sectors       28
%define number_of_sectors_in_FAT_table 36
%define root_directory_cluster         44
%define drive_number                   64

;переменные файлового заголовка FAT
%define FAT_data_cluster_high 20
%define FAT_data_cluster_low  26


org 0x7C5A

	mov AX, CS
	mov DS, AX
	mov SS, AX
	mov ES, AX
	mov SP, 0x7C00 - current_sector_of_FAT
	mov BP, 0x7C00


	xor EAX, EAX
	mov AX, [BP + number_of_reserved_sectors]
	mov EBX, [BP + number_of_hidden_sectors]
	add EBX, EAX
	mov [BP - start_sector_of_FAT], EBX
	;start_sector_of_FAT = number_of_hidden_sectors + number_of_reserved_sectors

	xor EAX, EAX
	mov AL, [BP + number_of_FAT_tables]
	mul dword [BP + number_of_sectors_in_FAT_table]
	add EAX, EBX
	mov [BP - start_sector_of_data], EAX
	;start_sector_of_data = start_sector_of_FAT + number_of_FAT_tables * number_of_sectors_in_FAT_table

	mov EAX, [BP + root_directory_cluster]
	mov [BP - current_cluster], EAX
	;current_cluster = root_directory_cluster

	;mov word [BP - current_cluster_offset], 0
	;current_cluster_offset = 0
	
	mov dword [BP - current_sector_of_FAT], 0
	;current_sector_of_FAT = 0


enum_next_data:
	call read_FAT

	mov word [BP - data_destination], FAT_data
	call read_data

	mov DX, 512 / 32
	mov BX, FAT_data
next_data:
	cmp byte [BX], 0
	je kernel_not_found

	mov CX, 11
	mov SI, BX
	mov DI, kernel_name
	rep cmpsb
	je kernel_found

	add BX, 32
	dec DX
	cmp DX, 0
	jne next_data

	call read_next_cluster
	cmp AX, 0
	je kernel_not_found
	
	jmp enum_next_data


kernel_found:
	;in BX - adress of FAT_Data

	mov AX, [BX + FAT_data_cluster_high]
	shl EAX, 16
	mov AX, [BX + FAT_data_cluster_low]
	mov [BP - current_cluster], EAX
	
	mov word [BP - data_destination], kernel_address
	load_next_kernel_sector:
	call read_FAT
	call read_data
	
	call read_next_cluster
	cmp AX, 0
	je start_kernel
	
	add word [BP - data_destination], 512
	jmp load_next_kernel_sector


start_kernel:
	jmp kernel_address


kernel_not_found:
	mov SI, kernel_name
	jmp handle_error

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;in [BP - data_destination]
read_data:
	mov EAX, [BP - current_cluster]
	sub EAX, 2
	mul byte [BP + sectors_in_cluster]
	add EAX, [BP - start_sector_of_data]
	;add EAX, [BP - current_cluster_offset]
	mov [start_sector], EAX
	;start_sector = start_sector_of_data + (cluster_number - 2) * sectors_in_cluster + offset
	
	mov BX, [BP - data_destination]
	mov word [buffer_address_offset], BX ;FAT_data
	call read_sector
	
	;inc word [BP - current_cluster_offset]
	;mov AX, [BP - current_cluster_offset]
	;cmp [BP + sectors_in_cluster], AL
	;jne skip_clean_current_cluster_offset
	;mov word [BP - current_cluster_offset], 0
;skip_clean_current_cluster_offset:
	
	ret


read_next_cluster:
	mov EBX, [BP - current_cluster]
	and BX, 0b1111111
	shl BX, 2
	;BX = current_cluster % 128 / 4

	add BX, FAT
	mov EAX, [BX]
	cmp EAX, 0x0FFFFFFF
	je no_next_cluster
	mov [BP - current_cluster], EAX

	;call read_FAT
	mov AX, 1
	ret

no_next_cluster:
	xor AX, AX
	ret


read_FAT:
	mov EAX, [BP - current_cluster]
	mov EBX, EAX
	shr EAX, 7
	add EAX, [BP - start_sector_of_FAT]
	mov [start_sector], EAX
	;start_sector = current_cluster / 128 + start_sector_of_FAT

	cmp EAX, [BP - current_sector_of_FAT]
	je skip_read_FAT
	;start_sector == current_sector_of_FAT => skip_read_FAT
	mov [BP - current_sector_of_FAT], EAX
	;current_sector_of_FAT = start_sector

	mov word [buffer_address_offset], FAT
	call read_sector

skip_read_FAT:
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;in  SI - адрес перечислителя
read_sector:
	mov AH, 42h
	mov DL, [BP + drive_number]
	mov SI, LBA_packet
	int 13h
	jc read_error
	ret

	LBA_packet:
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 1
		buffer_address_offset  dw 0
		buffer_address_segment dw 0
		start_sector           dq 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


read_error:
	mov SI, read_error_message
handle_error:
	call print_string
	
	int 16h
	int 19h


;in  SI - printed string
print_string:
	mov AH, 0x0E
print_next_string_char:
	lodsb
	cmp AL, 0
	je end_print_string
	int 0x10
	jmp print_next_string_char
end_print_string:
	ret


kernel_name:db "KERNEL     "
kernel_not_found_message: db "not found", 0
read_error_message: db "Read error", 0