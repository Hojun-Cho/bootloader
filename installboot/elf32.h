#include <elf.h>

/* e_ident */
#define IS_ELF(ehdr) ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
                      (ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
                      (ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
                      (ehdr).e_ident[EI_MAG3] == ELFMAG3)

typedef Elf32_Ehdr Ehdr;
typedef Elf32_Phdr Ephdr;
typedef Elf32_Shdr Eshdr;
typedef Elf32_Sym Esym;

static Ehdr
readEhdr(int fd)
{
	Ehdr h;	

	assert(pread(fd, &h, sizeof(h), 0) == h.e_ehsize);
	assert(IS_ELF(h) == 1);	
	return h;
}

static Ephdr
readEphdr(int fd, Ehdr h, int i)
{
	Ephdr ph;
	int off;

	off = h.e_phoff+i*h.e_phentsize;
	assert(pread(fd, &ph, h.e_phentsize, off)
			== sizeof(Ephdr));
	return ph;
}

static Eshdr
readEshdr(int fd, Ehdr h, int i)
{
	Eshdr sh;
	int off;

	off = h.e_shoff + i*h.e_shentsize;
	assert(pread(fd, &sh, h.e_shentsize, off) == sizeof(sh));
	return sh;
}

static void*
readEcode(int fd, Ehdr h, Ephdr ph, void *dst)
{
	if(dst == NULL)
		assert((dst = calloc(1, ph.p_filesz)) != NULL);
	assert(pread(fd, dst, ph.p_filesz, ph.p_offset) ==
			ph.p_filesz);
	return dst;
}

static void*
readsymtab(int fd, Eshdr sh, void *dst)
{
	if(dst == NULL)
		assert((dst = calloc(1, sh.sh_size)) != NULL);
	assert(pread(fd, dst, sh.sh_size, sh.sh_offset)
			== sh.sh_size);
	return dst;
}
