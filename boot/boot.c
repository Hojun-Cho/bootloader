typedef int dev_t;

#define SCREEN_WIDTH           80
#define SCREEN_LENGTH          25
#define VGA_BUFFER_ADDR         0xb8000

char*
memcpy(char *a, char *b, int l)
{
	for(int i = 0; i < l; ++i)
		a[i] = b[i];
	return a;
}

int
strlen(char *s)
{
	int i = 0;
	for(; s[i]; ++i)
		;
	return i;
}

char*
itostr(int n, char *s)
{
	char buf[16];	
	char *p = buf;

	do {
		*p++ = n%16;
	}while((n/=16) != 0);

	while(p > buf){
		*s++ = "0123456789abcdef"[(int)*--p];
	}
	return s;
}

void
boot(dev_t bootdev)
{
	char *p;
    char s[SCREEN_WIDTH] = "booted on disk:0x";
    short* vga = (short*)VGA_BUFFER_ADDR;

	p = s+strlen(s);
	p = itostr((int)bootdev, p);
	for(int j = 0; j < SCREEN_LENGTH; ++j){
		for (int i = 0; i < sizeof(s); ++i) {
			vga[(j*SCREEN_WIDTH)  + (i + SCREEN_WIDTH*2)] = 0x9100 | s[i];
		}
	}
	while(1)
		;
}
