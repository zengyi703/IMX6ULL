#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "led.h"

void LED_Init(void)
{
    gpio_pin_config_t gpio_pin_config;
    
    //LED D12
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 
                        IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U));
                          
    //LED D13
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER2_GPIO5_IO02, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER2_GPIO5_IO02, 
                        IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U));
    
    //LED IO口为高电平时关闭LED
    gpio_pin_config.direction=kGPIO_DigitalOutput;
    gpio_pin_config.outputLogic=1;
    gpio_pin_config.interruptMode=kGPIO_NoIntmode;
    
    GPIO_PinInit(GPIO5,1,&gpio_pin_config);
    GPIO_PinInit(GPIO5,2,&gpio_pin_config);
}

void LED_On(unsigned char led)
{
    if(led==1)
    {
        GPIO_WritePinOutput(GPIO5,1,0);
    }
    if(led==2)
    {
        GPIO_WritePinOutput(GPIO5,2,0);
    }
}

void LED_Off(unsigned char led)
{
    if(led==1)
    {
        GPIO_WritePinOutput(GPIO5,1,1);
    }
    if(led==2)
    {
        GPIO_WritePinOutput(GPIO5,2,1);
    }
}

void LED_Toggle(unsigned char led)
{
    if(led==1)
    {
        GPIO_WritePinOutput(GPIO5,1,!GPIO_ReadPinInput(GPIO5,1));
    }
    if(led==2)
    {
        GPIO_WritePinOutput(GPIO5,2,!GPIO_ReadPinInput(GPIO5,2));
    }
}





