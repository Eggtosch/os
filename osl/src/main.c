void _start(const char *program, unsigned long progsize) {
	asm volatile("int $0x80" :: "a"(0x1), "b"(program), "c"(progsize) : );
	while(1);
}
