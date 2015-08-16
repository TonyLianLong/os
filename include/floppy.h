#ifndef floppy_h
#define floppy_h
#include <sys.h>
#define FLOPPY_DOR_REG 0x3F2
#define FLOPPY_DATA_REG 0x3F5
#define FLOPPY_STATUS_REG 0x3F4
#define FLOPPY_DIR_REG 0x3F7
#define FLOPPY_DCR_REG 0x3F7
#define FLOPPY_MOTOR_A (1<<4)
#define FLOPPY_DMA (1<<3)
#define FLOPPY_WORK (1<<2)
#define FLOPPY_SELECT_A 0b00
#define FLOPPY_500KBPS 0b00
#define FLOPPY_SET_DMA_MODE_READ() write_port(0xB,0x46)
#define FLOPPY_SET_DMA_MODE_WRITE() write_port(0xB,0x4A)
#define FLOPPY_RESET_CMD 0x7
#define FLOPPY_SEEK_CMD 0xF
#define FLOPPY_READ_CMD 0xE6
#define FLOPPY_WRITE_CMD 0xCF
#define FLOPPY_CHECK_CMD 0x8
#define FLOPPY_SPECIFY_CMD 0x3
void init_floppy();
void floppy_motor_enabled();
void floppy_interrupt();
void floppy_motor_enabled();
void floppy_move();
uint8_t read_sector(uint8_t C,uint8_t H,uint8_t S);
uint8_t floppy_read(uint8_t C,uint8_t H,uint8_t S);
uint16_t floppy_check();
void wait_for_FDC();
void floppy_reset();
void floppy_set();
void floppy_seek(uint8_t C);
void init_floppy_DMA();
uint8_t floppy_read_address(uint32_t address);
void wait_for_FDC();
void turn_on_motor();
void turn_off_motor();
#endif