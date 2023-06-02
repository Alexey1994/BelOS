typedef struct {
	Number8  size;
	Number8  reserved;
	Number16 version;
	/*struct {
		Number32 number_of_ports:4;
		Number32 port_power:4; // 0 - auto
		Number32 number_of_companion_ports:4;
		Number32 companion_controller_number:4;
		Number32 port_indicators:16;
	}*/
	Number32 structural_parameters;
	/*struct {
		Number32 compatible_with_64:1;
		Number32 enable_periodic_queue:1;
		Number32 enable_async_queue:1;
		Number32 reserved1:1;
		Number32 isochronus_scheduling_threshold:4;
		Number32 extended_capabilities_pointer:8;
	}*/
	Number32 capability_parameters;
	Number32 companion_port_route;
}
EHCI_Capability;


typedef struct {
	/*struct {
		Number32 run:1;
		Number32 reset:1;
		Number32 frame_list_size:2; // 0 - 1024, 1 - 512, 2 - 256
		Number32 enable_periodic_schedule:1;
		Number32 enable_async_schedule:1;
		Number32 interrupt_on_async_advance_doorbell:1;
		Number32 light_reset:1;
		Number32 async_schedule_park_mode_count:2;
		Number32 zero1:1;
		Number32 async_schedule_park_mode_enable:1;
		Number32 zero2:4;
		Number32 number_of_microframes_to_process_between_interrupt:8;
		Number32 zero3:8;
	}*/
	Number32 command;
	struct {
		Number32 transfer_interrupt:1;
		Number32 error_interrupt:1;
		Number32 port_change_interrupt:1;
		Number32 frame_list_rellover:1;
		Number32 host_system_error:1;
		Number32 doorbell_interrupt:1;
		Number32 port_change:1;
		Number32 zero1:6;
		Number32 halted:1;
		Number32 reclamation:1;
		Number32 periodic_schedule_status:1;
		Number32 async_schedule_status:1;
		Number32 zero2:15;
	}
	status;
	//Number32 status;
	/*struct {
		Number32 transfer_interrupt_enable : 1;
		Number32 error_interrupt_enable : 1;
		Number32 port_change_interrupt_enable : 1;
	}*/
	Number32 interrupt;
	
	Number32 frame_index;
	Number32 segment_selector;
	Number32 frame_list_address;
	Number32 asynchronus_list_address;
	Number32 pad[10];
	Number32 config;
	
	/*struct {
		Number32 connected:1;
		Number32 connect_changed:1;
		Number32 enabled:1;
		Number32 enabled_changed:1;
		Number32 overcurrent:1;
		Number32 overcurrent_changed:1;
		Number32 force_resume:1;
		Number32 suspend:1;
		Number32 reset:1;
		Number32 zero1:1;
		Number32 line_status:2;
		Number32 port_power:1;
		Number32 companion_port_control:1; // 0 - local, 1 - companion host controller
		Number32 indicator:1; // 0 - off, 1 - amber, 2 - green
		Number32 port_test:4;
		Number32 wake_on_connect:1;
		Number32 wake_on_disconnect:1;
		Number32 wake_on_overcurrent:1;
		Number32 zero2:9;
	}*/
	Number32 port_status_and_control[32];
}
EHCI_Operation;


typedef struct {
	Number32 horizontal_link_pointer;
	/*
	struct {
		Number32 device_address:7;
		Number32 inactive:1;
		Number32 endpoint_number:4;
		Number32 endpoint_speed:2; //0 - full, 1 - low, 2 - high
		Number32 data_toggle_control:1;
		Number32 head_of_reclamation_list:1; //set if is the first Queue Head in async list
		Number32 max_packet_length:11;
		Number32 control_endpoint:1; //not used for high speed devices
		Number32 NAK_reload:4;
	}
	endpoint_characteristics;
	*/
	Number32 endpoint_characteristics;
	Number32 endpoint_capabilities;
	
	//Number32 current_TD_address;
	//Number32 current_TD_working_area; //copy of current TD data
	
	
	Number32 current_link;
	
	Number32 next_link;
	Number32 alternative_link;
	Number32 token;
	Number32 buffer[5];
	Number32 ext_buffer[5];
	
	//internal fields ???
	//USB_Transfer* transfer;
	//Link qh_link;
	//Number32 td_head;
	//Number32 active;
	//Number8 pad[20]
}
EHCI_Queue_Head;


EHCI_Capability* _capability;
EHCI_Operation*  _operation;

EHCI_Queue_Head* _periodic_queue_head;
Number32*        _frame_list;
EHCI_Queue_Head* _async_queue_head;


void print_EHCI_ports()
{
	get_module_address_by_function(print_EHCI_ports);
	EHCI_Capability* capability = global(_capability);
	EHCI_Operation*  operation = global(_operation);
	
	
		Number i;
		Number number_of_ports;
		
		number_of_ports = capability->structural_parameters & 0b1111;
		
		for(i = 0; i < number_of_ports; ++i) {
			Number32 port_status = operation->port_status_and_control[i];
			
			if(!(port_status & 1)) {
				continue;
			}
			
			if(!port_status & (1 << 13)) {
				//operation->port_status_and_control[i] |= (1 << 2); //enabled
				//operation->port_status_and_control[i] |= (1 << 12);//port_power
				
				operation->port_status_and_control[i] |= (1 << 8); //reset
				//sleep(100);
				Number j;
				for(j = 0; j < 1000000000; ++j);
				
				port_status = operation->port_status_and_control[i];
			}
			
			print(global_ptr("port %d"), i);
			
			if(port_status & 1) {
				print(global_ptr(" connected"));
			}
			
			if(port_status & (1 << 1)) {
				print(global_ptr(" connect_changed"));
			}
			
			if(port_status & (1 << 2)) {
				print(global_ptr(" enabled"));
			}
			
			if(port_status & (1 << 12)) {
				print(global_ptr(" powered"));
			}
			
			if(port_status & (1 << 13)) {
				print(global_ptr(" is_companion"));
			}
			
			print(global_ptr("\n"));
			
			//operation->port_status_and_control[i] |= (1 << 2); //enabled
			//operation->port_status_and_control[i] |= (1 << 12);//port_power
			
			//operation->port_status_and_control[i] &= ~(1 << 12);//port_power
			//operation->port_status_and_control[i] |= (1 << 12);//port_power
			/*
			operation->port_status_and_control[i] |= (1 << 8); //reset
			//sleep(100);
			Number j;
			for(j = 0; j < 1000000000; ++j);
			operation->port_status_and_control[i] |= (1 << 2); //enabled
			operation->port_status_and_control[i] |= (1 << 12);//port_power*/
		}
}


asm("pusha");
asm("call . + 13");
asm("mov $0x20, %al");
asm("out %al, $0xA0");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void EHCI_interrupt_handler()
{
	get_module_address_by_function(EHCI_interrupt_handler);
	EHCI_Capability* capability = global(_capability);
	EHCI_Operation*  operation = global(_operation);
	
	
	//print(global_ptr("EHCI interrupt\n"));
	
	/*if(operation->status.transfer_interrupt) {
		print(global_ptr("transfer "));
	}
	if(operation->status.error_interrupt) {
		print(global_ptr("error "));
	}
	if(operation->status.port_change_interrupt) {
		print(global_ptr("port_change_detect "));
	}
	if(operation->status.host_system_error) {
		print(global_ptr("host_system_error "));
	}
	if(operation->status.doorbell_interrupt) {
		print(global_ptr("doorbell_interrupt "));
	}
	print(global_ptr("\n"));*/
	
	if(operation->status.port_change_interrupt) {
		print_EHCI_ports();
	}
	
	//print(global_ptr("frame index: %d\n"), operation->frame_index >> 3);
	
	/*print(global_ptr("\nframe_index is (micro %d, frame %d)\n"), operation->frame_index >> 28, operation->frame_index & 0x0FFFFFFF);
	print(global_ptr("segment_selector is %d\n"), operation->segment_selector);
	print(global_ptr("frame_list_address is %d\n"), operation->frame_list_address);
	print(global_ptr("asynchronus_list_address is %d\n"), operation->asynchronus_list_address);*/
	
	//print(global_ptr("\nframe_index is (micro %d, frame %d)\n"), operation->frame_index >> 28, operation->frame_index & 0x0FFFFFFF);
	//print(global_ptr("\nframe_index is (micro %d, frame %d)\n"), operation->frame_index & 0b1111, (operation->frame_index >> 4) & 1023);
	
	/*Number frame_index = (operation->frame_index >> 4) & 1023;
	
	Number32* frame_list = operation->frame_list_address & 0x0FFFF000;
	EHCI_Queue_Head* periodic_queue_head = (Number32)frame_list[frame_index] & 0x0FFFF000;
	
	print(global_ptr("%d\n"), periodic_queue_head);
	
	print(global_ptr("%d %d %d %d %d\n"), periodic_queue_head->buffer[0], periodic_queue_head->buffer[1], periodic_queue_head->buffer[2], periodic_queue_head->buffer[3], periodic_queue_head->buffer[4]);
	Byte* buffer = frame_list->buffer[0];
	Number i;
	for(i = 0; i < 20; ++i) {
		print(global_ptr("%d "), buffer[i]);
	}
	print(global_ptr("\n"));*/
	
	//for(;;);
	
	/*print(global_ptr("endpoint_number %d\n"), (frame_list->endpoint_characteristics >> 7) & 0b1111);
	print(global_ptr("endpoint_speed %d\n"), (frame_list->endpoint_characteristics >> 12) & 0b11);
	print(global_ptr("data_toggle_control %d\n"), (frame_list->endpoint_characteristics >> 14) & 0b1);
	print(global_ptr("head_of_reclamation_list %d\n"), (frame_list->endpoint_characteristics >> 15) & 0b1);
	print(global_ptr("max_packet_length %d\n"), (frame_list->endpoint_characteristics >> 16) & 0b11111111111);
	print(global_ptr("control_endpoint %d\n"), (frame_list->endpoint_characteristics >> 27) & 0b1);
	print(global_ptr("NAK_reload %d\n\n"), (frame_list->endpoint_characteristics >> 28) & 0b1111);
	*/
	
	/*
	Number32 horizontal_link_pointer;
	
	struct {
		Number32 device_address:7;
		Number32 inactive:1;
		Number32 endpoint_number:4;
		Number32 endpoint_speed:2; //0 - full, 1 - low, 2 - high
		Number32 data_toggle_control:1;
		Number32 head_of_reclamation_list:1; //set if is the first Queue Head in async list
		Number32 max_packet_length:11;
		Number32 control_endpoint:1; //not used for high speed devices
		Number32 NAK_reload:4;
	}
	endpoint_characteristics;
	Number32 endpoint_characteristics;
	Number32 endpoint_capabilities;
	
	//Number32 current_TD_address;
	//Number32 current_TD_working_area; //copy of current TD data
	
	
	Number32 current_link;
	
	Number32 next_link;
	Number32 alternative_link;
	Number32 token;
	Number32 buffer[5];
	Number32 ext_buffer[5];
	*/
	
	*(Number32*)&operation->status = ~0; //clear status
	print(global_ptr("\n"));
}


void configure_USB_EHCI(Number16 address, PCI_Config* config)
{
	get_module_address_by_function(configure_USB_EHCI);
	Interrupt_Interface* interrupt_interface = global(_interrupt_interface);
	PCI_Interface* pci_interface = global(_pci_interface);
	Heap_Interface* heap_interface = global(_heap_interface);
	
	
	Number i;
	EHCI_Capability* capability;
	EHCI_Operation*  operation;
	EHCI_Queue_Head*  periodic_queue_head;
	EHCI_Queue_Head*  async_queue_head;
	Number32*         frame_list;
	
	capability = pci_interface->get_PCI_MMIO_address(config, 0);
	global(_capability) = capability;
	operation = (Byte*)capability + capability->size;
	global(_operation) = operation;
	
	//disable legacy
	Byte extended_capabilities_pointer;
	extended_capabilities_pointer = (capability->capability_parameters >> 8) & 0xFF;
	if(extended_capabilities_pointer >= 0x40) {
		Number32 legacy_support;
		legacy_support = *(Number32*)((Byte*)config + extended_capabilities_pointer);
		
		if(legacy_support & 0x00010000) { //HC BIOS Owned semaphore
			legacy_support |= 0x01000000; //HC OS Owned semaphore
			pci_interface->write_4bytes_in_PCI_config(address, extended_capabilities_pointer, legacy_support);
			
			do {
				legacy_support = pci_interface->read_4bytes_from_PCI_config(address, extended_capabilities_pointer);
			}
			while(!(legacy_support & 0x01000000) && (legacy_support & 0x00010000));
		}
	}

	
	//operation->command &= ~(1); //stop
	
	//operation->status = ~0;//0x3f; //clear status
	*(Number32*)&operation->status = ~0; //clear status
	
	//operation->command = 0; //stop
	
	//reset
	//operation->command = (8 << 16) | (1 << 1); //light_reset | reset
	//operation->command |= (8 << 16) | (1 << 1) | (1 << 4) | (1 << 5); //light_reset | reset | periodic_schedule_enable | async_schedule_enable
	//operation->command = (1 << 1) | (1 << 4) | (1 << 5); //reset | periodic_schedule_enable | async_schedule_enable
	operation->command = (1 << 1); //reset
	while(operation->status.halted);
	//operation->command |= 255 << 16; //interrupt threshold
	//operation->command |= 2 << 2; //frame list size = 256
	
	/*
		Number32 transfer_interrupt:1;
		Number32 error_interrupt:1;
		Number32 port_change_interrupt:1;
		Number32 frame_list_rellover:1;
		Number32 host_system_error:1;
		Number32 doorbell_interrupt:1;
		Number32 port_change:1;
		Number32 zero1:6;
		Number32 halted:1;
		Number32 reclamation:1;
		Number32 periodic_schedule_status:1;
		Number32 async_schedule_status:1;
		Number32 zero2:15;*/
	
	print(global_ptr("reseted\n"));
	
	
	periodic_queue_head = heap_interface->allocate_aligned(sizeof(EHCI_Queue_Head), 4096);
	global(_periodic_queue_head) = periodic_queue_head;
	periodic_queue_head->horizontal_link_pointer = (1 << 0); // PTR_TERMINATE
	periodic_queue_head->endpoint_characteristics = 0;
	periodic_queue_head->endpoint_capabilities = 0;
	periodic_queue_head->current_link = 0;
	periodic_queue_head->next_link = (1 << 0); // PTR_TERMINATE
	periodic_queue_head->alternative_link = 0;
	periodic_queue_head->token = 0;
	
	for(i = 0; i < 5; ++i) {
		periodic_queue_head->buffer[i] = 0;
		periodic_queue_head->ext_buffer[i] = 0;
	}
	
	frame_list = heap_interface->allocate_aligned(1024 * 4, 4096);
	global(_frame_list) = frame_list;
	
	for(i = 0; i < 1024; ++i) {
		frame_list[i] = (Number32)periodic_queue_head | (1 << 1); // | PTR_QH
	}
	
	async_queue_head = heap_interface->allocate_aligned(sizeof(EHCI_Queue_Head), 4096);
	global(_async_queue_head) = async_queue_head;
	async_queue_head->horizontal_link_pointer = (Number32)async_queue_head | (1 << 1); // | PTR_QH
	async_queue_head->endpoint_characteristics = 0x0000800; // QH_CH_H
	async_queue_head->endpoint_capabilities = 0;
	async_queue_head->current_link = 0;
	async_queue_head->next_link = (1 << 0); // PTR_TERMINATE
	async_queue_head->alternative_link = 0;
	async_queue_head->token = 0;
	
	operation->segment_selector = 0;
	operation->frame_index = 0;
	operation->frame_list_address = frame_list;//1024 * 1024; //periodic list
	operation->asynchronus_list_address = async_queue_head;//2 * 1024 * 1024; //queue head
	
	//print("%d\n", operation->command);
	
	
	
	//operation->interrupt |= 1 | (1 << 1) | (1 << 2) | (1 << 5); //transfer_interrupt_enable | error_interrupt_enable | port_change_interrupt_enable | async_advance_interrupt_enable
	//operation->interrupt = 0; //disable all interrupts
	operation->interrupt |= 0b11111; //enable all interrupts
	//operation->interrupt |= (1 << 2); //port_change_interrupt_enable
	//operation->interrupt |= 0b100; //port_change_interrupt_enable

	if(config->interrupt_line >= 0 && config->interrupt_line < 16) {
		interrupt_interface->set(global_ptr(EHCI_interrupt_handler) - 14, config->interrupt_line + 32);
	}
	else {
		print(global_ptr("no interrupts, interrupt_line is %d\n"), config->interrupt_line);
	}
	
	
	operation->config = 1;
	//print_EHCI_ports();
	
	
	//operation->command |= 1; //run
}