#ifndef sys_h
#define sys_h
#include <stdint.h>
#include <stdbool.h>
#include <console.h>
#pragma pack (1)
#define printk(str) print_string(str)
#define NULL (void *)0
#define MEM_TABLE_ADDRESS 0x300900
#define MEM_USEABLE_ADDRESS 0x400000
#define MEM_LIST_ADDRESS 0x300800
#define MEM_LIST_LENGTH 0x100
#define GDT_ADDRESS 0x302C00
#define SEG_32 (1<<14)
#define SEG_C 0x98
#define SEG_DRW 0x92
#define SEG_G (1<<15)
#define SEG_TSS_GATE 0x89
#define SEG_LDT 0x82
#define SEG_RING3 0x60
#define GDT_USEABLE_NUM 3
#define LDT_BASE_ADDRESS 0x303C00
#define LDT_SIZE 0x10
#define _BV(n) (0x01U << (n))
#define _LV(n) (~_BV(n))
#define bitRead(x, n) ((x) & _BV(n))
#define bitSet(x, n) ((x) |= _BV(n))
#define bitClear(x, n) ((x) &= _LV(n))
#define bitWrite(x, n, b) ((b)? bitSet((x),(n)) : bitClear((x), (n)))
#define backup_eflags() int eflags = get_eflags()
#define restore_eflags() set_eflags(eflags)
struct mem_list_struct {
uint32_t address;
uint32_t size_in_4k;
};
typedef struct mem_list_struct mem_list_t;
struct gdt_struct {
	uint16_t size;
	uint32_t offset;
};
typedef struct gdt_struct gdt_t;
uint8_t *mem_table;
uint32_t mem_table_size;
uint32_t mem_size;
mem_list_t *mem_list;
unsigned int kernel_code_selector;
unsigned int kernel_data_selector;
uint8_t read_port(uint16_t port);
void write_port(uint16_t port,uint8_t data);
int get_eflags();
void set_eflags();
bool is386();
void disable_cache();
void enable_cache();
unsigned int get_memory_size();
void mem_init();
void *malloc(uint32_t size);
void free(void *address);
void set_gdt(unsigned int entry,unsigned int base,unsigned int limit,unsigned int mode);
void init_gdt();
#endif