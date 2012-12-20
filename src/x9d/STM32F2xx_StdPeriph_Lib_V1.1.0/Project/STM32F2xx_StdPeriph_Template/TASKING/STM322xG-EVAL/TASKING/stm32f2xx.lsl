////////////////////////////////////////////////////////////////////////////
//
//  File        :  stm32f2xx.lsl
//
//  Version     :  @(#)stm32f2xx.lsl    1.1 10/11/03
//
//  Description :  LSL file for the STMicroelectronics STM32F2xx
//
//  Copyright 2010 Altium BV     
//  
//  Macros specific to control this LSL file
//
//  __MEMORY            Define this macro to suppress definition of on-chip 
//                      memory. Must be defined when you want to define on-chip
//                      in your project's LSL file.
//  __FLASH_SIZE        Specifies the size of flash memory to be allocated
//  __SRAM_SIZE         Specifies the size of the SRAM memory to be allocated
//  __NO_DEFAULT_AUTO_VECTORS
//                      When enabled the interrup vector table will not be 
//                      generated
//  __COPY_VECTOR_TABLE Define this macro to enable copying the vector table 
//                      at startup from ROM to RAM
//  __VECTOR_TABLE_RAM_COPY
//                      Define this macro to enable copying the vector table 
//                      at startup from ROM to RAM. It is also required to 
//                      define the macro __VECTOR_TABLE_RAM_SPACE
//  __VECTOR_TABLE_RAM_SPACE
//                      Define size of the RAM space for the vector table when
//                      it is copied from ROM to RAM
//  __VECTOR_TABLE_ROM_ADDR
//                      Specify the vector table address in ROM
//  __VECTOR_TABLE_RAM_ADDR
//                      Specify the vector table address in RAM when the the
//                      it is copied from ROM to RAM (__COPY_VECTOR_TABLE)
//
//  See arm_arch.lsl for more available macros.
//
//  Notes:
//  In the STM32F2xx, 3 different boot modes can be selected
//  - User Flash memory is selected as boot space
//  - SystemMemory is selected as boot space
//  - Embedded SRAM is selected as boot space
//
//  This aliases the physical memory associated with each boot mode to Block
//  000 (0x00000000 boot memory). Even when aliased in the boot memory space,
//  the related memory (Flash memory or SRAM) is still accessible at its
//  original memory space.
//
////////////////////////////////////////////////////////////////////////////

#ifndef __NO_DEFAULT_AUTO_VECTORS
// Suppress the vectors as defined arm_arch.lsl, because we define our 
// own vectors for CMIS
#define __CMSIS_VECTORS                 1
#define __NO_DEFAULT_AUTO_VECTORS       1
#define __NR_OF_VECTORS                 97
#endif

#ifndef __VECTOR_TABLE_ROM_ADDR
#  define __VECTOR_TABLE_ROM_ADDR       0x08000000
#endif

#ifndef __VECTOR_TABLE_RAM_ADDR
#  define __VECTOR_TABLE_RAM_ADDR       0x00000000
#endif


#ifndef __STACK
#  define __STACK                       4k
#endif

#ifndef __HEAP
#  define __HEAP                        2k
#endif

#include <arm_arch.lsl>

//
//  If no memory is defined yet use the following memory settings
//
#ifndef __MEMORY

// Specify default size for Flash and SRAM
#ifndef __FLASH_SIZE
#  define __FLASH_SIZE  1024k
#endif
#ifndef __SRAM_SIZE
#  define __SRAM_SIZE   128k
#endif

memory STM32F2xx_Flash
{
    mau = 8;
    type = rom;
    size = __FLASH_SIZE;
    map ( size = __FLASH_SIZE, dest_offset=0x08000000, dest=bus:ARM:local_bus);
}

memory STM32F2xx_SRAM
{
    mau = 8;
    type = ram;
    size = __SRAM_SIZE;
    map ( size = __SRAM_SIZE, dest_offset=0x20000000, dest=bus:ARM:local_bus);
}

#endif /* __MEMORY */


//
// Define the vector table for CMSIS
//
#ifdef __CMSIS_VECTORS
section_setup ::linear
{
                // vector table with handler addresses
                vector_table "vector_table" ( vector_size = 4, size = __NR_OF_VECTORS, run_addr = __VECTOR_TABLE_ROM_ADDR,
                                              template = ".text.handler_address",
                                              template_symbol = "_lc_vector_handler",
                                              vector_prefix = "_vector_",
                                              fill = loop,
                                              no_inline
                                            )
                {
                        vector ( id = 0, fill = "_lc_ub_stack" );                               // Top of Stack
                        vector ( id = 1, fill = "_START" );                                     // Reset Handler
                        vector ( id = 2, optional, fill = "NMI_Handler" );                      // NMI Handler
                        vector ( id = 3, optional, fill = "HardFault_Handler" );                // Hard Fault Handler
                        vector ( id = 4, optional, fill = "MemManage_Handler" );                // MPU Fault Handler
                        vector ( id = 5, optional, fill = "BusFault_Handler" );                 // Bus Fault Handler
                        vector ( id = 6, optional, fill = "UsageFault_Handler" );               // Usage Fault Handler
                        vector ( id = 11, optional, fill = "SVC_Handler" );                     // SVCall Handler
                        vector ( id = 12, optional, fill = "DebugMon_Handler" );                // Debug Monitor Handler
                        vector ( id = 14, optional, fill = "PendSV_Handler" );                  // PendSV Handler
                        vector ( id = 15, optional, fill = "SysTick_Handler" );                 // SysTick Handler

                        // External Interrupts :
                        vector ( id = 16, optional, fill = "WWDG_IRQHandler" );                 // Window Watchdog
                        vector ( id = 17, optional, fill = "PVD_IRQHandler" );                  // PVD through EXTI Line detect
                        vector ( id = 18, optional, fill = "TAMPER_STAMP_IRQHandler" );         // Tamper amd TimeStanp through EXTI
                        vector ( id = 19, optional, fill = "RTC_WKUP_IRQHandler" );             // RTC Wakeup through the EXTI line
                        vector ( id = 20, optional, fill = "FLASH_IRQHandler" );                // Flash global interrupt
                        vector ( id = 21, optional, fill = "RCC_IRQHandler" );                  // RCC global interrupt
                        vector ( id = 22, optional, fill = "EXTI0_IRQHandler" );                // EXTI Line 0 interrupt
                        vector ( id = 23, optional, fill = "EXTI1_IRQHandler" );                // EXTI Line 1 interrupt
                        vector ( id = 24, optional, fill = "EXTI2_IRQHandler" );                // EXTI Line 2 interrupt
                        vector ( id = 25, optional, fill = "EXTI3_IRQHandler" );                // EXTI Line 3 interrupt
                        vector ( id = 26, optional, fill = "EXTI4_IRQHandler" );                // EXTI Line 4 interrupt
                        vector ( id = 27, optional, fill = "DMA1_Stream0_IRQHandler" );         // DMA1 Stream 0 interrupt
                        vector ( id = 28, optional, fill = "DMA1_Stream1_IRQHandler" );         // DMA1 Stream 1 interrupt
                        vector ( id = 29, optional, fill = "DMA1_Stream2_IRQHandler" );         // DMA1 Stream 2 interrupt
                        vector ( id = 30, optional, fill = "DMA1_Stream3_IRQHandler" );         // DMA1 Stream 3 interrupt
                        vector ( id = 31, optional, fill = "DMA1_Stream4_IRQHandler" );         // DMA1 Stream 4 interrupt
                        vector ( id = 32, optional, fill = "DMA1_Stream5_IRQHandler" );         // DMA1 Stream 5 interrupt
                        vector ( id = 33, optional, fill = "DMA1_Stream6_IRQHandler" );         // DMA1 Stream 6 interrupt
                        vector ( id = 34, optional, fill = "ADC_IRQHandler" );                  // ADC_IRQHandler interrupts
                        vector ( id = 35, optional, fill = "CAN1_TX_IRQHandler" );              // CAN1 TX interrupts
                        vector ( id = 36, optional, fill = "CAN1_RX0_IRQHandler" );             // CAN1 RX0 interrupts
                        vector ( id = 37, optional, fill = "CAN1_RX1_IRQHandler" );             // CAN1 RX1 interrupt
                        vector ( id = 38, optional, fill = "CAN1_SCE_IRQHandler" );             // CAN1 SCE interrupt
                        vector ( id = 39, optional, fill = "EXTI9_5_IRQHandler" );              // EXTI Line 9..5 interrupts
                        vector ( id = 40, optional, fill = "TIM1_BRK_TIM9_IRQHandler" );        // TIM1 Break irq and TIM9 global interrupt
                        vector ( id = 41, optional, fill = "TIM1_UP_TIM10_IRQHandler" );        // TIM1 Update irq and TIM10 global interrupt
                        vector ( id = 42, optional, fill = "TIM1_TRG_COM_TIM11_IRQHandler" );   // TIM1 Trigger and Commutation irqs and TIM10 global irq
                        vector ( id = 43, optional, fill = "TIM1_CC_IRQHandler" );              // TIM1 Capture Compare
                        vector ( id = 44, optional, fill = "TIM2_IRQHandler" );                 // TIM2 global interrupt
                        vector ( id = 45, optional, fill = "TIM3_IRQHandler" );                 // TIM3 global interrupt
                        vector ( id = 46, optional, fill = "TIM4_IRQHandler" );                 // TIM4 global interrupt
                        vector ( id = 47, optional, fill = "I2C1_EV_IRQHandler" );              // I2C1 Event
                        vector ( id = 48, optional, fill = "I2C1_ER_IRQHandler" );              // I2C1 Error
                        vector ( id = 49, optional, fill = "I2C2_EV_IRQHandler" );              // I2C2 Event
                        vector ( id = 50, optional, fill = "I2C2_ER_IRQHandler" );              // I2C2 Error
                        vector ( id = 51, optional, fill = "SPI1_IRQHandler" );                 // SPI1 global interrupt
                        vector ( id = 52, optional, fill = "SPI2_IRQHandler" );                 // SPI2 global interrupt
                        vector ( id = 53, optional, fill = "USART1_IRQHandler" );               // USART1 global interrupt
                        vector ( id = 54, optional, fill = "USART2_IRQHandler" );               // USART2 global interrupt
                        vector ( id = 55, optional, fill = "USART3_IRQHandler" );               // USART3 global interrupt
                        vector ( id = 56, optional, fill = "EXTI15_10_IRQHandler" );            // EXTI Line 15..10 interrupts
                        vector ( id = 57, optional, fill = "RTCAlarm_IRQHandler" );             // RTC Alarm through EXTI Line
                        vector ( id = 58, optional, fill = "OTG_FS_WKUP_IRQHandler" );          // USB On-The-Go FS Wakeup throught EXTI
                        vector ( id = 59, optional, fill = "TIM8_BRK_TIM12_IRQHandler" );       // TIM8 Break irq and TIM12 global irq
                        vector ( id = 60, optional, fill = "TIM8_UP_TIM13_IRQHandler" );        // TIM8 Update irq and TIM13 global irq
                        vector ( id = 61, optional, fill = "TIM8_TRG_COM_TIM14_IRQHandler" );   // TIM8 trigger and Comm. irqs and TIM14 global irq
                        vector ( id = 62, optional, fill = "TIM8_CC_IRQHandler" );              // TIM8 Capture Compare irq
                        vector ( id = 63, optional, fill = "DMA1_Stream7_IRQHandler" );         // DMA1 Stream7 global interrupt
                        vector ( id = 64, optional, fill = "FSMC_IRQHandler" );                 // FSMC global interrupt
                        vector ( id = 65, optional, fill = "SDIO_IRQHandler" );                 // SDIO global interrupt
                        vector ( id = 66, optional, fill = "TIM5_IRQHandler" );                 // TIM5 global interrupt
                        vector ( id = 67, optional, fill = "SPI3_IRQHandler" );                 // SPI3 global interrupt
                        vector ( id = 68, optional, fill = "UART4_IRQHandler" );                // UART4 global interrupt
                        vector ( id = 69, optional, fill = "UART5_IRQHandler" );                // UART5 global interrupt
                        vector ( id = 70, optional, fill = "TIM6_DAC_IRQHandler" );             // TIM6 glbl irq, DAC1 and DAC2 underrun err irqs
                        vector ( id = 71, optional, fill = "TIM7_IRQHandler" );                 // TIM7 global interrupt
                        vector ( id = 72, optional, fill = "DMA2_Stream0_IRQHandler" );         // DMA2 Stream0 global interrupt
                        vector ( id = 73, optional, fill = "DMA2_Stream1_IRQHandler" );         // DMA2 Stream1 global interrupt
                        vector ( id = 74, optional, fill = "DMA2_Stream2_IRQHandler" );         // DMA2 Stream2 global interrupt
                        vector ( id = 75, optional, fill = "DMA2_Stream3_IRQHandler" );         // DMA2 Stream3 global interrupt
                        vector ( id = 76, optional, fill = "DMA2_Stream4_IRQHandler" );         // DMA2 Stream4 global interrupt
                        vector ( id = 77, optional, fill = "ETH_IRQHandler" );                  // Ethernet global interrupt
                        vector ( id = 78, optional, fill = "ETH_WKUP_IRQHandler" );              // Ethernet Wakeup through EXTI line irq
                        vector ( id = 79, optional, fill = "CAN2_TX_IRQHandler" );              // CAN2 TX interrupts
                        vector ( id = 80, optional, fill = "CAN2_RX0_IRQHandler" );             // CAN2 RX0 interrupts
                        vector ( id = 81, optional, fill = "CAN2_RX1_IRQHandler" );             // CAN2 RX1 interrupt
                        vector ( id = 82, optional, fill = "CAN2_SCE_IRQHandler" );             // CAN2 SCE interrupt
                        vector ( id = 83, optional, fill = "OTG_FS_IRQHandler" );               // USB On The Go FS global interrupt
                        vector ( id = 84, optional, fill = "DMA2_Stream5_IRQHandler" );         // DMA2 Stream5 global interrupt
                        vector ( id = 85, optional, fill = "DMA2_Stream6_IRQHandler" );         // DMA2 Stream6 global interrupt
                        vector ( id = 86, optional, fill = "DMA2_Stream7_IRQHandler" );         // DMA2 Stream7 global interrupt
                        vector ( id = 87, optional, fill = "USART6_IRQHandler" );               // USART6 global interrupt
                        vector ( id = 88, optional, fill = "I2C3_EV_IRQHandler" );              // I2C3 event interrupt
                        vector ( id = 89, optional, fill = "I2C3_ER_IRQHandler" );              // I2C3 error interrupt
                        vector ( id = 90, optional, fill = "OTG_HS_EP1_OUT_IRQHandler" );       // USB On The Go HS End Point 1 OUT global irq
                        vector ( id = 91, optional, fill = "OTG_HS_EP1_IN_IRQHandler" );        // USB On The Go HS End Point 1 In global irq
                        vector ( id = 92, optional, fill = "OTG_HS_WKUP_IRQHandler" );          // USB On The Go HS Wakeup through EXTI irq
                        vector ( id = 93, optional, fill = "OTG_HS_IRQHandler" );               // USB On The Go HS global interrupt
                        vector ( id = 94, optional, fill = "DCMI_IRQHandler" );                 // DCMI global interrupt
                        vector ( id = 95, optional, fill = "CRYP_IRQHandler" );                 // CRYP crypto global interrupt
                        vector ( id = 96, optional, fill = "HASH_RNG_IRQHandler" );             // Hash and Rng global interrupt
                }
}
#endif /* __CMSIS_VECTORS */
