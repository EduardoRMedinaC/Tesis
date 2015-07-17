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

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
