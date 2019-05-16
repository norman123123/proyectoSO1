#include <string.h>
#include <libgen.h>

#include "cr_API.h"
#include "library.h"

// typedef struct crFILE{
// 	//FIXME
// 	FILE* cursor;
// 	FILE* root;
// 	unsigned int* offset;
// 	int block;
// 	int size;
// 	unsigned char name[12];
// 	unsigned char data[1021 * 2048];

// } crFILE;

// typedef struct blockIndex {
// 	int block_number;
// 	int byte_number;
// 	int bit_number;
// 	unsigned char new_byte[8];
// } blockIndex;


char* dirfinder(char* path) {
	char *dirc, *dname;
	dirc = strdup(path);
	dname = dirname(dirc);
	return dname;
}


char* basefinder(char* path) {
	char *basec, *bname;
	basec = strdup(path);
	bname = basename(basec);
	return bname;
}


blockIndex* find_empty_block(){

	int encontrado = 0;
	unsigned int block_num = 0;
	unsigned char * buffer = malloc( sizeof( unsigned char ) * 2048*4);
	unsigned char mask = 1;
	blockIndex* block = malloc(sizeof(blockIndex));

	FILE* file = fopen(disk_path, "rb");
	fseek(file, 2048, SEEK_SET);
	fread(buffer, 1, 2048*4, file);

	unsigned char * byte = malloc(sizeof(char) * 8);
	for (int k = 0; k < 2048*4; k++) {
		for(int j = 0; j<8; j++){

			block_num++;
			byte[j] = (buffer[k] & (mask << j)) != 0;

			if (byte[j] == '0') {

				encontrado = 1;
				block->block_number = block_num;
				block->byte_number = k;
				block->bit_number = j;

				memcpy(block->new_byte, byte, 8);
				// strcpy(block->new_byte, byte);
				block->new_byte[j] = '1';
				break;
			}
		}
		if (encontrado){
			break;
		}
	}
	free(byte);
	free(buffer);
	fclose(file);
	return block;
}


void change_bitmap(blockIndex* block){

	unsigned char old_byte[8];
	unsigned char new_byte[8];

	unsigned int offset;

	FILE* file = fopen(disk_path, "w+b");
	offset = 2048 * (block->block_number + 1) + block->byte_number;

	fseek(file, offset , SEEK_SET);
	fwrite(block->new_byte, 1, 1, file);

	fclose(file);

}

char * itoa (int value, char *result, int base){
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}
