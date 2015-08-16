#ifndef elf_h
#define elf_h
#include <stdint.h>
#pragma pack (1)
#define ET_NONE   0 //type
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff
#define	EI_MAG0		0 //e_ident[] indexes
#define	EI_MAG1		1
#define	EI_MAG2		2
#define	EI_MAG3		3
#define	EI_CLASS	4
#define	EI_DATA		5
#define	EI_VERSION	6
#define	EI_OSABI	7
#define	EI_PAD		8
#define	ELFMAG0		0x7f //EI_MAG
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFCLASSNONE 0 //EI_CLASS
#define	ELFCLASS32	1
#define	ELFCLASS64	2
#define	ELFCLASSNUM	3

#define ELFDATANONE	0 //e_ident[EI_DATA]
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define EV_NONE		0 //e_version, EI_VERSION
#define EV_CURRENT	1
#define EV_NUM		2

#define ELFOSABI_NONE	0
#define ELFOSABI_LINUX	3

#define MACHINE_SPARC 0x02
#define MACHINE_X86 0x03
#define MACHINE_MIPS 0x08
#define MACHINE_POWERPC 0x14
#define MACHINE_ARM 0x28
#define MACHINE_IA_64 0x32
#define MACHINE_X86_64 0x3E

/* sh_type */
#define SHT_NULL	0
#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_HASH	5
#define SHT_DYNAMIC	6
#define SHT_NOTE	7
#define SHT_NOBITS	8
#define SHT_REL		9
#define SHT_SHLIB	10
#define SHT_DYNSYM	11
#define SHT_NUM		12
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0xffffffff

/* sh_flags */
#define SHF_WRITE	0x1
#define SHF_ALLOC	0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASKPROC	0xf0000000

/* These constants define the permissions on sections in the program
   header, p_flags. */
#define PF_R		0x4
#define PF_W		0x2
#define PF_X		0x1

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */
#define PT_LOOS    0x60000000      /* OS-specific */
#define PT_HIOS    0x6fffffff      /* OS-specific */
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff
#define PT_GNU_STACK	(PT_LOOS + 0x474e551)

typedef struct elf_header_struct {
unsigned char e_ident[16];  /* ELF魔数，ELF字长，字节序，ELF文件版本等 */
	uint16_t	e_type;         /*ELF文件类型，REL, 可执行文件，共享目标文件等 */
	uint16_t	e_machine;     /* ELF的CPU平台属性 */
	uint32_t	e_version;     /* ELF版本号 */
	uint32_t	e_entry;       /* ELF程序的入口虚拟地址，REL一般没有入口地址为0 */
	uint32_t	e_phoff;      
	uint32_t	e_shoff;        /* 段表在文件中的偏移 */
	uint32_t	e_flags;       /* 用于标识ELF文件平台相关的属性 */
	uint16_t	e_ehsize;       /* 本文件头的长度 */
	uint16_t	e_phentsize;   
	uint16_t	e_phnum;
	uint16_t	e_shentsize;    /* 段表描述符的大小 */
	uint16_t	e_shnum;      /* 段表描述符的数量 */
	uint16_t	e_shstrndx;     /* 段表字符串表所在的段在段表中的下标 */
} elf_header_t;
typedef struct elf_section_table_struct {
	uint16_t	sh_name;   /* 段的名字 */
	uint16_t	sh_type;    /* 段的类型，代码段，数据段，符号表等 */
	uint16_t	sh_flags;    /* 段在进程虚拟地址空间中的属性 */
	uint32_t	sh_addr;    /* 段的虚拟地址 */
	uint32_t	sh_offset;   /* 段在文件中的偏移 */
	uint16_t	sh_size;    /* 段的长度 */
	uint16_t	sh_link;   
	uint16_t	sh_info;
	uint16_t	sh_addralign;  /* 段地址对齐 */
	uint16_t	sh_entsize;
} elf_section_table_t;
typedef struct elf_program_table_struct {
	uint32_t	p_type;
	uint32_t	p_offset;
	uint8_t 	*p_vaddr;
	uint8_t 	*p_paddr;
	uint32_t	p_filesz;
	uint32_t	p_memsz;
	uint32_t	p_flags;
	uint32_t	p_align;
} elf_program_table_t;
elf_header_t *get_elf_header(uint8_t *buffer);
typedef void (*entry_t)();
entry_t get_elf_entry(uint8_t *buffer);
int loadelf(uint8_t *buffer,uint8_t *data,uint32_t length);
#endif