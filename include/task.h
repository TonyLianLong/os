#ifndef task_h
#define tash_h
#include <stdint.h>
#include <elf.h>
#define MAX_TASK_NUMBER 4095
#define TASK_EMPTY 0
#define TASK_RUN 1
#define TASK_STOP 2
#define TASK_STACK_SIZE 4096
#define DEFAULT_TASK_LIMIT_4K 64
struct TSS_struct {
	int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
	int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
	int es,cs,ss,ds,fs,gs;
	int ldtr,iomap;
};
typedef struct TSS_struct TSS_t;
struct task_struct {
	uint8_t status;
	TSS_t TSS;
	uint8_t *base;
	uint32_t limit_4k;
	uint8_t *kernel_stack;
	struct task_struct *next;
};
typedef struct task_struct task_t;
task_t *current_task;
task_t *task;
task_t *end_task;
uint8_t task_is_ready;
void load_tr(unsigned int tr);
void set_LDT(unsigned int LDT_id,unsigned int entry,unsigned int base,unsigned int limit,unsigned int mode);
void init_LDT(unsigned int LDT_id,uint8_t sys,uint32_t base,uint32_t limit);
int run_program(char *filename,uint32_t limit4k);
void init_task();
uint8_t task_status(unsigned int task_id);
void create_task(unsigned int task_id,TSS_t *TSS,unsigned int LDT_id);
void init_TSS(unsigned int task_id,TSS_t *TSS,entry_t entry,unsigned int stack,uint8_t sys,uint32_t interrupt_sp);
void _far_jmp(int eip,int cs);
void task_switch(unsigned int task_id);
void lldt(unsigned int gdt_num);
void ltr(unsigned int gdt_num);
int new_task(uint32_t base,uint32_t limit,entry_t entry,uint32_t stack);
int remove_task(uint32_t task_id);
void change_task();
#define far_jmp(cs,eip) _far_jmp(eip,cs)
#endif