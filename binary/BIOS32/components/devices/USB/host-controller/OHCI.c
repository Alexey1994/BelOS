typedef struct
{
	//hardware
	Number32 interrupts[32];
	Number16 frame_number;
	Number16 pad1;
	Number32 done_head;
	//Byte     reserved[120];
/*
	//software
	OHCI_Endpoint endpoints[63];
	OHCI_Endpoint control_endpoint;
	OHCI_Endpoint bulk_endpoint;
	Number32      mmio_base;
	Number32      powered_up;
	Number32      done_list;
	Number32      done_list_end_address;
	Number32      EHCI_companion;*/
}
OHCI_HCCA; //host controller communication area


typedef enum
{
	OHCI_HOST_CONTROLLER_RESET = 1,
	OHCI_CONTROL_LIST_FILLED = 2,
	OHCI_BULK_LIST_FILLED = 4
}
OHCI_Command_Status;


typedef enum
{
	OHCI_INTERRUPT_MASTER_ENABLE = 0x80000000
}
OHCI_Interrupt;


typedef struct
{
	/*struct
	{
		Number32 function_address: 7;
		Number32 endpoint_number: 4;
		Number32 direction: 2; //(1 - out, 2 - in), если 0 или 3 то определено в PID transfer descriptor
		Number32 speed: 1; //0 - full speed, 1 - low speed
		Number32 sKip: 1;
		Number32 format: 1; //0 for control, bulk, interrupt, 1 for isochronous endpoints
		Number32 maximum_packet_size: 11;
		Number32 reserved: 5;
	}
	flags;*/
	Number32 flags;
	Number32 tail;
	Number32 head;
	Number32 next;
}
OHCI_Endpoint;


typedef struct
{
	/*struct
	{
		Number32 reserved: 18;
		Number32 buffer_rounding: 1;
		Number32 direction_PID: 2;
		Number32 delay_interrupt: 3;
		Number32 data_toggle: 2;
		Number32 error_count: 2;
		Number32 condition_code: 4;
	}
	flags;*/
	Number32 flags;
	Number32 first_byte_address;
	Number32 next;
	Number32 last_byte_address;
}
OHCI_Transfer;


typedef struct
{
	Number32 flags;
	Number32 first_byte_address;
	Number32 next;
	Number32 last_byte_address;
	Number16 offset[8];
}
OHCI_Isochronous_Transfer;

/*
typedef enum
{
	OHCI_HOST_RESET = 0,
	OHCI_HOST_RESUME = 1,
	OHCI_HOST_OPERATIONAL = 2,
	OHCI_HOST_SUSPEND = 3
}
OHCI_Command_Status;*/


#define OHCI_FRAME_INTERVAL 0x2EDF
#define OHCI_DEFAULT_FRAME_INTERVAL ((((6 * (OHCI_FRAME_INTERVAL - 210)) / 7) << 16) | OHCI_FRAME_INTERVAL)

#define OHCI_PERIODIC_START 0x00002A27


#define OHCI_HC_STATE_RESET       0
#define OHCI_HC_STATE_RESUME      1
#define OHCI_HC_STATE_OPERATIONAL 2
#define OHCI_HC_STATE_SUSPEND     3


typedef struct
{
	//control and status
	Number32 revision;
	/*struct
	{
		Number32 control_bulk_service_ratio: 2;
		Number32 periodic_list_enable: 1;
		Number32 isochronus_enable: 1;
		Number32 control_list_enable: 1;
		Number32 bulk_list_enable: 1;
		Number32 host_controller_functional_state: 2;
		Number32 interrupt_routing: 1;
		Number32 remoute_wakeup_connected: 1;
		Number32 remote_wakeup_enable: 1;
		Number32 reserved: 21;
	}*/
	Number32 control;
	struct
	{
		Number32 host_controller_reset: 1;
		Number32 control_list_filled: 1;
		Number32 bulk_list_filled: 1;
		Number32 ownership_change_request: 1;
		Number32 reserved1: 12;
		Number32 scheduling_overrun_count: 1;
		Number32 reserved2: 15;
	}
	command_status;
	struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 reserved2: 1;
	}
	interrupt_status;
	/*struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;
	}*/
	Number32 interrupt_enable;
	/*struct
	{
		Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;
	}*/
	Number32 interrupt_disable;

	//memory pointer
	Number32 HCCA;
	Number32 PeriodCurrentED;
	Number32 ControlHeadED;
	Number32 ControlCurrentED;
	Number32 BulkHeadED;
	Number32 BulkCurrentED;
	Number32 DoneHead;

	//frame counter
	/*struct
	{
		Number32 interval: 14;
		Number32 reserved: 2;
		Number32 largest_data_packet: 15;
		Number32 toggle: 1;
	}*/
	Number32 frame_interval;
	Number32 frame_remaining;
	Number32 frame_number;
	Number32 PeriodicStart;
	Number32 LSThreshold;

	//root hub
	struct
	{
		Number32 number_of_downstream_ports: 8;
		Number32 power_switching_mode: 1;
		Number32 no_power_switching: 1;
		Number32 device_type: 1;
		Number32 over_current_protection_mode: 1;
		Number32 over_no_current_protection: 1;
		Number32 reserved: 11;
		Number32 power_on_to_power_good_time: 8;
	}
	root_hub_descriptor_A; //status?
	Number32 root_hub_descriptor_B;
	/*union
	{
		struct
		{
			Number32 local_power_status: 1;
			Number32 over_current_indicator: 1;
			Number32 reserved10: 13;
			Number32 device_remote_wakeup_enable: 1;
			Number32 local_power_sttus_change: 1;
			Number32 over_current_indicator_change_R: 1;
			Number32 reserved20: 14;
		};
		struct
		{
			Number32 clear_global_power: 1;
			Number32 reserved11: 14;
			Number32 start_remote_wakeup_enable: 1;
			Number32 set_global_power: 1;
			Number32 over_current_indicator_changeW: 1;
			Number32 reserved22: 13;
			Number32 clear_remote_wakeup_enable: 1;
		};
	}*/
	Number32 root_hub_status;

	/*
	struct
	{
		union
		{
			struct // read
			{
				USHORT  CurrentConnectStatus     : 1;
				USHORT  PortEnableStatus         : 1;
				USHORT  PortSuspendStatus        : 1;
				USHORT  PortOverCurrentIndicator : 1;
				USHORT  PortResetStatus          : 1;
				USHORT  Reserved1r               : 3;
				USHORT  PortPowerStatus          : 1;
				USHORT  LowSpeedDeviceAttached   : 1;
				USHORT  Reserved2r               : 6;
			};
			struct // write
			{
				USHORT  ClearPortEnable    : 1;
				USHORT  SetPortEnable      : 1;
				USHORT  SetPortSuspend     : 1;
				USHORT  ClearSuspendStatus : 1;
				USHORT  SetPortReset       : 1;
				USHORT  Reserved1w         : 3;
				USHORT  SetPortPower       : 1;
				USHORT  ClearPortPower     : 1;
				USHORT  Reserved2w         : 6;
			};
		};
		USHORT ConnectStatusChange            : 1;
		USHORT PortEnableStatusChange         : 1;
		USHORT PortSuspendStatusChange        : 1;
		USHORT PortOverCurrentIndicatorChange : 1;
		USHORT PortResetStatusChange          : 1;
		USHORT Reserved3                      : 11;
	};*/
	Number32 port_statuses[15]; //1 - порт подключен
}
OHCI_Interface;


OHCI_Interface* _interface;
OHCI_HCCA*      _hcca;


asm("pusha");
asm("call . + 13");
asm("mov $0x20, %al");
asm("out %al, $0xA0");
asm("out %al, $0x20");
asm("popa");
asm("iret");
void OHCI_interrupt_handler()
{
	get_module_address_by_function(OHCI_interrupt_handler);
	OHCI_Interface* interface = global(_interface);
	OHCI_HCCA* hcca = global(_hcca);
	
	
	print(global_ptr("OHCI interrupt\n"));
	
	/*if(operation->status & (1 << 0)) {
		print(global_ptr("transfer\n"));
	}
	if(operation->status & (1 << 1)) {
		print(global_ptr("error\n"));
	}
	if(operation->status & (1 << 2)) {
		print(global_ptr("port_change_detect\n"));
	}
	if(operation->status & (1 << 4)) {
		print(global_ptr("host_system_error\n"));
	}
	if(operation->status & (1 << 5)) {
		print(global_ptr("doorbell_interrupt\n"));
	}*/
	
	//if(operation->status & (1 << 2)) { //port_change_detect
		Number i;
		Number number_of_ports;
		
		number_of_ports = interface->root_hub_descriptor_A.number_of_downstream_ports;
		//print("%d downstream ports\n", interface->root_hub_descriptor_A.number_of_downstream_ports);
		
		for(i = 0; i < number_of_ports; ++i) {
			//print("port %d status %d\n", i, operation->port_status_and_control[i]);
			print(global_ptr("port %d status"), i);
			
			Number32 port_status = interface->port_statuses[i];
			
			/*
				USHORT  CurrentConnectStatus     : 1;
				USHORT  PortEnableStatus         : 1;
				USHORT  PortSuspendStatus        : 1;
				USHORT  PortOverCurrentIndicator : 1;
				USHORT  PortResetStatus          : 1;
				USHORT  Reserved1r               : 3;
				USHORT  PortPowerStatus          : 1;
				USHORT  LowSpeedDeviceAttached   : 1;
				USHORT  Reserved2r               : 6;*/
			
			if(port_status & 1) {
				print(global_ptr(" connected"));
			}
			
			if(port_status & (1 << 1)) {
				print(global_ptr(" enabled"));
			}
			
			if(port_status & (1 << 2)) {
				print(global_ptr(" suspend"));
			}
			
			if(port_status & (1 << 3)) {
				print(global_ptr(" over_current"));
			}
			
			if(port_status & (1 << 4)) {
				print(global_ptr(" reset"));
			}
			
			print("\n");
		}
	//}
	
	print(global_ptr("frame index: %d\n"), hcca->frame_number);
}


void configure_USB_OHCI(Number16 addresss, PCI_Config* config)
{
	get_module_address_by_function(configure_USB_OHCI);
	Interrupt_Interface* interrupt_interface = global(_interrupt_interface);
	PCI_Interface* pci_interface = global(_pci_interface);
	OHCI_Interface* interface = global(_interface);
	OHCI_HCCA* hcca = global(_hcca);
	
	
	interface = pci_interface->get_PCI_MMIO_address(config, 0);
	hcca = interface->HCCA;
	
	print(global_ptr("USB %x\n"), interface->revision);
	
	if(config->interrupt_line >= 0 && config->interrupt_line < 16) {
		interrupt_interface->set(global_ptr(OHCI_interrupt_handler) - 14, config->interrupt_line + 32);
	}
	else {
		print(global_ptr("no interrupts, interrupt_line is %d\n"), config->interrupt_line);
	}
	
		/*Number32 scheduling_overrun: 1;
		Number32 writeback_done_head: 1;
		Number32 start_of_frame: 1;
		Number32 resume_detected: 1;
		Number32 untercoverable_error: 1;
		Number32 frame_number_overflow: 1;
		Number32 root_hub_status_change: 1;
		Number32 reserved1: 23;
		Number32 ownership_change: 1;
		Number32 master_interrupt_enable: 1;*/
	
	interface->interrupt_enable = (1 << 31) | (1 << 6);//(1 << 31) | (1 << 1) | (1 << 6);//0xFFFFFFFF;//0x80000042;
	interface->interrupt_disable = 0x00000000;
}