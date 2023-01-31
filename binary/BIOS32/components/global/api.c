#ifndef API_INCLUDED
#define API_INCLUDED


#include <types.c>
#include <file.c>


typedef struct {
	void    (*print)            (Byte* parameters, ...);
	
	Boolean (*enum_files)       (File_Enumerator* enumerator);
	Boolean (*open_file)        (FAT_Data* file, Byte* name);
	Number  (*read_file_sector) (FAT_Data* file, Byte* sector);
	
	void    (*reset)            ();
	
	Number number_of_arguments;
	Byte*  arguments[256];
}
API;


#endif//API_INCLUDED