@tcc32 -w "-Icomponents/global" -nostdlib -c "components/%1/main.c" -o "bin/components/%1.elf" ^
	&& ld -T program_script.ld -o "bin/components/%1.o" "bin/components/%1.elf" ^
	&& objcopy -O binary -S "bin/components/%1.o" "bin/components/%1"