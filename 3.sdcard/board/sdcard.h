#ifndef _SDCARD_H_
#define _SDCARD_H_

void SDCARD_Init(void);
unsigned int SDCARD_ReadBlocks(unsigned char *Buffer,unsigned int StartBlock,unsigned int NumBlock);
unsigned int SDCARD_WriteBlocks(unsigned char *Buffer,unsigned int StartBlock,unsigned int NumBlock);

#endif
