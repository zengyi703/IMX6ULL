#include "fsl_device_registers.h"
#include "board.h"
#include "timer.h"
#include "uart.h"
#include "led.h"
#include "key.h"

#include "stdio.h"

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
    /* Init board hardware. */
    BOARD_Init();
    UART1_Init(115200);
    BOARD_PrintClock();
    LED_Init();
    KEY_Init();
    
    EPIT1_Init();
    
    while (1)
    {
        LED_Toggle(1);
        delay();
    }
}
