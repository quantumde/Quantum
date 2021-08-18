/*
*  kernel.c
*/
void kmain(void)
{
	const char *str = "quantum a-unix system";
	char *vidptr = (char*)0xb8000; 	//видео пямять начинается здесь
	unsigned int i = 0;
	unsigned int j = 0;

	/* этот цикл очищает экран*/
	while(j < 80 * 25 * 2) {
		/* пустой символ */
		vidptr[j] = ' ';
		/* байт атрибутов */
		vidptr[j+1] = 0x07; 		
		j = j + 2;
	}

	j = 0;

	/* в этом цикле строка записывается в видео память */
	while(str[j] != '\0') {
		/* ascii отображение */
		vidptr[i] = str[j];
		vidptr[i+1] = 0x07;
		++j;
		i = i + 2;
	}
	return;
}
struct IDT_entry{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void)
{
    unsigned long keyboard_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    keyboard_address = (unsigned long)keyboard_handler; 
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
    IDT[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

    write_port(0x20 , 0x11);
    write_port(0xA0 , 0x11);

    write_port(0x21 , 0x20);
    write_port(0xA1 , 0x28);

    write_port(0x21 , 0x00);  
    write_port(0xA1 , 0x00);  

    write_port(0x21 , 0x01);
    write_port(0xA1 , 0x01);

    write_port(0x21 , 0xff);
    write_port(0xA1 , 0xff);

    idt_address = (unsigned long)IDT ;
    idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16 ;

    load_idt(idt_ptr);
}
void kb_init(void)
{
    write_port(0x21 , 0xFD);
}
void keyboard_handler_main(void) {
    unsigned char status;
    char keycode;

    /* Пишем EOI */
    write_port(0x20, 0x20);

    status = read_port(KEYBOARD_STATUS_PORT);
    /* Нижний бит статуса будет выставлен, если буфер не пуст */
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);
        if(keycode < 0)
            return;
        vidptr[current_loc++] = keyboard_map[keycode];
        vidptr[current_loc++] = 0x07;   
    }
}