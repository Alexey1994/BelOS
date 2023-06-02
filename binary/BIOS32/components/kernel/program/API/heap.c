Byte* heap_top = 1024 * 1024;


void set_heap_top(Number new_top)
{
	heap_top = new_top;
}


void reset_heap()
{
	set_heap_top(1024 * 1024);
}


Byte* allocate_memory(Number size)
{
	Byte* allocated_memory;
	
	allocated_memory = heap_top;
	heap_top += size;
	
	return allocated_memory;
}


Byte* allocate_aligned_memory(Number size, Number align)
{
	Byte* allocated_memory;
	
	while((Number)heap_top % align) {
		++heap_top;
	}
	
	allocated_memory = heap_top;
	heap_top += size;
	
	return allocated_memory;
}


void free_memory(Byte* allocated_memory)
{
	heap_top = allocated_memory;
}