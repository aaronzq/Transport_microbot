/*
 * Flash.cc
 *
 *  Created on: Oct 15, 2016
 *      Author: loywong
 */

#include <ti/sysbios/knl/Task.h>

#include <inc/hw_memmap.h>
#include <inc/hw_flash.h>
#include <inc/hw_types.h>

#include "Flash.h"

// blkIdx: 0~255
void eraseFlashBlock(int blkIdx)
{
    unsigned int key;
    key = (HWREG(FLASH_BOOTCFG) & FLASH_BOOTCFG_KEY) ? 0xA4420000 : 0x71D50000;
    HWREG(FLASH_FMA) = blkIdx * 0x400;
    HWREG(FLASH_FMC) = key | FLASH_FMC_ERASE;
    while(HWREG(FLASH_FMC) & FLASH_FMC_ERASE)
        Task_sleep(1);
}

void programFlashWord(unsigned int addr, unsigned int data)
{
    unsigned int key;
    key = (HWREG(FLASH_BOOTCFG) & FLASH_BOOTCFG_KEY) ? 0xA4420000 : 0x71D50000;
    HWREG(FLASH_FMD) = data;
    HWREG(FLASH_FMA) = addr & 0xFFFFFFFC;
    HWREG(FLASH_FMC) = key | FLASH_FMC_WRITE;
    while(HWREG(FLASH_FMC) & FLASH_FMC_WRITE)
        Task_sleep(1);
}

void programFlash(unsigned int addr, unsigned int *data, int wordLen)
{
    addr &= 0xFFFFFFFC;
    for(; wordLen > 0; wordLen--)
    {
        programFlashWord(addr, *data);
        addr += 4;
        data++;
    }
}

void ReadFlash(unsigned int addr, unsigned char *data, int byteLen)
{
    union
    {
        unsigned int d;
        unsigned char c[4];
    };
    addr &= 0xFFFFFFFC;
    for(; byteLen >= 4; byteLen -= 4, addr += 4)
    {
        d = *(volatile unsigned int *)addr;
        *(data++) = c[0];
        *(data++) = c[1];
        *(data++) = c[2];
        *(data++) = c[3];
    }
    d = *(volatile unsigned int *)addr;
    for(int i = 0; byteLen > 0; byteLen--, i++)
    {
        *(data++) = c[i];
    }
}



