Byte* heap_top = 1024 * 1024;


void reset_heap()
{
	heap_top = 1024 * 1024;
}


Byte* allocate_memory(Number size)
{
	Byte* allocated_memory;
	
	allocated_memory = heap_top;
	heap_top += size;
	
	return allocated_memory;
}


void free_memory(Byte* allocated_memory)
{
	heap_top = allocated_memory;
}