#include <file.h>
#include <floppy.h>
#include <sys.h>
#include <string.h>

uint32_t root_size;
uint8_t filesystem;
uint16_t cluster_size;
uint32_t FAT_address;
uint16_t sector_size;
uint32_t FAT_size;
uint16_t FAT_number;
uint32_t root_address;
uint32_t root_size;
uint32_t file_address;
uint8_t *FAT;
uint8_t *root;
int filesystem_init(){
	if(read_sector(0,0,1)){
		return 1;
	}
	fat_boot_sector_t *FAT_boot_sector = (fat_boot_sector_t *)0xA00;
	if(*((uint16_t *)(0xBFE)) != 0xAA55){
		//Not a valid filesystem
		return 2;
	}
	if(FAT_boot_sector->total_sectors_32 == 0){
		filesystem = FAT12;
	}else{
		filesystem = FAT16;
	}
	cluster_size = FAT_boot_sector->cluster_size_sector;
	sector_size = FAT_boot_sector->sector_size;
	FAT_address = (FAT_boot_sector->reserved_sector)*sector_size;
	FAT_size = (FAT_boot_sector->fat_size)*sector_size;
	FAT_number = (FAT_boot_sector->fat_number);
	root_address = FAT_address+FAT_size*FAT_number;
	root_size = (FAT_boot_sector->root_entry)*0x20;
	file_address = root_address+root_size;
	FAT = malloc(FAT_size);
	for(int i = 0;i<(FAT_size/0x400);i++){
		//Fat's size is 240 sectors in the boot floppy
		floppy_read_address(FAT_address+i*0x400);
		memcpy(FAT+i*0x400,(uint8_t *)0xA00,0x400);
	}
	root = malloc(root_size);
	for(int i = 0;i<(root_size/0x400);i++){
		//Fat's size is 16 KB in the boot floppy
		floppy_read_address(root_address+i*0x400);
		memcpy(root+i*0x400,(uint8_t *)0xA00,0x400);
	}
	return 0;
}
int cluster_read(uint32_t cluster,uint8_t *buffer,uint32_t length){
	uint32_t floppy_address = file_address+(cluster-2)*cluster_size*sector_size;
	if(cluster<2){
		return 1;
	}
	if(length>cluster_size*sector_size){
		return 2;
	}
	while(length>0x400){
		if(floppy_read_address(floppy_address)){
			return 3;
		}
		memcpy(buffer,(uint8_t *)0xA00,0x400);
		floppy_address += 0x400;
		buffer += 0x400;
		length -= 0x400;
	}
	if(length > 0){
		if(floppy_read_address(floppy_address)){
			return 3;
		}
		memcpy(buffer,(uint8_t *)0xA00,length);
	}
	return 0;
}
file_t file_info(char *filename){
	char filename_search[11];
	file_t file;
	uint8_t dot_location = 0;
	
	file.status = 0;
	for(int i = 0;i<11;i++){
		filename_search[i] = ' ';
	}
	int j = 0;
	for(int i = 0;filename[j]&&(j<12);i++){
		if(filename[j] == '.'){
			if(dot_location){
				file.status = 1;
				
				break;
			}
			if(j == 0){
				file.status = 2;
				break;
			}
			dot_location = j;
			i = 7;
			j++;
			continue;
		}
		filename_search[i] = filename[j];
		j++;
	}
	if(!file.status){
		int root_pointer = 0;
		for(;root_pointer<root_size;root_pointer+=0x20){
			if((((directory_item_t *)((uint32_t)(root+root_pointer)))->attr == 0x0F)||\
			(((directory_item_t *)((uint32_t)(root+root_pointer)))->name[0] == 0x00)||\
			(((directory_item_t *)((uint32_t)(root+root_pointer)))->name[0] == 0xE5)){
				continue;
			}
			if(memicmp((uint8_t *)(root+root_pointer),filename_search,11) == 0){
				file.item = *((directory_item_t *)(root+root_pointer));
				break;
			}
		}
		if(root_pointer == root_size){
			file.status = 3;
		}
	}
	return file;
}
uint32_t file_size(char *filename){
	file_t file;
	file = file_info(filename);
	if(file.status != 0){
		return 0;
	}
	return file.item.size;
}
uint32_t next_cluster(uint32_t cluster){
	uint32_t cluster_data;
	if(cluster < 2){
		return 0;
	}
	if(filesystem == FAT12){
		if(!(cluster%2)){
			cluster_data = FAT[(cluster/2)*3]|((FAT[(cluster/2)*3+1]&0xF)<<8);
		}else{
			cluster_data = (FAT[(cluster/2)*3+2]<<4)|((FAT[(cluster/2)*3+1]>>4)&0xF);
		}
		if(cluster_data == 0){
			return 0;
		}
		if(cluster_data == 1){
			return 0;
		}
		if(cluster_data >= 0xFF0 && cluster_data <= 0xFF7){
			return 0;
		}
		if(cluster_data >= 0xFF8 && cluster_data <= 0xFFF){
			//Finished
			return 0;
		}
		print_hex(cluster_data);
		return cluster_data;
	}else{
		return 0;
	}
}
int read_file(char *filename,uint8_t *buf,uint32_t length){
	uint32_t cluster_size_byte = cluster_size*sector_size;
	file_t file;
	file = file_info(filename);
	if(file.status){
		return file.status;
	}
	uint32_t cluster = file.item.cluster;
	do{
		if(length>cluster_size_byte){
			cluster_read(cluster,buf,cluster_size_byte);
		}else{
			cluster_read(cluster,buf,length);
			break;
		}
		buf += cluster_size_byte;
		length -= cluster_size_byte;
	}while(cluster = next_cluster(cluster));
	return 0;
}