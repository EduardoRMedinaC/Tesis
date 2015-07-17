/*******************************************************************************
* File Name: UART_1_SPI.c
* Version 3.0
*
* Description:
*  This file provides the source code to the API for the SCB Component in
*  SPI mode.
*
* Note:
*
*******************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_1_PVT.h"
#include "UART_1_SPI_UART_PVT.h"

#if(UART_1_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    const UART_1_SPI_INIT_STRUCT UART_1_configSpi =
    {
        UART_1_SPI_MODE,
        UART_1_SPI_SUB_MODE,
        UART_1_SPI_CLOCK_MODE,
        UART_1_SPI_OVS_FACTOR,
        UART_1_SPI_MEDIAN_FILTER_ENABLE,
        UART_1_SPI_LATE_MISO_SAMPLE_ENABLE,
        UART_1_SPI_WAKE_ENABLE,
        UART_1_SPI_RX_DATA_BITS_NUM,
        UART_1_SPI_TX_DATA_BITS_NUM,
        UART_1_SPI_BITS_ORDER,
        UART_1_SPI_TRANSFER_SEPARATION,
        0u,
        NULL,
        0u,
        NULL,
        (uint32) UART_1_SCB_IRQ_INTERNAL,
        UART_1_SPI_INTR_RX_MASK,
        UART_1_SPI_RX_TRIGGER_LEVEL,
        UART_1_SPI_INTR_TX_MASK,
        UART_1_SPI_TX_TRIGGER_LEVEL,
        (uint8) UART_1_SPI_BYTE_MODE_ENABLE,
        (uint8) UART_1_SPI_FREE_RUN_SCLK_ENABLE,
        (uint8) UART_1_SPI_SS_POLARITY
    };


    /*******************************************************************************
    * Function Name: UART_1_SpiInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the SPI operation.
    *
    * Parameters:
    *  config:  Pointer to a structure that contains the following ordered list of
    *           fields. These fields match the selections available in the
    *           customizer.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiInit(const UART_1_SPI_INIT_STRUCT *config)
    {
        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Configure pins */
            UART_1_SetPins(UART_1_SCB_MODE_SPI, config->mode, UART_1_DUMMY_PARAM);

            /* Store internal configuration */
            UART_1_scbMode       = (uint8) UART_1_SCB_MODE_SPI;
            UART_1_scbEnableWake = (uint8) config->enableWake;
            UART_1_scbEnableIntr = (uint8) config->enableInterrupt;

            /* Set RX direction internal variables */
            UART_1_rxBuffer      =         config->rxBuffer;
            UART_1_rxDataBits    = (uint8) config->rxDataBits;
            UART_1_rxBufferSize  = (uint8) config->rxBufferSize;

            /* Set TX direction internal variables */
            UART_1_txBuffer      =         config->txBuffer;
            UART_1_txDataBits    = (uint8) config->txDataBits;
            UART_1_txBufferSize  = (uint8) config->txBufferSize;

            /* Configure SPI interface */
            UART_1_CTRL_REG     = UART_1_GET_CTRL_OVS(config->oversample)           |
                                            UART_1_GET_CTRL_BYTE_MODE(config->enableByteMode) |
                                            UART_1_GET_CTRL_EC_AM_MODE(config->enableWake)    |
                                            UART_1_CTRL_SPI;

            UART_1_SPI_CTRL_REG = UART_1_GET_SPI_CTRL_CONTINUOUS    (config->transferSeperation)  |
                                            UART_1_GET_SPI_CTRL_SELECT_PRECEDE(config->submode &
                                                                          UART_1_SPI_MODE_TI_PRECEDES_MASK) |
                                            UART_1_GET_SPI_CTRL_SCLK_MODE     (config->sclkMode)            |
                                            UART_1_GET_SPI_CTRL_LATE_MISO_SAMPLE(config->enableLateSampling)|
                                            UART_1_GET_SPI_CTRL_SCLK_CONTINUOUS(config->enableFreeRunSclk)  |
                                            UART_1_GET_SPI_CTRL_SSEL_POLARITY (config->polaritySs)          |
                                            UART_1_GET_SPI_CTRL_SUB_MODE      (config->submode)             |
                                            UART_1_GET_SPI_CTRL_MASTER_MODE   (config->mode);

            /* Configure RX direction */
            UART_1_RX_CTRL_REG     =  UART_1_GET_RX_CTRL_DATA_WIDTH(config->rxDataBits)         |
                                                UART_1_GET_RX_CTRL_BIT_ORDER (config->bitOrder)           |
                                                UART_1_GET_RX_CTRL_MEDIAN    (config->enableMedianFilter) |
                                                UART_1_SPI_RX_CTRL;

            UART_1_RX_FIFO_CTRL_REG = UART_1_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(config->rxTriggerLevel);

            /* Configure TX direction */
            UART_1_TX_CTRL_REG      = UART_1_GET_TX_CTRL_DATA_WIDTH(config->txDataBits) |
                                                UART_1_GET_TX_CTRL_BIT_ORDER (config->bitOrder)   |
                                                UART_1_SPI_TX_CTRL;

            UART_1_TX_FIFO_CTRL_REG = UART_1_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(config->txTriggerLevel);

            /* Configure interrupt with SPI handler but do not enable it */
            CyIntDisable    (UART_1_ISR_NUMBER);
            CyIntSetPriority(UART_1_ISR_NUMBER, UART_1_ISR_PRIORITY);
            (void) CyIntSetVector(UART_1_ISR_NUMBER, &UART_1_SPI_UART_ISR);

            /* Configure interrupt sources */
            UART_1_INTR_I2C_EC_MASK_REG = UART_1_NO_INTR_SOURCES;
            UART_1_INTR_SPI_EC_MASK_REG = UART_1_NO_INTR_SOURCES;
            UART_1_INTR_SLAVE_MASK_REG  = UART_1_GET_SPI_INTR_SLAVE_MASK(config->rxInterruptMask);
            UART_1_INTR_MASTER_MASK_REG = UART_1_GET_SPI_INTR_MASTER_MASK(config->txInterruptMask);
            UART_1_INTR_RX_MASK_REG     = UART_1_GET_SPI_INTR_RX_MASK(config->rxInterruptMask);
            UART_1_INTR_TX_MASK_REG     = UART_1_GET_SPI_INTR_TX_MASK(config->txInterruptMask);

            /* Set active SS0 */
            UART_1_SpiSetActiveSlaveSelect(UART_1_SPI_SLAVE_SELECT0);

            /* Clear RX buffer indexes */
            UART_1_rxBufferHead     = 0u;
            UART_1_rxBufferTail     = 0u;
            UART_1_rxBufferOverflow = 0u;

            /* Clear TX buffer indexes */
            UART_1_txBufferHead = 0u;
            UART_1_txBufferTail = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: UART_1_SpiInit
    ********************************************************************************
    *
    * Summary:
    *  Configures the SCB for the SPI operation.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiInit(void)
    {
        /* Configure SPI interface */
        UART_1_CTRL_REG     = UART_1_SPI_DEFAULT_CTRL;
        UART_1_SPI_CTRL_REG = UART_1_SPI_DEFAULT_SPI_CTRL;

        /* Configure TX and RX direction */
        UART_1_RX_CTRL_REG      = UART_1_SPI_DEFAULT_RX_CTRL;
        UART_1_RX_FIFO_CTRL_REG = UART_1_SPI_DEFAULT_RX_FIFO_CTRL;

        /* Configure TX and RX direction */
        UART_1_TX_CTRL_REG      = UART_1_SPI_DEFAULT_TX_CTRL;
        UART_1_TX_FIFO_CTRL_REG = UART_1_SPI_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with SPI handler but do not enable it */
    #if(UART_1_SCB_IRQ_INTERNAL)
            CyIntDisable    (UART_1_ISR_NUMBER);
            CyIntSetPriority(UART_1_ISR_NUMBER, UART_1_ISR_PRIORITY);
            (void) CyIntSetVector(UART_1_ISR_NUMBER, &UART_1_SPI_UART_ISR);
    #endif /* (UART_1_SCB_IRQ_INTERNAL) */

        /* Configure interrupt sources */
        UART_1_INTR_I2C_EC_MASK_REG = UART_1_SPI_DEFAULT_INTR_I2C_EC_MASK;
        UART_1_INTR_SPI_EC_MASK_REG = UART_1_SPI_DEFAULT_INTR_SPI_EC_MASK;
        UART_1_INTR_SLAVE_MASK_REG  = UART_1_SPI_DEFAULT_INTR_SLAVE_MASK;
        UART_1_INTR_MASTER_MASK_REG = UART_1_SPI_DEFAULT_INTR_MASTER_MASK;
        UART_1_INTR_RX_MASK_REG     = UART_1_SPI_DEFAULT_INTR_RX_MASK;
        UART_1_INTR_TX_MASK_REG     = UART_1_SPI_DEFAULT_INTR_TX_MASK;

        /* Set active SS0 for master */
    #if (UART_1_SPI_MASTER_CONST)
        UART_1_SpiSetActiveSlaveSelect(UART_1_SPI_SLAVE_SELECT0);
    #endif /* (UART_1_SPI_MASTER_CONST) */

    #if(UART_1_INTERNAL_RX_SW_BUFFER_CONST)
        UART_1_rxBufferHead     = 0u;
        UART_1_rxBufferTail     = 0u;
        UART_1_rxBufferOverflow = 0u;
    #endif /* (UART_1_INTERNAL_RX_SW_BUFFER_CONST) */

    #if(UART_1_INTERNAL_TX_SW_BUFFER_CONST)
        UART_1_txBufferHead = 0u;
        UART_1_txBufferTail = 0u;
    #endif /* (UART_1_INTERNAL_TX_SW_BUFFER_CONST) */
    }
#endif /* (UART_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: UART_1_SpiPostEnable
********************************************************************************
*
* Summary:
*  Restores HSIOM settings for the SPI master output pins (SCLK and/or SS0-SS3) 
*  to be controlled by the SCB SPI.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_1_SpiPostEnable(void)
{
#if(UART_1_SCB_MODE_UNCONFIG_CONST_CFG)

    if (UART_1_CHECK_SPI_MASTER)
    {
    #if (UART_1_SCLK_PIN)
        /* Set SCB SPI to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SCLK_HSIOM_REG, UART_1_SCLK_HSIOM_MASK,
                                       UART_1_SCLK_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
    #endif /* (UART_1_SCLK_PIN) */

    #if (UART_1_SS0_PIN)
        /* Set SCB SPI to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS0_HSIOM_REG, UART_1_SS0_HSIOM_MASK,
                                       UART_1_SS0_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
    #endif /* (UART_1_SS0_PIN) */

    #if (UART_1_SS1_PIN)
        /* Set SCB SPI to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS1_HSIOM_REG, UART_1_SS1_HSIOM_MASK,
                                       UART_1_SS1_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
    #endif /* (UART_1_SS1_PIN) */

    #if (UART_1_SS2_PIN)
        /* Set SCB SPI to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS2_HSIOM_REG, UART_1_SS2_HSIOM_MASK,
                                       UART_1_SS2_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
    #endif /* (UART_1_SS2_PIN) */

    #if (UART_1_SS3_PIN)
        /* Set SCB SPI to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS3_HSIOM_REG, UART_1_SS3_HSIOM_MASK,
                                       UART_1_SS3_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
    #endif /* (UART_1_SS3_PIN) */
    }

#else

#if (UART_1_SPI_MASTER_SCLK_PIN)
    /* Set SCB SPI to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SCLK_M_HSIOM_REG, UART_1_SCLK_M_HSIOM_MASK,
                                   UART_1_SCLK_M_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
#endif /* (UART_1_MISO_SDA_TX_PIN_PIN) */

#if (UART_1_SPI_MASTER_SS0_PIN)
    /* Set SCB SPI to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS0_M_HSIOM_REG, UART_1_SS0_M_HSIOM_MASK,
                                   UART_1_SS0_M_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
#endif /* (UART_1_SPI_MASTER_SS0_PIN) */

#if (UART_1_SPI_MASTER_SS1_PIN)
    /* Set SCB SPI to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS1_M_HSIOM_REG, UART_1_SS1_M_HSIOM_MASK,
                                   UART_1_SS1_M_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
#endif /* (UART_1_SPI_MASTER_SS1_PIN) */

#if (UART_1_SPI_MASTER_SS2_PIN)
    /* Set SCB SPI to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS2_M_HSIOM_REG, UART_1_SS2_M_HSIOM_MASK,
                                   UART_1_SS2_M_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
#endif /* (UART_1_SPI_MASTER_SS2_PIN) */

#if (UART_1_SPI_MASTER_SS3_PIN)
    /* Set SCB SPI to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS3_M_HSIOM_REG, UART_1_SS3_M_HSIOM_MASK,
                                   UART_1_SS3_M_HSIOM_POS, UART_1_HSIOM_SPI_SEL);
#endif /* (UART_1_SPI_MASTER_SS3_PIN) */

#endif /* (UART_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: UART_1_SpiStop
********************************************************************************
*
* Summary:
*  Changes the HSIOM settings for the SPI master output pins (SCLK and/or SS0-SS3) to
*  keep them inactive after the block is disabled. The output pins are
*  controlled by the GPIO data register.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UART_1_SpiStop(void)
{
#if(UART_1_SCB_MODE_UNCONFIG_CONST_CFG)

    if (UART_1_CHECK_SPI_MASTER)
    {
    #if (UART_1_SCLK_PIN)
        /* Set output pin state after block is disabled */
        UART_1_spi_sclk_Write(UART_1_GET_SPI_SCLK_INACTIVE);

        /* Set GPIO to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SCLK_HSIOM_REG, UART_1_SCLK_HSIOM_MASK,
                                       UART_1_SCLK_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
    #endif /* (UART_1_MISO_SDA_TX_PIN_PIN) */

    #if (UART_1_SS0_PIN)
        /* Set output pin state after block is disabled */
        UART_1_spi_ss0_Write(UART_1_GET_SPI_SS0_INACTIVE);

        /* Set GPIO to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS0_HSIOM_REG, UART_1_SS0_HSIOM_MASK,
                                       UART_1_SS0_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
    #endif /* (UART_1_SS0_PIN) */

    #if (UART_1_SS1_PIN)
        /* Set output pin state after block is disabled */
        UART_1_spi_ss1_Write(UART_1_GET_SPI_SS1_INACTIVE);

        /* Set GPIO to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS1_HSIOM_REG, UART_1_SS1_HSIOM_MASK,
                                       UART_1_SS1_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
    #endif /* (UART_1_SS1_PIN) */

    #if (UART_1_SS2_PIN)
        /* Set output pin state after block is disabled */
        UART_1_spi_ss2_Write(UART_1_GET_SPI_SS2_INACTIVE);

        /* Set GPIO to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS2_HSIOM_REG, UART_1_SS2_HSIOM_MASK,
                                       UART_1_SS2_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
    #endif /* (UART_1_SS2_PIN) */

    #if (UART_1_SS3_PIN)
        /* Set output pin state after block is disabled */
        UART_1_spi_ss3_Write(UART_1_GET_SPI_SS3_INACTIVE);

        /* Set GPIO to drive output pin */
        UART_1_SET_HSIOM_SEL(UART_1_SS3_HSIOM_REG, UART_1_SS3_HSIOM_MASK,
                                       UART_1_SS3_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
    #endif /* (UART_1_SS3_PIN) */
    }

#else

#if (UART_1_SPI_MASTER_SCLK_PIN)
    /* Set output pin state after block is disabled */
    UART_1_sclk_m_Write(UART_1_GET_SPI_SCLK_INACTIVE);

    /* Set GPIO to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SCLK_M_HSIOM_REG, UART_1_SCLK_M_HSIOM_MASK,
                                   UART_1_SCLK_M_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
#endif /* (UART_1_MISO_SDA_TX_PIN_PIN) */

#if (UART_1_SPI_MASTER_SS0_PIN)
    /* Set output pin state after block is disabled */
    UART_1_ss0_m_Write(UART_1_GET_SPI_SS0_INACTIVE);

    /* Set GPIO to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS0_M_HSIOM_REG, UART_1_SS0_M_HSIOM_MASK,
                                   UART_1_SS0_M_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
#endif /* (UART_1_SPI_MASTER_SS0_PIN) */

#if (UART_1_SPI_MASTER_SS1_PIN)
    /* Set output pin state after block is disabled */
    UART_1_ss1_m_Write(UART_1_GET_SPI_SS1_INACTIVE);

    /* Set GPIO to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS1_M_HSIOM_REG, UART_1_SS1_M_HSIOM_MASK,
                                   UART_1_SS1_M_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
#endif /* (UART_1_SPI_MASTER_SS1_PIN) */

#if (UART_1_SPI_MASTER_SS2_PIN)
    /* Set output pin state after block is disabled */
    UART_1_ss2_m_Write(UART_1_GET_SPI_SS2_INACTIVE);

    /* Set GPIO to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS2_M_HSIOM_REG, UART_1_SS2_M_HSIOM_MASK,
                                   UART_1_SS2_M_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
#endif /* (UART_1_SPI_MASTER_SS2_PIN) */

#if (UART_1_SPI_MASTER_SS3_PIN)
    /* Set output pin state after block is disabled */
    UART_1_ss3_m_Write(UART_1_GET_SPI_SS3_INACTIVE);

    /* Set GPIO to drive output pin */
    UART_1_SET_HSIOM_SEL(UART_1_SS3_M_HSIOM_REG, UART_1_SS3_M_HSIOM_MASK,
                                   UART_1_SS3_M_HSIOM_POS, UART_1_HSIOM_GPIO_SEL);
#endif /* (UART_1_SPI_MASTER_SS3_PIN) */

#endif /* (UART_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (UART_1_SPI_MASTER_CONST)
    /*******************************************************************************
    * Function Name: UART_1_SetActiveSlaveSelect
    ********************************************************************************
    *
    * Summary:
    *  Selects one of the four slave select lines to be active during the transfer.
    *  After initialization the active slave select line is 0.
    *  The component should be in one of the following states to change the active
    *  slave select signal source correctly:
    *   - The component is disabled
    *   - The component has completed transfer (TX FIFO is empty and the
    *     SCB_INTR_MASTER_SPI_DONE status is set)
    *  This function does not check that these conditions are met.
    *  This function is only applicable to SPI Master mode of operation.
    *
    * Parameters:
    *  slaveSelect: slave select line which will be active while the following
    *               transfer.
    *   UART_1_SPI_SLAVE_SELECT0 - Slave select 0
    *   UART_1_SPI_SLAVE_SELECT1 - Slave select 1
    *   UART_1_SPI_SLAVE_SELECT2 - Slave select 2
    *   UART_1_SPI_SLAVE_SELECT3 - Slave select 3
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiSetActiveSlaveSelect(uint32 slaveSelect)
    {
        uint32 spiCtrl;

        spiCtrl = UART_1_SPI_CTRL_REG;

        spiCtrl &= (uint32) ~UART_1_SPI_CTRL_SLAVE_SELECT_MASK;
        spiCtrl |= (uint32)  UART_1_GET_SPI_CTRL_SS(slaveSelect);

        UART_1_SPI_CTRL_REG = spiCtrl;
    }
#endif /* (UART_1_SPI_MASTER_CONST) */


#if !(UART_1_CY_SCBIP_V0 || UART_1_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: UART_1_SpiSetSlaveSelectPolarity
    ********************************************************************************
    *
    * Summary:
    *  Sets active polarity for slave select line.
    *  The component should be in one of the following states to change the active
    *  slave select signal source correctly:
    *   - The component is disabled.
    *   - The component has completed transfer.
    *  This function does not check that these conditions are met.
    *
    * Parameters:
    *  slaveSelect: slave select line to change active polarity.
    *   UART_1_SPI_SLAVE_SELECT0 - Slave select 0
    *   UART_1_SPI_SLAVE_SELECT1 - Slave select 1
    *   UART_1_SPI_SLAVE_SELECT2 - Slave select 2
    *   UART_1_SPI_SLAVE_SELECT3 - Slave select 3
    *
    *  polarity: active polarity of slave select line.
    *   UART_1_SPI_SS_ACTIVE_LOW  - Slave select is active low
    *   UART_1_SPI_SS_ACTIVE_HIGH - Slave select is active high
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiSetSlaveSelectPolarity(uint32 slaveSelect, uint32 polarity)
    {
        uint32 ssPolarity;

        /* Get position of the polarity bit associated with slave select line */
        ssPolarity = UART_1_GET_SPI_CTRL_SSEL_POLARITY((uint32) 1u << slaveSelect);

        if (0u != polarity)
        {
            UART_1_SPI_CTRL_REG |= (uint32)  ssPolarity;
        }
        else
        {
            UART_1_SPI_CTRL_REG &= (uint32) ~ssPolarity;
        }
    }
#endif /* !(UART_1_CY_SCBIP_V0 || UART_1_CY_SCBIP_V1) */


#if(UART_1_SPI_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: UART_1_SpiSaveConfig
    ********************************************************************************
    *
    * Summary:
    *  Clears INTR_SPI_EC.WAKE_UP and enables it. This interrupt
    *  source triggers when the master assigns the SS line and wakes up the device.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiSaveConfig(void)
    {
        UART_1_ClearSpiExtClkInterruptSource(UART_1_INTR_SPI_EC_WAKE_UP);
        UART_1_SetSpiExtClkInterruptMode(UART_1_INTR_SPI_EC_WAKE_UP);
    }


    /*******************************************************************************
    * Function Name: UART_1_SpiRestoreConfig
    ********************************************************************************
    *
    * Summary:
    *  Disables the INTR_SPI_EC.WAKE_UP interrupt source. After wakeup
    *  slave does not drive the MISO line and the master receives 0xFF.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void UART_1_SpiRestoreConfig(void)
    {
        UART_1_SetSpiExtClkInterruptMode(UART_1_NO_INTR_SOURCES);
    }
#endif /* (UART_1_SPI_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
