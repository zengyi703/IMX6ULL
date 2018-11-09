#include "fsl_device_registers.h"
#include "sdcard.h"
#include "board.h"
#include "timer.h"
#include "uart.h"
#include "led.h"
#include "key.h"
#include "stdio.h"
#include "fsl_card.h"
#include "ff.h"
#include "diskio.h"

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

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
    char temp[100]={0};
    FRESULT error;
    unsigned int i;
    
    /* Init board hardware. */
    BOARD_Init();
    UART1_Init(115200);
    BOARD_PrintClock();
    SDCARD_Init();
    LED_Init();
    
    if (f_mount(&g_fileSystem, "2:", 1U))
    {
        printf("Mount volume failed.\r\n");
        return -1;
    }
    
    error = f_open(&g_fileObject, "2:/test.txt", FA_READ|FA_OPEN_EXISTING);
    if (error)
    {
        if (error == FR_EXIST)
        {
            printf("File exists.\r\n");
        }
        else
        {
            printf("Open file failed.\r\n");
            return -1;
        }
    }
    
    i=10;
    error = f_read(&g_fileObject, temp, sizeof(temp), &i);
    
    printf("%s",temp);
    
    while (1)
    {
        LED_Toggle(1);
        delay();
    }
}
