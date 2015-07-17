/*******************************************************************************
* File Name: UART_1_SPI_BOOT.c
* Version 3.0
*
* Description:
*  This file provides the source code of the bootloader communication APIs
*  for the SCB Component SPI mode.
*
* Note:
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_1_BOOT.h"
#include "UART_1_SPI_UART.h"

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (UART_1_SPI_BTLDR_COMM_ENABLED)

/*******************************************************************************
* Function Name: UART_1_SpiCyBtldrCommStart
********************************************************************************
*
* Summary:
*  Starts the SPI component.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_1_SpiCyBtldrCommStart(void)
{
    UART_1_Start();
}


/*******************************************************************************
* Function Name: UART_1_SpiCyBtldrCommStop
********************************************************************************
*
* Summary:
*  Disables the SPI component.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_1_SpiCyBtldrCommStop(void)
{
    UART_1_Stop();
}


/*******************************************************************************
* Function Name: UART_1_SpiCyBtldrCommReset
********************************************************************************
*
* Summary:
*  Resets the receive and transmit communication buffers.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_1_SpiCyBtldrCommReset(void)
{
    /* Clear RX and TX buffers */
    UART_1_SpiUartClearRxBuffer();
    UART_1_SpiUartClearTxBuffer();
}


/*******************************************************************************
* Function Name: UART_1_SpiCyBtldrCommRead
********************************************************************************
*
* Summary:
*  Allows the caller to read data from the bootloader host (the host writes the
*  data). The function handles polling to allow a block of data to be completely
*  received from the host device.
*
* Parameters:
*  pData:    Pointer to storage for the block of data to be read from the
*            bootloader host
*  size:     Number of bytes to be read.
*  count:    Pointer to the variable to write the number of bytes actually
*            read.
*  timeOut:  Number of units in 10 ms to wait before returning because of a
*            timeout.
*
* Return:
*  Returns CYRET_SUCCESS if no problem was encountered or returns the value
*  that best describes the problem. For more information refer to the
*  "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cystatus UART_1_SpiCyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
{
    cystatus status;
    uint32 byteCount;
    uint32 timeoutMs;
    uint32 i;

    status = CYRET_BAD_PARAM;

    if ((NULL != pData) && (size > 0u))
    {
        status = CYRET_TIMEOUT;
        timeoutMs = ((uint32) 10u * timeOut); /* Convert from 10mS check to 1mS checks */

        /* Wait with timeout 1mS for packet start */
        byteCount = 0u;
        do
        {
            /* Check packet start */
            if (0u != UART_1_SpiUartGetRxBufferSize())
            {
                /* Wait for packet end */
                do
                {
                    byteCount = UART_1_SpiUartGetRxBufferSize();
                    CyDelayUs(UART_1_SPI_BYTE_TO_BYTE);
                }
                while (byteCount != UART_1_SpiUartGetRxBufferSize());

                /* Disable data reception into RX FIFO */
                UART_1_RX_FIFO_CTRL_REG |= UART_1_RX_FIFO_CTRL_FREEZE;

                byteCount = UART_1_BYTES_TO_COPY(byteCount, size);
                *count = (uint16) byteCount;
                status = CYRET_SUCCESS;

                break;
            }

            CyDelay(UART_1_WAIT_1_MS);
            --timeoutMs;
        }
        while (0u != timeoutMs);

        /* Get data from the RX buffer into bootloader buffer */
        for (i = 0u; i < byteCount; ++i)
        {
            pData[i] = (uint8) UART_1_SpiUartReadRxData();
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: UART_1_SpiCyBtldrCommWrite
********************************************************************************
*
* Summary:
*  Allows the caller to read data from the bootloader host (the host writes the
*  data). The function handles polling to allow a block of data to be completely
*  received from the host device.
*
* Parameters:
*  pData:    Pointer to the block of data to be written to the bootloader host.
*  size:     Number of bytes to be written.
*  count:    Pointer to the variable to write the number of bytes actually
*            written.
*  timeOut:  Number of units in 10 ms to wait before returning because of a
*            timeout.
*
* Return:
*  Returns CYRET_SUCCESS if no problem was encountered or returns the value
*  that best describes the problem. For more information refer to the
*  "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cystatus UART_1_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
{
    cystatus status;
    uint32 timeoutMs;

    status = CYRET_BAD_PARAM;

    if ((NULL != pData) && (size > 0u))
    {
        status = CYRET_TIMEOUT;
        timeoutMs = ((uint32) 10u * timeOut); /* Convert from 10mS check to 1mS checks */

        /* Put data into TX buffer */
        UART_1_SpiUartPutArray(pData, (uint32) size);

        /* Wait with timeout 1mS for packet end */
        do
        {
            /* Check for packet end */
            if ((0u == UART_1_SpiUartGetTxBufferSize()) &&
                (0u == (UART_1_SPI_STATUS_REG & UART_1_SPI_STATUS_BUS_BUSY)) &&
                (0u == (UART_1_TX_FIFO_STATUS_REG & UART_1_TX_FIFO_SR_VALID)))
            {
                *count = size;
                status = CYRET_SUCCESS;

                break;
            }

            CyDelay(UART_1_WAIT_1_MS);
            --timeoutMs;
        }
        while (0u != timeoutMs);

        /* Enable data reception into RX FIFO */
        UART_1_SpiUartClearRxBuffer();
        UART_1_RX_FIFO_CTRL_REG &= (uint32) ~UART_1_RX_FIFO_CTRL_FREEZE;
    }

    return (status);
}

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (UART_1_SPI_BTLDR_COMM_ENABLED) */


/* [] END OF FILE */
