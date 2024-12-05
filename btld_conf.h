#define elem(x) ((int)(sizeof(x)/sizeof((x)[0])))
#define nil		((void*)0)

#define FAT16_MAX (0x40000)
#define	DOSPTYP_UNUSED 0x00
#define	DOSPTYP_FAT16 0x06
#define DOSPARTOFF 446
#define DOSDISKOFF 444
#define NDOSPART 4
#define DOSACTIVE 0x80
#define	DOSMBR_SIGNATURE 0xAA55
#define	BOOT_MAGIC	0xf1abde3f
#define DOSMBR_SIGNATURE_OFF 0x1fe

#define PARTSZ	16

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long