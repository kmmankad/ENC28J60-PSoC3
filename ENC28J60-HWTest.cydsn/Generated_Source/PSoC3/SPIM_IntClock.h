/*******************************************************************************
* File Name: SPIM_IntClock.h
* Version 1.70
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_CLOCK_SPIM_IntClock_H)
#define CY_CLOCK_SPIM_IntClock_H

#include <cytypes.h>
#include <cyfitter.h>

/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v1_70 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

/***************************************
*        Function Prototypes
***************************************/

void SPIM_IntClock_Start(void) ;
void SPIM_IntClock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void SPIM_IntClock_StopBlock(void) ;
#endif

void SPIM_IntClock_StandbyPower(uint8 state) ;
void SPIM_IntClock_SetDividerRegister(uint16 clkDivider, uint8 reset) ;
uint16 SPIM_IntClock_GetDividerRegister(void) ;
void SPIM_IntClock_SetModeRegister(uint8 modeBitMask) ;
void SPIM_IntClock_ClearModeRegister(uint8 modeBitMask) ;
uint8 SPIM_IntClock_GetModeRegister(void) ;
void SPIM_IntClock_SetSourceRegister(uint8 clkSource) ;
uint8 SPIM_IntClock_GetSourceRegister(void) ;
#if defined(SPIM_IntClock__CFG3)
void SPIM_IntClock_SetPhaseRegister(uint8 clkPhase) ;
uint8 SPIM_IntClock_GetPhaseRegister(void) ;
#endif

#define SPIM_IntClock_Enable()                       SPIM_IntClock_Start()
#define SPIM_IntClock_Disable()                      SPIM_IntClock_Stop()
#define SPIM_IntClock_SetDivider(clkDivider)         SPIM_IntClock_SetDividerRegister(clkDivider, 1)
#define SPIM_IntClock_SetDividerValue(clkDivider)    SPIM_IntClock_SetDividerRegister((clkDivider) - 1, 1)
#define SPIM_IntClock_SetMode(clkMode)               SPIM_IntClock_SetModeRegister(clkMode)
#define SPIM_IntClock_SetSource(clkSource)           SPIM_IntClock_SetSourceRegister(clkSource)
#if defined(SPIM_IntClock__CFG3)
#define SPIM_IntClock_SetPhase(clkPhase)             SPIM_IntClock_SetPhaseRegister(clkPhase)
#define SPIM_IntClock_SetPhaseValue(clkPhase)        SPIM_IntClock_SetPhaseRegister((clkPhase) + 1)
#endif


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define SPIM_IntClock_CLKEN              (* (reg8 *) SPIM_IntClock__PM_ACT_CFG)
#define SPIM_IntClock_CLKEN_PTR          ((reg8 *) SPIM_IntClock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define SPIM_IntClock_CLKSTBY            (* (reg8 *) SPIM_IntClock__PM_STBY_CFG)
#define SPIM_IntClock_CLKSTBY_PTR        ((reg8 *) SPIM_IntClock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define SPIM_IntClock_DIV_LSB            (* (reg8 *) SPIM_IntClock__CFG0)
#define SPIM_IntClock_DIV_LSB_PTR        ((reg8 *) SPIM_IntClock__CFG0)
#define SPIM_IntClock_DIV_PTR            ((reg16 *) SPIM_IntClock__CFG0)

/* Clock MSB divider configuration register. */
#define SPIM_IntClock_DIV_MSB            (* (reg8 *) SPIM_IntClock__CFG1)
#define SPIM_IntClock_DIV_MSB_PTR        ((reg8 *) SPIM_IntClock__CFG1)

/* Mode and source configuration register */
#define SPIM_IntClock_MOD_SRC            (* (reg8 *) SPIM_IntClock__CFG2)
#define SPIM_IntClock_MOD_SRC_PTR        ((reg8 *) SPIM_IntClock__CFG2)

#if defined(SPIM_IntClock__CFG3)
/* Analog clock phase configuration register */
#define SPIM_IntClock_PHASE              (* (reg8 *) SPIM_IntClock__CFG3)
#define SPIM_IntClock_PHASE_PTR          ((reg8 *) SPIM_IntClock__CFG3)
#endif


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define SPIM_IntClock_CLKEN_MASK         SPIM_IntClock__PM_ACT_MSK
#define SPIM_IntClock_CLKSTBY_MASK       SPIM_IntClock__PM_STBY_MSK

/* CFG2 field masks */
#define SPIM_IntClock_SRC_SEL_MSK        SPIM_IntClock__CFG2_SRC_SEL_MASK
#define SPIM_IntClock_MODE_MASK          (~(SPIM_IntClock_SRC_SEL_MSK))

#if defined(SPIM_IntClock__CFG3)
/* CFG3 phase mask */
#define SPIM_IntClock_PHASE_MASK         SPIM_IntClock__CFG3_PHASE_DLY_MASK
#endif

#endif /* CY_CLOCK_SPIM_IntClock_H */


/* [] END OF FILE */
