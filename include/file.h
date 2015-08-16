#ifndef file_h
#define file_h
#include <stdint.h>
#pragma pack (1)
#define FAT12 1
#define FAT16 2
typedef struct fat_boot_sector_struct {
uint8_t bootcode[3];
uint8_t oem_name[8];
uint16_t sector_size;
uint8_t cluster_size_sector;
uint16_t reserved_sector;
uint8_t fat_number;
uint16_t root_entry;
uint16_t total_sectors_16;
uint8_t media_type;
uint16_t fat_size;
uint16_t sectors_per_track;
uint16_t head_side;
uint32_t hidden_sector;
uint32_t total_sectors_32;
uint8_t bios_drive_num;
uint8_t reserved;
uint8_t boot_signature;
uint32_t volume_id;
uint8_t volume_label[11];
uint8_t fat_type_label[8];
} fat_boot_sector_t;
typedef struct directory_item_struct {
uint8_t name[8],ext[3]; /* name and extension */
//00000000(读写)
//00000001(只读)
//00000010(隐藏)
//00000100(系统)
//00001000(卷标)
//00010000(子目录)
//00100000(归档)
uint8_t attr; /* attribute bits */
uint8_t lcase; /* Case for base and extension */
uint8_t ctime_cs; /* Creation time, centiseconds (0-199) */
uint16_t ctime; /* Creation time */
uint16_t cdate; /* Creation date */
uint16_t adate; /* Last access date */
uint16_t cluster_hi; /* High 16 bits of cluster in FAT32 */
uint16_t time,date,cluster;/* time, date and first cluster */
uint32_t size; /* file size (in bytes) */
} directory_item_t;
typedef struct file_struct {
uint8_t status;
directory_item_t item;
} file_t;
int filesystem_init();
file_t file_info(char *filename);
int read_file(char *filename,uint8_t *buffer,uint32_t length);
#endif