Boolean execute_COM_program(Byte* name, Byte* command_line)
{
	Byte*    program;
	void   (*program_start)(API* api);
	FAT_Data file;

	program_start = program = 1024 * 1024;
	
	if(!open_FAT_file(&fs,  &file, name)) {
		return 0;
	}
	
	while(read_FAT_file_sector(&fs, &file, program)) {
		program += 512;
	}
	
	API api = {
		.command_line = command_line,
		.print = &print,
		.enum_files = &enum_files,
		.open_file = &open_file,
		.read_file_sector = &read_file_sector,
		.reset = reset,
	};

	program_start(&api);
	
	return 1;
}