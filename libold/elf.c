#include <stdint.h>
#include <elf.h>
#include <string.h>
elf_header_t *get_elf_header(uint8_t *buffer){
	return (elf_header_t *)buffer;
}
entry_t get_elf_entry(uint8_t *buffer){
	entry_t entry = (entry_t)(((elf_header_t *)buffer)->e_entry);
	return entry;
}
int loadelf(uint8_t *buffer,uint8_t *data,uint32_t length){
	elf_header_t *header;
	elf_program_table_t *program_header_table;
	header = (elf_header_t *)buffer;
	if(!((header->e_ident[EI_MAG0] == ELFMAG0)&&(header->e_ident[EI_MAG1] == ELFMAG1)&&(header->e_ident[EI_MAG2] == ELFMAG2)&&(header->e_ident[EI_MAG3] == ELFMAG3))){
		return 1;
	}
	if(!(header->e_ident[EI_CLASS] == ELFCLASS32)){
		return 2;
	}
	if(!(header->e_ident[EI_VERSION] == 1)){
		return 3;
	}
	if(!(header->e_ident[EI_OSABI] == 0)){
		return 4;
	}
	if(!(header->e_type == ET_EXEC)){
		return 5;
	}
	if(!(header->e_machine == MACHINE_X86)){
		return 6;
	}
	if(!(header->e_version == EV_CURRENT)){
		return 7;
	}
	/*if(!(header->e_entry == 0)){
		return 8;
	}*/
	program_header_table = (elf_program_table_t *)(buffer+(header->e_phoff));
	for(int i = 0;i<(header->e_phnum);i++){
		switch(program_header_table->p_type){
			case PT_LOAD:
				if(program_header_table->p_flags){
					if((uint32_t)(program_header_table->p_vaddr+program_header_table->p_filesz) > length){
						return 8;
					}
					memcpy(data+(uint32_t)(program_header_table->p_vaddr),buffer+program_header_table->p_offset,program_header_table->p_filesz);
				}
			break;
		}
		program_header_table = (elf_program_table_t *)(((uint32_t)program_header_table)+header->e_phentsize);
	}
	return 0;
}