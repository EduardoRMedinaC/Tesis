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
#include <stdlib.h>
#include <str.h>

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void BaudRate(char baudios);
static void send(char bytes[], int tr);
static int read_datablock(char* data);
cystatus readlineCR(char* line, uint16 timeout);
cystatus read (char* buffer, uint16 size, uint16 timeOut);

/* Constants */
#define ENABLED         (1u)
#define DISABLED        (0u)
#define NON_APPLICABLE  (DISABLED)

/* RX and TX buffers UART operation */
#define COMMON_BUFFER_SIZE     (8u)
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

#define UART_CLK_DIVIDER_FOR_19200          (77u)
#define UART_CLK_DIVIDER_FOR_9600           (155u)
#define UART_CLK_DIVIDER_FOR_4800           (311u)
#define UART_CLK_DIVIDER_FOR_2400           (624u)
#define UART_CLK_DIVIDER_FOR_1200           (1249u)
#define UART_CLK_DIVIDER_FOR_600            (2499u)
#define UART_CLK_DIVIDER_FOR_300            (4999u)

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
    DISABLED,                    /* enableInterrupt: enable internal interrupt
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

/*************************************************************
* Especial characters
**************************************************************/
#define ACK 0x06u
#define STX 0x02u
#define ETX 0x03u


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_UartInit(&configUart);
    UART_1_Start();
    UART_2_Start();
    char data[10]="";
    int log;

    for(;;)
    {
        /* Place your application code here. */
         if(UART_2_UartGetChar() == 'z')
        {
            log = read_datablock(data);
            if(log == 4)
                UART_2_UartPutString("No identification message\r\n");
            if(log == 3)
                UART_2_UartPutString("Identification Message too short\r\n");
            if(log == 2)
                UART_2_UartPutString("El resultado no es el correcto, pruebe nuevamente\r\n");
            if(log == 1)
                UART_2_UartPutString("No se ha encontrado el STX\r\n");
            if(log == 0)
                UART_2_UartPutString("La comunicacion fue un exito!\r\n");
        }
        UART_2_UartPutString(data);
    }
}

static void BaudRate(char8 baudios)
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

static void send(char *bytes, int tr)
{
    /*
    sends an command to serial and reads and checks the echo
    port  - the open serial port
    bytes - the string to be send
    tr    - the responce time
    */
    int8 len = strlen(bytes);
    char *echo = (char*)malloc(sizeof(char)*10);
    strcpy(echo,"");
    char *mensaje = (char*)malloc(sizeof(char)*55);
    strcpy(mensaje,"bytes vs echo no son iguales(");
    UART_1_SpiUartClearRxBuffer();
    UART_1_SpiUartClearTxBuffer();
    
    UART_1_UartPutString(bytes);
    CyDelay(tr);
    
    read(echo,len,1500);
    if(strcmp(echo,bytes) != 0)
    {    
        strcat(mensaje,bytes);
        strcat(mensaje,"vs");
        strcat(mensaje,echo);
        strcat(mensaje, ")\r\n");
        UART_2_UartPutString(mensaje);
    }
    free(echo);
    free(mensaje);
}

cystatus readlineCR(char* line, uint16 timeout)
{
    int timeoutUs = timeout * 1000;
    cystatus status = CYRET_TIMEOUT;
    char ch[2]="";
    strcpy(line,"");
    do
    {
        if(UART_1_SpiUartGetRxBufferSize() > 0)
        {
            ch[0] = UART_1_UartGetChar();
            strcat(line,ch);
            timeoutUs = timeout * 1000;
        } 
        else
        {
            CyDelayUs(5);
            timeoutUs -= 5;
        }
    }while (ch[0] != '\n' && timeoutUs >= 0);
    
    return status;
}

cystatus read (char* buffer, uint16 size, uint16 timeOut) {
	int timeoutUs = timeOut * 1000;
	cystatus status = CYRET_TIMEOUT;
	
	uint16 count = 0;
	while(count < size && timeoutUs >= 0) {
		if(UART_1_SpiUartGetRxBufferSize() > 0) {
			buffer[count++] = UART_1_UartGetByte();
			// Switch to byte-to-byte timeout and mark as success
			timeoutUs = timeOut * 1000; //mS
			status = CYRET_SUCCESS;
		} else {
			CyDelayUs(5);
			timeoutUs -= 5;
		}
	}
	return status;
}

static int read_datablock(char *data)
{
    int16 tr = 200;
    char *identification_message = (char*)malloc(sizeof(char)* 32);
    strcpy(identification_message,"");
    char *manufactures_id;
    char *identification;              // meassure id
    char speed;
    char *acknowledgement_message = (char*)malloc(sizeof(char)*7);
    strcpy(acknowledgement_message,"0000\r\n");
    *acknowledgement_message = ACK;
    char ch[2]="";                         // received character
    int bcc;                                // block character controller
    
    UART_1_Stop();
    BaudRate(BD_300);
    UART_1_Enable();
    // 1 ->
    CyDelay(tr);
    send("/?!\r\n", tr);     //IEC 62056-21:2002(E) 6.3.1
    //2 <-
    CyDelay(tr);
    readlineCR(identification_message,1500);      //IEC 62056-21:2002(E) 6.3.2
    //UART_2_UartPutString(identification_message);
    if(strlen(identification_message) < 1 || identification_message[0] != '/')
    {
        UART_1_Stop();
        return 4;       // No identification message
    }
    if(strlen(identification_message) < 7)
    {
        UART_1_Stop();
        return 3;       // Identification Message too short
    }
    if(islower(identification_message[3]))
        tr = 20;
    manufactures_id = substring(identification_message, 1, 4);
    free(manufactures_id);
    if(identification_message[5] == '\\')
        identification = substring(identification_message, 7, -2);
    else
        identification = substring(identification_message, 5, -2);
    free(identification);
    speed = identification_message[4];
    free(identification_message);
    // 3 ->
    // IEC 62056-21:2002(E) 6.3.3
    *(acknowledgement_message + 2) = speed;
    send(acknowledgement_message, tr);
    free(acknowledgement_message);
    
    UART_1_Stop();
    BaudRate(speed);
    UART_1_Enable();
    UART_1_SpiUartClearRxBuffer();
    CyDelay(tr);
    // 4 <-
    read(ch,1,2200);
    strcpy(data,"");
    if( ch[0] == STX)
    {
        read(ch,1,1500);
        UART_2_UartPutString(ch);
        bcc = 0;
        while (ch[0] != '!')
        {
            bcc = bcc ^ ch[0];
            read(ch,1,1500);
            UART_2_UartPutString(ch);
        }
        while (ch[0] != ETX)
        {
            bcc = bcc ^ ch[0];
            read(ch,1,1500);
            UART_2_UartPutString(ch);
        }
        bcc = bcc ^ ch[0];                  // ETX itself is part of block check
        read(ch,1,1500);                    // ch[0] es ahora el BLOCK CHECK CHARACTER
        
        if(bcc != ch[0])
        {
            strcpy(data, "");
            return 2;           // El resultado no es el correcto, pruebe nuevamente
        }
    }
    else
    {
        return 1;               // No se ha encontrado el STX
    }
    UART_1_Stop();
    return 0;                   // La comunicacion fue un exito!
}
/* [] END OF FILE */
