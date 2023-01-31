typedef struct {
	Byte     signature[2];
	Byte     pad[0x3C - 2];
	Number32 pe_start;
}
MZ_Header;


typedef enum {
	I386_MACHINE = 0x014C
}
PE_Machine;

typedef enum {
	APPLICATION_32 = 0x010B
}
PE_Type;


typedef struct {
	Byte signature[4]; //PE\0\0
	Number16 machine; //I386_MACHINE
	Number16 number_of_sections;
	Number32 timestamp;
	Number32 deprecated1;
	Number32 deprecated2;
	Number16 size_of_optional_header;
	Number16 characteristics;
}
PE_Header;


typedef struct {
	Number16 magic; //APPLICATION_32
	Number16 linker_version;
	Number32 size_of_code;
	Number32 size_of_initialized_data;
	Number32 size_of_uninitialized_data;
	Number32 entry_point;
	Number32 base_of_code;
	Number32 base_of_data;
	
	Number32 image_base;
	Number32 section_alignment;
	Number32 file_alignment;
	
	Number32 os_version;
	Number32 image_version;
	Number32 subsystem_version;
	Number32 win32_version;
	
	Number32 size_of_image;
	Number32 size_of_headers;
	
	Number32 checksum;
	Number16 subsystem; //3 for console
	Number16 dll_characteristics;
	
	Number32 size_of_stack_reserve;
	Number32 size_of_stack_commit;
	
	Number32 size_of_heap_reserve;
	Number32 size_of_heap_commit;
	
	Number32 loader_flags;
	
	Number32 number_of_directories;
}
PE_Optional_Header;


typedef enum {
	EXECUTABLE_CODE_PE_SECTION       = 0x00000020,
	INITIALIZED_DATA_PE_SECTION      = 0x00000040,
	UNINITIALIZED_DATA_PE_SECTION    = 0x00000080,
	ENABLE_EXECUTE_MEMORY_PE_SECTION = 0x20000000,
	ENABLE_READ_MEMORY_PE_SECTION    = 0x40000000,
	ENABLE_WRITE_MEMORY_PE_SECTION   = 0x80000000
}
PE_Section_Type;

typedef struct {
	Byte name[8];
	Number32 memory_size;
	Number32 address;
	Number32 raw_size;
	Number32 raw_data;
	Number32 relocations; //0
	Number32 line_numbers; //0
	Number32 number_of_relocations; //0
	Number32 flags;
}
PE_Section_Header;


typedef struct {
	Number32 address;
	Number32 size;
}
PE_Directory;


typedef struct {
	Number32 lookup_table_address;
	Number32 time;
	Number32 first_forwarder_index;
	Number32 name_address;
	Number32 import_address_table;
}
PE_Import_Directory;

typedef union {
	struct {
		Number16 number;
		Number16 flag; // 0x80000000 for ordinal
	}
	ordinal;
	
	Number32 names;
}
PE_Import_Lookup_Table;


Boolean execute_EXE_program(Byte* name, Byte* command_line)
{
	Byte*    program;
	void   (*program_start)(API* api);
	FAT_Data file;

	program_start = program = 1024 * 1024;
	
	if(!open_FAT_file(&fs,  &file, name)) {
		return 0;
	}
	
	if(!read_FAT_file_sector(&fs, &file, program)) {
		return 0;
	}
	

	
	MZ_Header*          mz_header;
	PE_Header*          pe_header;
	PE_Optional_Header* pe_optional_header;
	PE_Directory*       pe_directories;
	PE_Section_Header*  pe_section_header;
	Number i;
	Number j;

	mz_header = program;
	
	if(
		mz_header->signature[0] != 'M' ||
		mz_header->signature[1] != 'Z'
	) {
		print("MZ not found");
		return 0;
	}
	
	pe_header = program + mz_header->pe_start;
	
	if(
		pe_header->signature[0] != 'P' ||
		pe_header->signature[1] != 'E' ||
		pe_header->signature[2] != '\0' ||
		pe_header->signature[3] != '\0'
	) {
		print("PE not found");
		return 0;
	}
	
	if(pe_header->machine != I386_MACHINE) {
		print("is not I386 application");
		return 0;
	}
	
	pe_optional_header = pe_header + 1;
	
	if(pe_optional_header->magic != APPLICATION_32) {
		print("is not 32 bits application");
		return 0;
	}
	
	program += 512;
	for(i = 0; i < pe_optional_header->size_of_headers / 512 - 1; ++i) {
		if(!read_FAT_file_sector(&fs, &file, program)) {
			return 0;
		}
		
		program += 512;
	}
	
	pe_directories = pe_optional_header + 1;
	
	pe_section_header = pe_directories + pe_optional_header->number_of_directories;
	
	
	for(i = 0; i < pe_header->number_of_sections; ++i) {
		/*print(
			"%s address: %d raw_size: %d raw_data: %d\n",
			pe_section_header[i].name,
			pe_section_header[i].address,
			pe_section_header[i].raw_size,
			pe_section_header[i].raw_data
		);*/
		
		program = pe_optional_header->image_base + pe_section_header[i].address;
		//TODO: rewind file to pe_section_header[i].raw_data
		
		for(j = 0; j < pe_section_header[i].raw_size; j += 512) {
			if(!read_FAT_file_sector(&fs, &file, program)) {
				return 0;
			}
			
			program += 512;
		}
	}
	
	
	API api;

	initialize_program_api(&api);
	
	//TODO: handle import directory
	
	program_start = pe_optional_header->image_base + pe_optional_header->entry_point;
	
	program_start(&api);
	
	return 1;
}