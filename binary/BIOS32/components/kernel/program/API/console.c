Number read_character_from_console(Byte* console)
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