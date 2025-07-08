#ifndef __FLASH_EEP_H_
#define __FLASH_EEP_H_


#include "main.h"

/*
移植只需修改

EEP_DATA_BLOCK_NUM
EEP_DATA_BLOCK_SIZE

EEP_INDEX_ADDR
EEP_MAX_END_ADDR
*/


#define EEP_DATA_BLOCK_NUM  (500)	//数据块数
#define EEP_DATA_BLOCK_SIZE (128)	//每个数据块的大小


#define EEP_INDEX_ADDR      (0x08070000)
#define EEP_INDEX_SIZE      (EEP_DATA_BLOCK_NUM * 2)


#define EEP_DATA_ADDR       (EEP_INDEX_ADDR + EEP_INDEX_SIZE)
#define EEP_DATA_SIZE       (EEP_DATA_BLOCK_NUM * EEP_DATA_BLOCK_SIZE)
#define EEP_DATA_END        (EEP_DATA_ADDR + EEP_DATA_SIZE)

#define EEP_PAGE_COUNT      (((EEP_INDEX_SIZE + EEP_DATA_SIZE) + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE)

#define EEP_MAX_END_ADDR    (0x08080000)


#if (EEP_DATA_END > EEP_MAX_END_ADDR)
#error "EEP_DATA_END is greater than EEP_MAX_END_ADDR"
#endif


void eep_erase_page(uint32_t addr, uint32_t page_count);

uint16_t eep_get_write_index(void);
uint8_t eep_read_data(uint16_t addr, uint8_t *data, uint16_t size);
uint8_t eep_write_data(uint16_t addr, uint8_t *data, uint16_t size);

#endif
