%define loader_enumerator 0x0500

;перечислитель
%define FAT                      0    ;512
%define FAT_data                 512  ;512
%define current_cluster          1024 ;4
%define number_of_clusters       1028 ;4
%define number_of_sectors_in_FAT 1032 ;4
%define current_data_offset      1036 ;2
%define drive_number             1038 ;1


org 0x7C00

mov AX, CS
mov DS, AX
mov SS, AX
mov ES, AX
mov SP, 0x7C00
;mov BP, 0x7C00 + 512

;mov DL, 0x80
mov SI, loader_enumerator
call open_root

find_kernel:
	mov SI, loader_enumerator
	call enum_data

	cmp DI, 0
	je missing_kernel_error

	mov SI, kernel_name
	mov CX, 7
	repe cmpsb
	jne find_kernel

mov  SI, loader_enumerator
mov  EDI, 0x8000
call load_data

xor AX, AX
mov AL, [loader_enumerator + drive_number]
push AX
push loader_enumerator
push open_root
push enum_data
push load_data
jmp 0x8000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;in  DL - номер диска
;in  SI - адрес перечислителя
open_root:
	mov [SI + drive_number], DL
	mov word [SI + current_data_offset], 0

	xor EAX, EAX
	mov [start_sector], EAX
	mov [buffer_address_offset], SI
	call read_sector
	
	mov EAX, [SI + 512 - 12]
	mov [SI + number_of_clusters], EAX
	
	mov EAX, [SI + 512 - 16]
	mov [SI + number_of_sectors_in_FAT], EAX
	
	xor EAX, EAX
	mov [SI + current_cluster], EAX
	call read_FAT

	ret


;in  SI - адрес перечислителя
;out DI - адрес метаданных
;out EAX - кластер данных
enum_data:
	mov AX, [SI + current_data_offset]
	and AX, 0b111111111
	cmp AX, 0
	jne skip_read_next_enum_data

	call read_data
	mov word [SI + current_data_offset], 0

	skip_read_next_enum_data:
	mov DI, SI
	add DI, FAT_data
	add DI, [SI + current_data_offset]
	
	mov EAX, [DI]

	cmp EAX, 0
	je no_more_data

	mov BX, DI
	mov EAX, [BX]
	add DI, 5
	
	add word [SI + current_data_offset], 16

	ret

no_more_data:
	xor DI, DI
	xor EAX, EAX
	ret


;in  SI - адрес перечислителя
;in  EDI - адрес назначения
;in  EAX - кластер данных
;out EDI - адрес конца
load_data:
	mov [SI + current_cluster], EAX
	
load_next_data:
	push EDI
	call read_data
	pop EDI

	mov CX, 128
	mov BX, 0
	copy_next_data_byte:
	mov EAX, [BX + SI + FAT_data]
	mov [EDI], EAX
	add BX, 4
	add EDI, 4
	dec CX
	cmp CX, 0
	jne copy_next_data_byte

	push EDI
	call read_next_cluster
	pop EDI

	cmp AX, 0
	jne load_next_data

	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;in  SI - адрес перечислителя
read_data:
	mov EAX, [SI + current_cluster]
	add EAX, [SI + number_of_sectors_in_FAT]
	inc EAX
	mov [start_sector], EAX
	mov AX, SI
	add AX, FAT_data
	mov [buffer_address_offset], AX

	call read_sector
	ret


;in  SI - адрес перечислителя
;out AX - 1 если успех, 0 ошибка
read_next_cluster:
	mov EBX, [SI + current_cluster]
	and BX, 0b1111111
	shl BX, 2
	;BX = current_cluster % 128 / 4

	add BX, SI
	mov EAX, [BX]
	cmp EAX, 0xFFFFFFFF
	je no_next_cluster
	mov [SI + current_cluster], EAX

	call read_FAT
	mov AX, 1
	ret

no_next_cluster:
	mov AX, 0
	ret


;in  SI - адрес перечислителя
read_FAT:
	mov EAX, [SI + current_cluster]
	mov EBX, EAX
	shr EAX, 7
	inc EAX
	mov [start_sector], EAX
	;start_sector = current_cluster / 128 + 1

	cmp EBX, EAX
	je skip_read_FAT

	mov [buffer_address_offset], SI
	call read_sector

skip_read_FAT:
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;in  SI - адрес перечислителя
read_sector:
	push SI
	mov AH, 42h
	mov DL, [SI + drive_number]
	mov SI, LBA_packet
	int 13h
	pop SI
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

;in  AX - printed number
;print_number:
;	mov CX, 10
;	mov DX, 0
;	div CX
;	push DX
;	cmp AX, 0
;	je skip_print_number
;	call print_number
;skip_print_number:
;	pop DX
;	add DX, '0'
;	mov AL, DL
;	mov AH, 0x0E
;	int 0x10
;	ret


missing_kernel_error:
	mov SI, kernel_name
	call print_string
	mov SI, missing_error_message
	jmp print_message

read_error:
	mov SI, read_error_message

print_message:
	call print_string

end_message:
	mov AL, 10
	int 0x10
	mov AL, 13
	int 0x10

	mov AH, 0
	int 0x16
	int 0x19

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

kernel_name:           db "kernel", 0
read_error_message:    db "read error", 0
missing_error_message: db " is missing", 0