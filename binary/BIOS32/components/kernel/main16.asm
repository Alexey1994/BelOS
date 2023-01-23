org 0x8000


mov AX, CS
mov DS, AX
mov SS, AX
mov SP, 0x10000 - 4


;clear_screen
mov AL, ' '
mov CX, 80 * 25
repeat_clear_screen:
	call print_char
	dec CX
	cmp CX, 0
	jne repeat_clear_screen


;информация о VESA
;mov AX, 0x4F00
;mov DI, 0x7E00
;int 10h
;cmp AX, 0x004F je video_error
;поиск режима
;mov EAX, 0x7E00 + 14


;установка видеорежима 1024x768x256
;mov AX, 0x4F02
;mov BX, 0x4105 ;1024x768x256
;int 10h


;получение информации о видеорежиме
;mov AX, 0x4F01
;mov CX, 0x4105 ;1024x768x256
;mov DI, 0x7D00
;int 10h


;разрешить A20
cli
in AL, 0x92
or AL, 2
out 0x92, AL


lgdt [GDT_pointer]
call switch_to_32_bit
use32

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


%macro out_8 2
	mov AL, %2
	out %1, AL
%endmacro

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
	dw 0xffff     ; размер сегмента
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
	              ; 0    AVL(?)
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
	              ; 0    AVL(?)
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
	              ; 0    AVL(?)
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
	              ; 0    AVL(?)
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
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	Trap_Desc make_trap_frame, 8, 0x8E
	

IDT_pointer:
	dw $ - IDT ;размер
	dd IDT     ;адрес


use32
make_trap_frame:
	pusha
	out_8 0x20, 0x20
	popa
	iret


;обработчик должен быть в первых 64кб оперативной памяти
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
	push dword make_trap_frame
	call set_interrupt_handler
	add ESP, 8
	ret


use32
switch_to_16_bit:
	cli

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
	
	mov BX, 0x0870
	call reset_pic


	sti
	
	pop EAX
	push AX
	
	ret


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
	
	sti

	xor EAX, EAX
	pop AX
	push EAX

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
	;mov AL, 'E'
	;call print_char
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


align 32
kernel_main: