#include "fsl_iomuxc.h"
#include "fsl_uart.h"
#include "uart.h"

#include "stdio.h"

void UART1_IRQHandler(void);

int fputc(int ch,FILE *f)
{
    UART1_SentChar((unsigned char)ch);
    
    return ch;
}

void UART1_Init(unsigned int baudRate)
{
    uart_config_t config;
  
    UART_SoftwareReset(UART1);//reset uart1
    
    //初始化IO //之前不进中断，问题就出在这
    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, 
                        IOMUXC_SW_PAD_CTL_PAD_HYS(0U)  | //滞后
                        IOMUXC_SW_PAD_CTL_PAD_PUS(1U)  | //上下拉
                        IOMUXC_SW_PAD_CTL_PAD_PUE(1U)  | //保持器选择
                        IOMUXC_SW_PAD_CTL_PAD_PKE(1U)  | //保持器使能
                        IOMUXC_SW_PAD_CTL_PAD_ODE(0U)  | //开漏输出
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U)| //速度
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U)  | //驱动能力
                        IOMUXC_SW_PAD_CTL_PAD_SRE(1U)    //压摆率
                        );
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 
                        IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PKE_MASK);
    
    SystemInstallIrqHandler(UART1_IRQn, (system_irq_handler_t)UART1_IRQHandler, NULL);

    //Configure UART
    config.baudRate_Bps=baudRate;
    config.parityMode = kUART_ParityDisabled;
    config.dataBitsCount = kUART_EightDataBits;
    config.stopBitCount = kUART_OneStopBit;
    config.txFifoWatermark = 2;
    config.rxFifoWatermark = 1;
    config.enableAutoBaudRate = false;
    config.enableTx = true;
    config.enableRx = true;
    UART_Init(UART1, &config, 80000000); //80MHz
    
    UART_EnableInterrupts(UART1,kUART_RxDataReadyEnable);// Enable interrupt
    EnableIRQ(UART1_IRQn);
}

void UART1_SentChar(unsigned char Dat)
{
    while (!UART_GetStatusFlag(UART1,kUART_TxEmptyFlag));
    UART_WriteByte(UART1, Dat);
}

void UART1_IRQHandler(void)
{
    unsigned char data;
  
    if(UART_GetStatusFlag(UART1, kUART_RxDataReadyFlag))//rx interrupt
    {
        data = UART_ReadByte(UART1);
        UART1_SentChar(data);
    }
}




