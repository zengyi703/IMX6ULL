#include "fsl_iomuxc.h"
#include "fsl_card.h"
#include "sdcard.h"
#include "stdio.h"
#include "led.h"

#define BOARD_USDHC1_CLK_FREQ (CLOCK_GetSysPfdFreq(kCLOCK_Pfd2) / (CLOCK_GetDiv(kCLOCK_Usdhc1Div) + 1U))

sd_card_t g_sd;

void GPIO_IRQ_Handler();

static void CardInformationLog(sd_card_t *card)
{
    assert(card);

    printf("Card size %d * %d bytes\r\n", card->blockCount, card->blockSize);
    printf("Working condition:\r\n");
    if (card->operationVoltage == kCARD_OperationVoltage330V)
    {
        printf("  Voltage : 3.3V\r\n");
    }
    else if (card->operationVoltage == kCARD_OperationVoltage180V)
    {
        printf("  Voltage : 1.8V\r\n");
    }

    if (card->currentTiming == kSD_TimingSDR12DefaultMode)
    {
        if (card->operationVoltage == kCARD_OperationVoltage330V)
        {
            printf("  Timing mode: Default mode\r\n");
        }
        else if (card->operationVoltage == kCARD_OperationVoltage180V)
        {
            printf("  Timing mode: SDR12 mode\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR25HighSpeedMode)
    {
        if (card->operationVoltage == kCARD_OperationVoltage180V)
        {
            printf("  Timing mode: SDR25\r\n");
        }
        else
        {
            printf("  Timing mode: High Speed\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR50Mode)
    {
        printf("  Timing mode: SDR50\r\n");
    }
    else if (card->currentTiming == kSD_TimingSDR104Mode)
    {
        printf("  Timing mode: SDR104\r\n");
    }
    else if (card->currentTiming == kSD_TimingDDR50Mode)
    {
        printf("  Timing mode: DDR50\r\n");
    }

    printf("  Freq : %d HZ\r\n", card->busClock_Hz);
}

unsigned char SDCARD_Init(void)
{
    sd_card_t *card = &g_sd;  
  
    IOMUXC_SetPinMux(IOMUXC_UART1_RTS_B_GPIO1_IO19, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RTS_B_GPIO1_IO19, 
                        IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_CLK_USDHC1_CLK, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_CLK_USDHC1_CLK, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_CMD_USDHC1_CMD, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_CMD_USDHC1_CMD, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_DATA0_USDHC1_DATA0, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_DATA0_USDHC1_DATA0, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_DATA1_USDHC1_DATA1, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_DATA1_USDHC1_DATA1, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_DATA2_USDHC1_DATA2, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_DATA2_USDHC1_DATA2, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    IOMUXC_SetPinMux(IOMUXC_SD1_DATA3_USDHC1_DATA3, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SD1_DATA3_USDHC1_DATA3, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
    
    /* Tricky here: As IRQ handler in example doesn't use parameters, just ignore them */
    SystemInstallIrqHandler(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)GPIO_IRQ_Handler, NULL);
    
    card->host.base = USDHC1;
    card->host.sourceClock_Hz = BOARD_USDHC1_CLK_FREQ;
    
    /* Init card. */
    if (SD_Init(card))
    {
        printf("\r\nSD card init failed.\r\n");
        while(1)
        {
            LED_Toggle(2);
        }
    }
    /* card information log */
    CardInformationLog(card);
    
    return 0;
}

unsigned int SDCARD_ReadBlocks(unsigned char *Buffer,unsigned int StartBlock,unsigned int NumBlock)
{
    sd_card_t *card = &g_sd;  
    
    return SD_ReadBlocks(card, Buffer, StartBlock, NumBlock);
}

unsigned int SDCARD_WriteBlocks(unsigned char *Buffer,unsigned int StartBlock,unsigned int NumBlock)
{
    sd_card_t *card = &g_sd;  
    
    return SD_WriteBlocks(card, Buffer, StartBlock, NumBlock);
}




