%macro out_8 2
	mov AL, %2
	out %1, AL
%endmacro


%define KERNEL_STACK 0x8000


org 0x8000


mov AX, CS
mov DS, AX
mov SS, AX
mov SP, KERNEL_STACK


;clear_screen
mov AL, ' '
mov CX, 80 * 25
repeat_clear_screen:
	call print_char
	dec CX
	cmp CX, 0
	jne repeat_clear_screen


;enable A20
cli
in AL, 0x92
or AL, 2
out 0x92, AL


lgdt [GDT_pointer]

mov dword [ESP_for_32_bits], KERNEL_STACK
call switch_to_32_bit
use32

call set_text_mode

push set_text_mode
push set_VESA_mode
push get_VESA_mode_info
push get_VESA_Info
push reset
push read_sector
push clean_interrupt_handler
push set_interrupt_handler
call kernel_main
	
no_actions:
	hlt
	jmp no_actions


use16

;AL - char code
print_char:
	mov AH, 0x0E
	int 0x10
	ret


;in  AX - printed number
print_number:
	mov CX, 10
	mov DX, 0
	div CX
	push DX
	cmp AX, 0
	je skip_print_number
	call print_number
skip_print_number:
	pop DX
	add DX, '0'
	mov AL, DL
	mov AH, 0x0E
	int 0x10
	ret


;BX - PIC setting, 0x0870 for 16 bit, 0x2028 for 32
reset_pic:
	out_8 0x20, 0x11
	out_8 0x21, BH
	out_8 0x21, 0x04
	out_8 0x21, 0x01
	out_8 0x21, 0x00
	
	out_8 0xA0, 0x11
	out_8 0xA1, BL
	out_8 0xA1, 0x02
	out_8 0xA1, 0x01
	out_8 0xA1, 0x00
	ret


align 16
GDT:
	; dummy
	dq 0

	; CODE (CS register = 8)
	dw 0xFFFF     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10011010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    если сегмент в памяти то 1
	              ; 1    сегмент исполняемый
	              ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода(1 - разрешено, 0 - запрещено)
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    зарезервировано
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес

	; DATA (DS register = 16)
	dw 0xffff     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10010010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    если сегмент в памяти то 1
	              ; 0    сегмент исполняемый
	              ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    зарезервировано
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес
	
	; CODE16 (CS register = 24)
	dw 0xFFFF     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10011010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    0 - системный, 1 - код или данные
	              ; 1    сегмент исполняемый
	              ; 1    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода(1 - разрешено, 0 - запрещено)
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b00001111 ; 0    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 0    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    зарезервировано
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес

	; DATA16 (DS register = 32)
	dw 0xFFFF     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10010010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    0 - системный, 1 - код или данные
	              ; 0    сегмент исполняемый
	              ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b00001111 ; 0    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 0    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    зарезервировано
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес

GDT_pointer:
	dw $ - GDT ;размер
	dd GDT     ;адрес


%macro Trap_Desc 3
	dw %1  ; handler_address_low
	dw %2  ; selector
	db 0   ; zero
	db %3  ; attributes
	dw 0   ; handler_address_high
%endmacro

align 16
IDT:
	%rep 256
	Trap_Desc empty_interrupt_handler, 8, 0x8E
	%endrep
	

IDT_pointer:
	dw $ - IDT ;размер
	dd IDT     ;адрес


use32
empty_interrupt_handler:
	pusha
	mov AL, 0x20
	out 0x20, AL
	out 0xA0, AL
	popa
	iret


;handler must be in first 64kb of RAM
use32
set_interrupt_handler:
	cli
	
	mov EBX, IDT
	
	mov EAX, [ESP + 8]
	shl EAX, 3
	add EBX, EAX
	
	mov EAX, [ESP + 4]

	mov [EBX], AX

	sti
	ret


use32
clean_interrupt_handler:
	mov EAX, [ESP + 4]
	push EAX
	push dword empty_interrupt_handler
	call set_interrupt_handler
	add ESP, 8
	ret


saved_EIP: dd 0
ESP_for_32_bits: dd 0

use32
switch_to_16_bit:
	cli

	pop dword [saved_EIP]
	mov [ESP_for_32_bits], ESP
	xor ESP, ESP
	
	lidt [idtr_16]

	mov AX, 32
	mov DS, AX
	mov SS, AX
	mov ES, AX
	mov FS, AX
	mov GS, AX

	jmp 24:p_16_bit
	
	align 16
	use16
	p_16_bit:

	mov EAX, CR0
	and EAX, 0xFFFFFFFE
	mov CR0, EAX

	jmp 0:r_16_bit
	
	r_16_bit:
	mov AX, CS
	mov DS, AX
	mov SS, AX
	mov ES, AX
	mov FS, AX
	mov GS, AX
	
	mov SP, 0x1000
	push word [saved_EIP]
	
	mov BX, 0x0870
	call reset_pic

	sti
	ret


;in ESP_for_32_bits
use16
switch_to_32_bit:
	cli

	mov BX, 0x2028
	call reset_pic
	
	lidt [IDT_pointer]

	mov EAX, CR0
	or EAX, 1
	mov CR0, EAX
	
	jmp 8:setup_32_bit_segment_registers

	use32
	setup_32_bit_segment_registers:
	mov EAX, 16
	mov DS, EAX
	mov SS, EAX
	mov ES, EAX
	mov FS, EAX
	mov GS, EAX
	
	xor EAX, EAX
	pop AX
	mov ESP, [ESP_for_32_bits]
	push EAX
	
	sti
	ret


align 4
idtr_16:
	dw 0x3FF
	dd 0


%define read_sector_buffer 0x0500

read_sector_success: dd 1

;read_sector(Number32 start, Byte* sector)
use32
read_sector:
	mov EAX, [ESP + 4]
	mov [start_sector], EAX
	
	;mov EAX, [ESP + 8]
	;mov [buffer_address_offset], AX


	call switch_to_16_bit
	use16
	
	mov byte [read_sector_success], 1
	
	mov AH, 42h
	mov DL, 0x80
	mov SI, LBA_packet
	int 13h
	
	jnc no_read_error
	mov dword [read_sector_success], 0
	no_read_error:
	
	
	call switch_to_32_bit
	use32
	
	mov ESI, read_sector_buffer
	mov EDI, [ESP + 8]
	mov ECX, 512 / 4
	rep movsd
	
	mov EAX, [read_sector_success]
	
	ret
	
	LBA_packet:
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 1
		buffer_address_offset  dw read_sector_buffer
		buffer_address_segment dw 0
		start_sector           dq 0


use32
reset:
	call switch_to_16_bit
	use16
	
	int 19h
	
	call switch_to_32_bit
	use32
	
	ret


VESA_mode: dw 0
VESA_mode_info: dd 0;times 256 db 0
;VESA_Info: times 20 db 0
;VESA_CRTC_info: times 64 db 0
%define VESA_Info 0x500
%define VESA_CRTC_info 0x500
;%define VESA_mode_info 0x500


use32
get_VESA_Info:
	call switch_to_16_bit
	use16
	
	mov AX, 0x4F00
	mov DI, VESA_Info
	int 10h
	
	call switch_to_32_bit
	use32
	
	mov EAX, VESA_Info
	
	ret


use32
get_VESA_mode_info:
	mov EAX, [ESP + 4]
	mov [VESA_mode_info], EAX
	
	mov EAX, [ESP + 8]
	mov [VESA_mode], AX
	
	call switch_to_16_bit
	use16
	
	mov AX, 0x4F01
	mov CX, [VESA_mode]
	mov DI, [VESA_mode_info]
	int 10h
	
	call switch_to_32_bit
	use32
	
	ret


use32
set_VESA_mode:
	mov EAX, [ESP + 4]
	mov [VESA_mode], AX
	
	call switch_to_16_bit
	use16
	
	mov AX, 0x4F02
	mov BX, [VESA_mode]
	mov DI, VESA_CRTC_info
	int 10h
	
	call switch_to_32_bit
	use32
	
	ret


use32
set_text_mode:
	call switch_to_16_bit
	use16
	
	mov AH, 0
	mov AL, 3 | 0b10000000
	int 10h
	
	;set 8x8 font
	;mov AH, 0x11
	;mov AL, 0x12
	;xor BX, BX
	;int 10h
	
	call switch_to_32_bit
	use32

	ret


align 32
kernel_main: