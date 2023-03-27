typedef enum {
	PS2_OUTPUT_BUFFER_FULL       = 0b00000001,
	PS2_INPUT_BUFFER_FULL        = 0b00000010,
	PS2_INITIALIZED              = 0b00000100,
	PS2_A2_STATE                 = 0b00001000,
	PS2_KEYBOARD_CONNECTED       = 0b00010000,
	PS2_MOUSE_OUTPUT_BUFFER_FULL = 0b00100000,
	PS2_TIMEOUT_ERROR            = 0b01000000,
	PS2_PARITY_ERROR             = 0b10000000
}
PS2_State;


typedef enum {
	PS2_READ_COMMAND_BYTE          = 0x20,
	PS2_WRITE_COMMAND_BYTE         = 0x60,
	PS2_DISABLE_MOUSE_INTERFACE    = 0xA7,
	PS2_ENABLE_MOUSE_INTERFACE     = 0xA8,
	PS2_MOUSE_INTERFACE_TEST       = 0xA9,
	PS2_KEYBOARD_INTERFACE_TEST    = 0xAB,
	PS2_DISABLE_KEYBOARD_INTERFACE = 0xAD,
	PS2_ENABLE_KEYBOARD_INTERFACE  = 0xAE,
	PS2_WRITE_IN_MOUSE_DEVICE      = 0xD4,
}
PS2_Command;


typedef enum {
	PS2_ENABLE_INPUT_BUFFER_FULL_INTERRUPT       = 0b00000001, //IRQ1
	PS2_ENABLE_MOUSE_INPUT_BUFFER_FULL_INTERRUPT = 0b00000010, //IRQ12
	PS2_DISABLE_MOUSE                            = 0b00100000,
}
PS2_Command_Byte;


typedef enum {
	PS2_MOUSE_ENABLE_STREAMING  = 0xF4,
	PS2_MOUSE_DISABLE_STREAMING = 0xF5,
	PS2_MOUSE_SET_DEFAULTS      = 0xF6,
}
PS2_Mouse_Device_Command;


Byte read_data_from_ps2()
{
	Number tries_count;
	
	//wait while input buffer not full
	tries_count = 0;
	while(!(in_8(0x64) & PS2_MOUSE_OUTPUT_BUFFER_FULL)) {
		++tries_count;
		
		if(tries_count > 1000) {
			return 0;
		}
	}
	
	return in_8(0x60);
}


Boolean write_data_in_ps2(Byte data)
{
	Number tries_count;
	
	//wait while output buffer not empty
	tries_count = 0;
	while(in_8(0x64) & PS2_OUTPUT_BUFFER_FULL) {
		++tries_count;
		
		if(tries_count > 1000) {
			return 0;
		}
	}
	
	out_8(0x60, data);
	
	return 1;
}


Boolean ps2_mouse_acknowledge()
{
	return read_data_from_ps2() == 0xFA;
}


Byte   ps2_mouse_packet[3];
Number ps2_mouse_packet_index;


void initialize_ps2_mouse()
{
	Byte command;
	
	out_8(0x64, PS2_DISABLE_MOUSE_INTERFACE);
	
	//reset mouse
	out_8(0x64, PS2_WRITE_IN_MOUSE_DEVICE);
	if(!write_data_in_ps2(PS2_MOUSE_SET_DEFAULTS)) {
		return;
	}
	if(!ps2_mouse_acknowledge()) {
		return;
	}
	
	//enable streaming
	out_8(0x64, PS2_WRITE_IN_MOUSE_DEVICE);
	if(!write_data_in_ps2(PS2_MOUSE_ENABLE_STREAMING)) {
		return;
	}
	if(!ps2_mouse_acknowledge()) {
		return;
	}

	//enable IRQ12
	asm("cli");
	ps2_mouse_packet_index = 0;
	out_8(0x64, PS2_READ_COMMAND_BYTE);
	command = in_8(0x60);
	command |= PS2_ENABLE_MOUSE_INPUT_BUFFER_FULL_INTERRUPT;
	command &= ~PS2_DISABLE_MOUSE;
	out_8(0x64, PS2_WRITE_COMMAND_BYTE);
	write_data_in_ps2(command);
	asm("sti");
	
	out_8(0x64, PS2_ENABLE_MOUSE_INTERFACE);
}


asm("pusha");
asm("call interrupt_44_handler");
asm("mov $0x20, %al");
asm("out %al, $0xA0");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void interrupt_44_handler()
{
	ps2_mouse_packet[ps2_mouse_packet_index] = read_data_from_ps2();
	++ps2_mouse_packet_index;
	
	if(ps2_mouse_packet_index == 3) {
		//print(
		//	"lb%d "
		//	"rb%d "
		//	"\n",
		//	!!(ps2_mouse_packet[0] & 0b00000001),
		//	!!(ps2_mouse_packet[0] & 0b00000010)
		//);
		
		ps2_mouse_packet_index = 0;
	}
}