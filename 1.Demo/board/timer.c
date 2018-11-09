#include "fsl_epit.h"
#include "timer.h"
#include "led.h"

/* Get source clock for EPIT driver (EPIT prescaler = 0) */
#define EXAMPLE_EPIT_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / \
                              (CLOCK_GetDiv(kCLOCK_PerclkDiv) + 1U))

void EPIT1_IRQHandler(void);

void EPIT1_Init(void)
{
    epit_config_t epitConfig;
  
    SystemInstallIrqHandler(EPIT1_IRQn, (system_irq_handler_t)(uint32_t)EPIT1_IRQHandler, NULL);

    epitConfig.clockSource = kEPIT_ClockSource_Periph;
    epitConfig.divider = 1U;
    epitConfig.enableRunInStop = true;
    epitConfig.enableRunInWait = true;
    epitConfig.enableRunInDbg = false;
    epitConfig.enableCounterOverwrite = false;
    epitConfig.enableFreeRun = false;
    epitConfig.enableResetMode = true;
    
    /* Init EPIT module */
    EPIT_Init(EPIT1, &epitConfig);
    
    /* Set timer period */
    EPIT_SetTimerPeriod(EPIT1, USEC_TO_COUNT(1000000U, EXAMPLE_EPIT_CLK_FREQ) - 1);
    EPIT_SetOutputCompareValue(EPIT1, 0);
    
    /* Enable output compare interrupts */
    EPIT_EnableInterrupts(EPIT1, kEPIT_OutputCompareInterruptEnable);
    
    /* Enable at the Interrupt */
    EnableIRQ(EPIT1_IRQn);
    
    EPIT_StartTimer(EPIT1);
}

void EPIT1_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    EPIT_ClearStatusFlags(EPIT1, kEPIT_OutputCompareFlag);
    //LED_Toggle(2);
}




