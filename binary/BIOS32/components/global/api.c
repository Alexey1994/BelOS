#ifndef API_INCLUDED
#define API_INCLUDED


#include <types.c>
#include <file.c>


typedef struct {
	Byte* command_line;
	
	
	void    (*print)            (Byte* parameters, ...);
	
	Boolean (*enum_files)       (File_Enumerator* enumerator);
	Boolean (*open_file)        (FAT_Data* file, Byte* name);
	Number  (*read_file_sector) (FAT_Data* file, Byte* sector);
	
	void    (*reset)            ();
}
API;


#endif//API_INCLUDED