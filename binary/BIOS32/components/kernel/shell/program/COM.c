Boolean execute_COM_program(FAT_Data* file, API* api)
{
	Byte*    program;
	//void   (*program_start)(API* api);

	//program_start = program = 1024 * 1024;
	
	program = 1024 * 1024;
	
	while(read_FAT_file_sector(&fs, file, program)) {
		program += 512;
	}

	//program_start(api);
	
	execute(1024 * 1024, api);
	
	return 1;
}