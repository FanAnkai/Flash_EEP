

#include "flash_eep.h"
#include <string.h>


uint8_t eep_backup_data[EEP_DATA_SIZE];


// 写入当前写入索引
uint8_t eep_write_index(uint16_t index)
{
    HAL_StatusTypeDef res = HAL_ERROR;

    HAL_FLASH_Unlock();
    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, EEP_INDEX_ADDR + index * 2, 0x0000);
    HAL_FLASH_Lock();

    if(res != HAL_OK)
    {
        return 0;
    }
    return 1;
}

// 获取当前写入索引
uint16_t eep_get_write_index(void)
{
    uint16_t *ptr = (uint16_t *)EEP_INDEX_ADDR;

    for (uint16_t i = 0; i < EEP_DATA_BLOCK_NUM; i++)
    {
        if (ptr[i] == 0xFFFF)
        {
            return i;
        }
    }
    return EEP_DATA_BLOCK_NUM;
}

void eep_erase_page(uint32_t addr, uint32_t page_count)
{
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase;
    uint32_t error;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = addr;
    erase.NbPages = page_count;
    HAL_FLASHEx_Erase(&erase, &error);
    HAL_FLASH_Lock();
}


static void eep_write_block(uint16_t index, const uint8_t *data)
{
    HAL_FLASH_Unlock();
    uint32_t base = EEP_DATA_ADDR + index * EEP_DATA_BLOCK_SIZE;
    for (uint8_t i = 0; i < EEP_DATA_BLOCK_SIZE; i += 4)
    {
        uint32_t word;
        memcpy(&word, data + i, 4);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + i, word);
    }
    HAL_FLASH_Lock();
}


uint8_t eep_read_data(uint16_t addr, uint8_t *data, uint16_t size)
{
    if (!data || (addr + size) > EEP_DATA_BLOCK_SIZE) return 1;

    uint16_t index = eep_get_write_index();
    if (index == 0)
    {
        return 2;
    }
    
     // 上一个有效数据块
    index--;
    
    uint8_t *base = (uint8_t *)(EEP_DATA_ADDR + index * EEP_DATA_BLOCK_SIZE);
    memcpy(data, base + addr, size);
    return 0;
}

uint8_t eep_write_data(uint16_t addr, uint8_t *data, uint16_t size)
{
    if (!data || (addr + size) > EEP_DATA_BLOCK_SIZE) return 1;

    uint16_t index = eep_get_write_index();

    if (index == 0)
    {
        memset(eep_backup_data, 0xFF, EEP_DATA_BLOCK_SIZE);
    }
    else
    {
        uint32_t old_addr = EEP_DATA_ADDR + (index - 1) * EEP_DATA_BLOCK_SIZE;
        memcpy(eep_backup_data, (uint8_t *)old_addr, EEP_DATA_BLOCK_SIZE);
    }

    memcpy(eep_backup_data + addr, data, size);

    // 如果写满了，擦除并从头开始
    if (index >= EEP_DATA_BLOCK_NUM)
    {
        eep_erase_page(EEP_INDEX_ADDR, EEP_PAGE_COUNT);
        index = 0;
    }

    eep_write_block(index, eep_backup_data);
    eep_write_index(index);
    return 0;
}

