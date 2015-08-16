#ifndef interrupt_h
#define interrupt_h
#include <stdint.h>
struct idt_struct{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t zero;      // unused, set to 0
   uint8_t type_attr; // type and attributes, see below
   uint16_t offset_2; // offset bits 16..31
};
typedef struct idt_struct idt_t;
void init_interrupt();
void write_idt_entry(unsigned int entry,void(*timer)(),uint8_t type);
void write_idt_entry_s(unsigned int entry,unsigned int selector,void(*timer)(),uint8_t type);
void empty_interrupt();
void trap();
void trap2();
void timer_interrupt();
void keyboard_interrupt();
void mouse_interrupt();
void soft_interrupt();
void enable_interrupt();
void trap_exec(int *esp);
void trap2_exec(int *esp);
uint32_t soft_interrupt_exec();
#define sti() asm("sti")
#define cli() asm("cli")
#define _S_LINE(x) #x
#define __S_LINE(x) _S_LINE(x)
#define __S_LINE__ __S_LINE(__LINE__)
#define INTERRUPT_GATE 0b1110
#define INTERRUPT_NOT_FOR_PAGING 0b10000000
#define INTERRUPT_NOT_PROTECTED 0b1100000
#define TRAP_GATE 0b1111
#define IDT_base_address ((idt_t *)0x200000)
#endif