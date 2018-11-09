#include "fsl_device_registers.h"
#include "board.h"
#include "timer.h"
#include "uart.h"
#include "led.h"
#include "key.h"

#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"

static void led1_task(void);
static void led2_task(void);

#define led1_task_PRIORITY (configMAX_PRIORITIES - 1)
#define led2_task_PRIORITY (configMAX_PRIORITIES - 2)

void vApplicationIRQHandler(uint32_t ulICCIAR)
{
    SystemIrqHandler(ulICCIAR);
}

int main(void)
{
    /* Init board hardware. */
    BOARD_Init();
    SystemInitIrqTable();
    
    UART1_Init(115200);
    //BOARD_PrintClock();
    LED_Init();
    //KEY_Init();
    //EPIT1_Init();
    
    xTaskCreate(led1_task, "LED1_task", configMINIMAL_STACK_SIZE + 10, NULL, led1_task_PRIORITY, NULL);
    xTaskCreate(led2_task, "LED1_task", configMINIMAL_STACK_SIZE + 10, NULL, led2_task_PRIORITY, NULL);
    vTaskStartScheduler();
}

static void led1_task(void)
{
    for(;;)
    {
        LED_Toggle(1);
        vTaskDelay(500);
    }
}

static void led2_task(void)
{
    for(;;)
    {
        LED_Toggle(2);
        vTaskDelay(1000);
    }
}
