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

#define BOOTSEG	0x7c0
#define BOOTRELOCSEG 0x7a0
#define MBRSTACKOFF 0xfffc
#define PARTSZ	16
#define LOADSEG (LOADADDR >> 4)
#define FATSEG 0x07e0	/* FAT table segment */

#define BOOTSTACKOFF ((BOOTSEG << 4) - 4)  /* stack starts here, grows down */
#define LFMAGIC 0x464c  /* LFMAGIC (last two bytes of \7fELF) */
#define ELFMAGIC 0x464c457f  /* ELFMAGIC ("\7fELF") */

