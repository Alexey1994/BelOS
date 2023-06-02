@tcc32 -w "-Icomponents/global" -nostdlib -c "components/devices/%1/main.c" -o "bin/components/devices/%1.elf" ^
	&& ld -T COM_program_script.ld -o "bin/components/devices/%1.o" "bin/components/devices/%1.elf" ^
	&& objcopy -O binary -S "bin/components/devices/%1.o" "bin/components/devices/%1"