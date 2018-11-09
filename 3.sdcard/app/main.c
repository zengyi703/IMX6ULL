#include "fsl_device_registers.h"
#include "board.h"
#include "timer.h"
#include "sdcard.h"
#include "uart.h"
#include "led.h"
#include "key.h"
#include "stdio.h"
#include "fsl_card.h"

extern sd_card_t g_sd;

static void delay(void)
{
    uint32_t i,j;
    for (i = 0; i < 1000000; ++i)
    {
        for(j=0; j < 100; j++)
        {
            __NOP(); /* delay */
        }
    }
}

int main(void)
{
    unsigned int i;
    unsigned char g_dataRead[512]={0};
    //unsigned char g_dataWrite[512];
    
    /* Init board hardware. */
    BOARD_Init();
    UART1_Init(115200);
    BOARD_PrintClock();
    SDCARD_Init();
    
    LED_Init();
    //KEY_Init();
    //EPIT1_Init();
    
    
    
    //for(i=0;i<512;i++)
    //{
    //    g_dataWrite[i]=i;
    //}
    
    //SDCARD_WriteBlocks(g_dataWrite,100,1);
    SDCARD_ReadBlocks(g_dataRead,0,1);
    
    printf("\r\n");
    for(i=0;i<512;i++)
    {
        if(i%16==0)
        {
            printf("\r\n");
        }  
        printf("%4x",g_dataRead[i]);
    }
    
    while (1)
    {
        LED_Toggle(1);
        delay();
    }
}
