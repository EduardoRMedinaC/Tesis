/*******************************************************************************
* File Name: UART_CLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_UART_CLK_H)
#define CY_CLOCK_UART_CLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void UART_CLK_StartEx(uint32 alignClkDiv);
#define UART_CLK_Start() \
    UART_CLK_StartEx(UART_CLK__PA_DIV_ID)

#else

void UART_CLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void UART_CLK_Stop(void);

void UART_CLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 UART_CLK_GetDividerRegister(void);
uint8  UART_CLK_GetFractionalDividerRegister(void);

#define UART_CLK_Enable()                         UART_CLK_Start()
#define UART_CLK_Disable()                        UART_CLK_Stop()
#define UART_CLK_SetDividerRegister(clkDivider, reset)  \
    UART_CLK_SetFractionalDividerRegister((clkDivider), 0u)
#define UART_CLK_SetDivider(clkDivider)           UART_CLK_SetDividerRegister((clkDivider), 1u)
#define UART_CLK_SetDividerValue(clkDivider)      UART_CLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define UART_CLK_DIV_ID     UART_CLK__DIV_ID

#define UART_CLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define UART_CLK_CTRL_REG   (*(reg32 *)UART_CLK__CTRL_REGISTER)
#define UART_CLK_DIV_REG    (*(reg32 *)UART_CLK__DIV_REGISTER)

#define UART_CLK_CMD_DIV_SHIFT          (0u)
#define UART_CLK_CMD_PA_DIV_SHIFT       (8u)
#define UART_CLK_CMD_DISABLE_SHIFT      (30u)
#define UART_CLK_CMD_ENABLE_SHIFT       (31u)

#define UART_CLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << UART_CLK_CMD_DISABLE_SHIFT))
#define UART_CLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << UART_CLK_CMD_ENABLE_SHIFT))

#define UART_CLK_DIV_FRAC_MASK  (0x000000F8u)
#define UART_CLK_DIV_FRAC_SHIFT (3u)
#define UART_CLK_DIV_INT_MASK   (0xFFFFFF00u)
#define UART_CLK_DIV_INT_SHIFT  (8u)

#else 

#define UART_CLK_DIV_REG        (*(reg32 *)UART_CLK__REGISTER)
#define UART_CLK_ENABLE_REG     UART_CLK_DIV_REG
#define UART_CLK_DIV_FRAC_MASK  UART_CLK__FRAC_MASK
#define UART_CLK_DIV_FRAC_SHIFT (16u)
#define UART_CLK_DIV_INT_MASK   UART_CLK__DIVIDER_MASK
#define UART_CLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_UART_CLK_H) */

/* [] END OF FILE */
