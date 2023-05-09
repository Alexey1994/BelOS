@tcc32 -w "-Icomponents/global" -nostdlib -c "components/drivers/%1/main.c" -o "bin/components/drivers/%1.elf" ^
	&& ld -T COM_program_script.ld -o "bin/components/drivers/%1.o" "bin/components/drivers/%1.elf" ^
	&& objcopy -O binary -S "bin/components/drivers/%1.o" "bin/components/drivers/%1"