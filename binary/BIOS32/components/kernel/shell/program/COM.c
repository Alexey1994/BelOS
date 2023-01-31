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
	
	API api;

	initialize_program_api(&api);

	program_start(&api);
	
	return 1;
}