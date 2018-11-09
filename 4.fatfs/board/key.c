#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "key.h"
#include "led.h"

void USER_KEY_IRQHandler(void);

void KEY_Init(void)
{
    gpio_pin_config_t gpio_pin_config;
    
    //USER KEY
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER0_GPIO5_IO00, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER0_GPIO5_IO00, 
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK | //滞后配置
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK | //PKE、PUE、PUS上下拉和保持器相关配置
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U)|
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U));
    
    SystemInstallIrqHandler(GPIO5_Combined_0_15_IRQn, (system_irq_handler_t)USER_KEY_IRQHandler, NULL);
    
    gpio_pin_config.direction=kGPIO_DigitalInput;
    gpio_pin_config.outputLogic=1;
    gpio_pin_config.interruptMode=kGPIO_IntRisingEdge;
    
    EnableIRQ(GPIO5_Combined_0_15_IRQn);
    GPIO_PinInit(GPIO5,0,&gpio_pin_config);
    
    GPIO_ClearPinsInterruptFlags(GPIO5, 1U << 0);// clear the interrupt status
    GPIO_EnableInterrupts(GPIO5, 1U << 0);// Enable GPIO pin interrupt
}

void USER_KEY_IRQHandler(void)
{ 
    /* clear the interrupt status */
    GPIO_ClearPinsInterruptFlags(GPIO5, 1U << 0);
    /* Change state of switch. */
    LED_Toggle(2);
}




