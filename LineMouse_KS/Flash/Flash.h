/*
 * Flash.h
 *
 *  Created on: Oct 15, 2016
 *      Author: loywong
 */

#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_

// blkIdx: 0~255
void eraseFlashBlock(int blkIdx);

void programFlashWord(unsigned int addr, unsigned int data);

void programFlash(unsigned int addr, unsigned int *data, int wordLen);

void ReadFlash(unsigned int addr, unsigned char *data, int byteLen);

#endif /* FLASH_FLASH_H_ */
