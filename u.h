#define elem(x) ((int)(sizeof(x)/sizeof((x)[0])))
#define nil		((void*)0)
#define USED(x) if(x);

typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
typedef unsigned long	ulong;
typedef unsigned int	uint;
typedef signed char	schar;
typedef	long long	vlong;
typedef	unsigned long long uvlong;
typedef long		iptr;
typedef unsigned long	uptr;
typedef unsigned long	usize;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef char* va_list;
#define va_start(list, start) ((list) = (char*)((int*)&(start)+1))
#define va_end(list)				\
	USED(list)

#define va_arg(list, mode)			\
	((sizeof(mode) == 1) ? 			\
		((list += 4), (mode*)list)[-4]:	\
	(sizeof(mode) == 2) ?			\
		((list += 4), (mode*)list)[-2]:	\
		((list += sizeof(mode)), (mode*)list)[-1])
