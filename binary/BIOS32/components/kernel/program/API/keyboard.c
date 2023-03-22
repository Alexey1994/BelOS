//void (*on_key_down_in_program_handler)(Byte key_code, Boolean is_special) = 0;
//void (*on_key_up_in_program_handler)(Byte key_code, Boolean is_special) = 0;


void set_key_down_handler(void(*key_down_handler)(Byte key_code, Boolean is_special))
{
	//on_key_down_in_program_handler = key_down_handler;
	on_key_down_handler = key_down_handler;
}


void set_key_up_handler(void(*key_up_handler)(Byte key_code, Boolean is_special))
{
	//on_key_up_in_program_handler = key_up_handler;
	on_key_up_handler = key_up_handler;
}


Number read_character_from_keyboard_with_wait(Byte* keyboard)
{
	Number character;
	
	for(;;) {
		character = read_character_from_keyboard();
		
		if(character) {
			break;
		}
		
		asm("hlt");
	}
	
	return character;
}