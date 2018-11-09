#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "board.h"
#include "stdio.h"

static const mmu_attribute_t s_mmuDevAttr = {.type = MMU_MemoryDevice,
                                             .domain = 0U,
                                             .accessPerm = MMU_AccessRWNA,
                                             .shareable = 0U,
                                             .notSecure = 0U,
                                             .notGlob = 0U,
                                             .notExec = 1U};

static const mmu_attribute_t s_mmuRomAttr = {.type = MMU_MemoryWriteBackWriteAllocate,
                                             .domain = 0U,
                                             .accessPerm = MMU_AccessRORO,
                                             .shareable = 0U,
                                             .notSecure = 0U,
                                             .notGlob = 0U,
                                             .notExec = 0U};

static const mmu_attribute_t s_mmuRamAttr = {.type = MMU_MemoryWriteBackWriteAllocate,
                                             .domain = 0U,
                                             .accessPerm = MMU_AccessRWRW,
                                             .shareable = 0U,
                                             .notSecure = 0U,
                                             .notGlob = 0U,
                                             .notExec = 0U};

static const mmu_attribute_t s_mmuBufferAttr = {.type = MMU_MemoryNonCacheable,
                                                .domain = 0U,
                                                .accessPerm = MMU_AccessRWRW,
                                                .shareable = 0U,
                                                .notSecure = 0U,
                                                .notGlob = 0U,
                                                .notExec = 0U};

//Freq = Osc*Div/2
const clock_arm_pll_config_t g_ccmConfigArmPll_1056M = {.loopDivider = 88U};
const clock_arm_pll_config_t g_ccmConfigArmPll_792M = {.loopDivider = 66U};
const clock_arm_pll_config_t g_ccmConfigArmPll_900M = {.loopDivider = 75U};

/* System clock frequency. */
extern uint32_t SystemCoreClock;

#pragma data_alignment = 16384
static uint32_t MMU_L1Table[4096] @"OcramData";

void BOARD_Init(void)
{
    BOARD_BootClockRUN();
    BOARD_InitMemory();
    SystemInitIrqTable();
}

static void BOARD_BootClockGate(void)
{
    /* Disable all unused peripheral clock */
    CCM->CCGR0 = 0x00C0000FU;
    CCM->CCGR1 = 0xF00C0000U;
    CCM->CCGR2 = 0x003F0030U;
    CCM->CCGR3 = 0xFF3CC300U;
    CCM->CCGR4 = 0x0000F3FCU;
    CCM->CCGR5 = 0x00330033U;
    CCM->CCGR6 = 0x00CC0300U;
}

void BOARD_BootClockRUN(void)
{
    /* Boot ROM did initialize the XTAL, here we only sets external XTAL OSC freq */
    CLOCK_SetXtalFreq(24000000U);
    CLOCK_SetRtcXtalFreq(32768U);

    /* Switch CPU off ARM PLL */
    if (CLOCK_GetMux(kCLOCK_Pll1SwMux) == 0) /* CPU runs on ARM PLL */
    {
        CLOCK_SetMux(kCLOCK_StepMux, 0);   /* Set Step MUX to OSC */
        CLOCK_SetMux(kCLOCK_Pll1SwMux, 1); /* Let CPU run on Step MUX */
    }
    
    /*Set arm pll freq to 1056/2=528M*/
    CLOCK_InitArmPll(&g_ccmConfigArmPll_1056M); /* Configure ARM PLL to 1056M */
    CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);          /* Now CPU runs again on ARM PLL*/
    CLOCK_SetDiv(kCLOCK_ArmDiv, 1);             /* Configure ARM clock root with divide 2 */
    /*Set arm pll freq to 900/1=900M*/
    //CLOCK_InitArmPll(&g_ccmConfigArmPll_900M); /* Configure ARM PLL to 900M */
    //CLOCK_SetMux(kCLOCK_Pll1SwMux, 0);         /* Now CPU runs again on ARM PLL*/
    //CLOCK_SetDiv(kCLOCK_ArmDiv, 0);            /* Configure ARM clock root with divide 1 */

    /* Disable unused clock */
    BOARD_BootClockGate();

    /* Power down all unused PLL */
    CLOCK_DeinitUsb2Pll();
    CLOCK_DeinitAudioPll();
    CLOCK_DeinitVideoPll();
    CLOCK_DeinitEnetPll();
    
    //Configure UART divider to default
    CLOCK_SetMux(kCLOCK_UartMux, 0); //Set UART source to PLL3 80M
    CLOCK_SetDiv(kCLOCK_UartDiv, 0); //Set UART divider to 1

    CLOCK_SetMux(kCLOCK_PerclkMux, 0); /* Set I2C/EPIT source to IPG 66M */
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0); /* Set I2C/EPIT divider to 1 */

    CLOCK_SetMux(kCLOCK_PerclkMux, 0);
    CLOCK_SetDiv(kCLOCK_Usdhc1Div, 0U);
    
    /* Update core clock */
    SystemCoreClockUpdate();
}

/* Initialize memory system (MMU). */
void BOARD_InitMemory(void)
{
    uint32_t i;

    MMU_Init(MMU_L1Table);

    MMU_ConfigSection(MMU_L1Table, (const void *)0x00000000U, 0x00000000U, &s_mmuRomAttr); /* ROM */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00900000U, 0x00900000U, &s_mmuRamAttr); /* OCRAM */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00A00000U, 0x00A00000U, &s_mmuDevAttr); /* GIC */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00B00000U, 0x00B00000U, &s_mmuDevAttr); /* GPV_0 PL301 */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00C00000U, 0x00C00000U, &s_mmuDevAttr); /* GPV_1 PL301 */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00D00000U, 0x00D00000U, &s_mmuDevAttr); /* cpu */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x00E00000U, 0x00E00000U, &s_mmuDevAttr); /* per_m */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x01800000U, 0x01800000U, &s_mmuDevAttr); /* APBH DMA */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x02000000U, 0x02000000U, &s_mmuDevAttr); /* AIPS-1 */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x02100000U, 0x02100000U, &s_mmuDevAttr); /* AIPS-2 */
    MMU_ConfigSection(MMU_L1Table, (const void *)0x02200000U, 0x02200000U, &s_mmuDevAttr); /* AIPS-3 */

    for (i = 0; i < 32; i++)
    {
        MMU_ConfigSection(MMU_L1Table, (const void *)(0x0C000000U + (i << 20)), (0x0C000000U + (i << 20)),
                          &s_mmuDevAttr); /* QSPI Rx Buf */
    }

    for (i = 0; i < 256; i++)
    {
        MMU_ConfigSection(MMU_L1Table, (const void *)(0x50000000U + (i << 20)), (0x50000000U + (i << 20)),
                          &s_mmuRamAttr); /* EIM */
    }

    for (i = 0; i < 256; i++)
    {
        MMU_ConfigSection(MMU_L1Table, (const void *)(0x60000000U + (i << 20)), (0x60000000U + (i << 20)),
                          &s_mmuRomAttr); /* QSPI */
    }

    for (i = 0; i < 2048; i++)
    {
        MMU_ConfigSection(MMU_L1Table, (const void *)(0x80000000U + (i << 20)), (0x80000000U + (i << 20)),
                          &s_mmuRamAttr); /* DDR */
    }

/* You can place global or static variables in NonCacheable section to make it uncacheable.*/
#pragma section = "NonCacheable"
    uint32_t ncahceStart = (uint32_t)__section_begin("NonCacheable");
    uint32_t size = (uint32_t)__section_size("NonCacheable");

    size = (size + 0xFFFFFU) & (~0xFFFFFU);

    for (i = 0; i < ((size) >> 20); i++)
    {
        MMU_ConfigSection(MMU_L1Table, (const void *)(ncahceStart + (i << 20)), (ncahceStart + (i << 20)),
                          &s_mmuBufferAttr); /* Frame buffer */
    }

    MMU_Enable();
}

void BOARD_PrintClock(void)
{
    printf("\r\n\r\n");
    printf("************************************************************\r\n");
    
    //输入时钟
    //printf("* OSC: %dHz\r\n",CLOCK_GetFreq(kCLOCK_OscClk));
    //printf("* RTC: %dHz\r\n",CLOCK_GetFreq(kCLOCK_RtcClk));
    //printf("* ------------------------------------------------------\r\n");
    //内部主要部件时钟
    printf("* CPU: %dHz    AXI:  %dHz\r\n",CLOCK_GetFreq(kCLOCK_CpuClk),CLOCK_GetFreq(kCLOCK_AxiClk));
    printf("* AHB: %dHz    MMDC: %dHz\r\n",CLOCK_GetFreq(kCLOCK_AhbClk),CLOCK_GetFreq(kCLOCK_MmdcClk));
    printf("* IPG: %dHz\r\n",CLOCK_GetFreq(kCLOCK_IpgClk));
    
    //ARM PLL相关时钟 PLL1
    //printf("* -------------------PLL1-------------------------------\r\n");
    //printf("* ARMPLL: %dHz\r\n",CLOCK_GetFreq(kCLOCK_ArmPllClk));
    
    //SYS PLL相关时钟 PLL2
    //printf("* -------------------PLL2-------------------------------\r\n");
    //printf("* SYSPLL:  %d\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    //printf("* SYSPDF0: %dHz     SYSPDF1: %dHz\r\n",CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk),CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    //printf("* SYSPDF2: %dHz     SYSPDF3: %dHz\r\n",CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk),CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    
    //USB1 PLL相关时钟
    //printf("* -------------------PLL3-------------------------------\r\n");
    //printf("* USB1PLL:  %d\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllClk));
    //printf("* USB1PDF0: %dHz     USB1PDF1: %dHz\r\n",CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk),CLOCK_GetFreq(kCLOCK_Usb1PllPfd1Clk));
    //printf("* USB1PDF2: %dHz     USB1PDF3: %dHz\r\n",CLOCK_GetFreq(kCLOCK_Usb1PllPfd2Clk),CLOCK_GetFreq(kCLOCK_Usb1PllPfd3Clk));
    
    //音频 PLL相关时钟
    //printf("* -------------------PLL4-------------------------------\r\n");
    //printf("* AudioPLL: %dHz\r\n",CLOCK_GetFreq(kCLOCK_AudioPllClk));
    
    //视频 PLL相关时钟
    //printf("* -------------------PLL5-------------------------------\r\n");
    //printf("* VideoPLL: %dHz\r\n",CLOCK_GetFreq(kCLOCK_VideoPllClk));
    
    //Enet PLL相关时钟
    //printf("* -------------------PLL6-------------------------------\r\n");
    //printf("* EnetPLL0:  %d\r\n", CLOCK_GetFreq(kCLOCK_EnetPll0Clk));
    //printf("* EnetPLL1:  %d\r\n", CLOCK_GetFreq(kCLOCK_EnetPll1Clk));
    //printf("* EnetPLL2:  %d\r\n", CLOCK_GetFreq(kCLOCK_EnetPll2Clk));
    
    //USB2 PLL相关时钟
    //printf("* -------------------PLL7-------------------------------\r\n");
    //printf("* USB2PLL:  %d\r\n", CLOCK_GetFreq(kCLOCK_Usb2PllClk));

    printf("************************************************************\r\n");
    printf("\r\n");
}




