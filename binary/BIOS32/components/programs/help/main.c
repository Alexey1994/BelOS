#include <API.c>


void main(API* api)
{
	File_Enumerator enumerator;
	Boolean         is_many_files;

	is_many_files = 0;
	
	enumerator.data_cluster   = 0;
	enumerator.cluster_offset = 0;
	enumerator.file_number    = 0;
	
	while(api->enum_files(&enumerator)) {
		if(is_many_files) {
			api->print("\n");
		}
		
		api->print("%s", enumerator.file_data->name);
		is_many_files = 1;
	}
}