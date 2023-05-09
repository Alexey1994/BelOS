#ifndef MOUSE_INCLUDED
#define MOUSE_INCLUDED


typedef struct {
	Byte           left_button_pressed:1;
	Byte           right_button_pressed:1;
	Byte           middle_button_pressed:1;
	Byte           one:1;
	Byte           x_sign:1;
	Byte           y_sign:1;
	Byte           x_overflow:1;
	Byte           y_overflow:1;
	Signed_Number8 x;
	Signed_Number8 y;
}
Mouse_Event;


#endif//MOUSE_INCLUDED