Boolean execute_COM_program(FAT_Data* file, API* api)
{
	Byte*    program;

	program = 1024 * 1024;
	
	while(read_FAT_file_sector(&fs, file, program)) {
		program += 512;
	}

	execute(1024 * 1024, api);
	
	return 1;
}