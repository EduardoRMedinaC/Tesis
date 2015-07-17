/* ========================================
 *
 * Copyright FP UNA, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF FP UNA.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void BaudRate(char8 baudios);
static void send(char8 bytes[], float tr);
static int read_datablock(char8* data);
static void readlineCR(char8* line, int select);
static void substr(char8* substring, char8* string, int start, int end);



/* Constants */
#define ENABLED         (1u)
#define DISABLED        (0u)
#define NON_APPLICABLE  (DISABLED)

/* RX and TX buffers UART operation */
#define COMMON_BUFFER_SIZE     (16u)
uint8 bufferTx[COMMON_BUFFER_SIZE];
uint8 bufferRx[COMMON_BUFFER_SIZE + 1u];

/*******************************************************************************
* UART Configuration
*******************************************************************************/
#define UART_OVERSAMPLING       (16u)
#define UART_DATA_WIDTH         (7u)
#define UART_RX_INTR_MASK       (UART_1_INTR_RX_NOT_EMPTY)
#define UART_TX_INTR_MASK       (0u)

#define UART_RX_BUFFER_SIZE     (COMMON_BUFFER_SIZE)
#define UART_TX_BUFFER_SIZE     (COMMON_BUFFER_SIZE)
#define UART_RX_BUFER_PTR       bufferRx
#define UART_TX_BUFER_PTR       bufferTx

/* UART desired baud rate is 115200 bps. The selected Oversampling parameter is
* 16. The CommCLK = Baud rate * Oversampling = 115200 * 16 = 1.843 MHz.
* The clock divider has to be calculated to control clock frequency as clock
* component provides interface to it.
* Divider = (HFCLK / CommCLK) = (24MHz / 1.8432 MHz) = 13. But the value
* written into the register has to decremented by 1. The end result is 12.
* The clock accuracy is important for UART operation. The actual CommCLK equal:
* CommCLK(actual) = (24MHz / 13MHz) = 1.846 MHz
* The deviation of actual CommCLK from desired must be calculated:
* Deviation = (1.843MHz – 1.846 MHz) / 1.843 MHz = ~0.2%
* Taking into account HFCLK accuracy ±2%, the total error is: 0.2 + 2= 2.2%.
* The total error value is less than 5% and it is enough for correct
* UART operation.
*/

#define UART_CLK_DIVIDER_FOR_19200          (78u)
#define UART_CLK_DIVIDER_FOR_9600           (156u)
#define UART_CLK_DIVIDER_FOR_4800           (312u)
#define UART_CLK_DIVIDER_FOR_2400           (625u)
#define UART_CLK_DIVIDER_FOR_1200           (1250u)
#define UART_CLK_DIVIDER_FOR_600            (2500u)
#define UART_CLK_DIVIDER_FOR_300            (5000u)

/* Commons Baud Rate */
#define BD_300      '0'
#define BD_600      '1'
#define BD_1200     '2'
#define BD_2400     '3'
#define BD_4800     '4'
#define BD_9600     '5'
#define BD_19200    '6'

const UART_1_UART_INIT_STRUCT configUart =
{
    UART_1_UART_MODE_STD,       /* mode: Standard */
    UART_1_UART_TX_RX,          /* direction: RX + TX */
    UART_DATA_WIDTH,            /* dataBits: 7 bits */
    UART_1_UART_PARITY_EVEN,    /* parity: EVEN */
    UART_1_UART_STOP_BITS_1,    /* stopBits: 1 bit */
    UART_OVERSAMPLING,          /* oversample: 16 */
    DISABLED,                   /* enableIrdaLowPower: disabled */
    DISABLED,                   /* enableMedianFilter: disabled */
    DISABLED,                   /* enableRetryNack: disabled */
    DISABLED,                   /* enableInvertedRx: disabled */
    DISABLED,                   /* dropOnParityErr: disabled */
    DISABLED,                   /* dropOnFrameErr: disabled */
    NON_APPLICABLE,             /* enableWake: disabled */
    UART_RX_BUFFER_SIZE,        /* rxBufferSize: TX software buffer size */
    UART_RX_BUFER_PTR,          /* rxBuffer: pointer to RX software buffer */
    UART_TX_BUFFER_SIZE,        /* txBufferSize: TX software buffer size */
    UART_TX_BUFER_PTR,          /* txBuffer: pointer to TX software buffer */
    DISABLED,                   /* enableMultiproc: disabled */
    DISABLED,                   /* multiprocAcceptAddr: disabled */
    NON_APPLICABLE,             /* multiprocAddr: N/A */
    NON_APPLICABLE,             /* multiprocAddrMask: N/A */
    ENABLED,                    /* enableInterrupt: enable internal interrupt
                                 * handler for the software buffer */
    UART_RX_INTR_MASK,          /* rxInterruptMask: enable INTR_RX.NOT_EMPTY to
                                 * handle RX software buffer operations */
    NON_APPLICABLE,             /* rxTriggerLevel: N/A */
    UART_TX_INTR_MASK,          /* txInterruptMask: no TX interrupts on start up */
    NON_APPLICABLE,             /* txTriggerLevel: N/A */
    DISABLED,                   /* enableByteMode: disabled */
    DISABLED,                   /* enableCts: disabled */
    DISABLED,                   /* ctsPolarity: disabled */
    DISABLED,                   /* rtsRxFifoLevel: disabled */
    DISABLED,                   /* rtsPolarity: disabled */
};


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

static void BaudRate(char baudios)
{   
    switch(baudios)
    {
        case (BD_600):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_600, 0u);
            break;
        case (BD_1200):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_1200, 0u);
            break;
        case (BD_2400):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_2400, 0u);
            break;
        case (BD_4800):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_4800, 0u);
            break;
        case (BD_9600):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_9600, 0u);
            break;
        case (BD_19200):
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_19200, 0u);
            break;
        default:
            UART_CLK_SetFractionalDividerRegister(UART_CLK_DIVIDER_FOR_300, 0u);
            break;
    }
    
}
/* [] END OF FILE */
