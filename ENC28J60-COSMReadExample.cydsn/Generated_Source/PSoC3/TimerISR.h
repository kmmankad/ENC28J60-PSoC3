/*******************************************************************************
* File Name: TimerISR.h
* Version 1.60
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/
#if !defined(__TimerISR_INTC_H__)
#define __TimerISR_INTC_H__


#include <cytypes.h>
#include <cyfitter.h>

#if(CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3)     
    #if(CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_3A_ES2)      
        #include <intrins.h>
        #define TimerISR_ISR_PATCH() _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
    #endif
#endif


/* Interrupt Controller API. */
void TimerISR_Start(void);
void TimerISR_StartEx(cyisraddress address);
void TimerISR_Stop(void) ;

CY_ISR_PROTO(TimerISR_Interrupt);

void TimerISR_SetVector(cyisraddress address) ;
cyisraddress TimerISR_GetVector(void) ;

void TimerISR_SetPriority(uint8 priority) ;
uint8 TimerISR_GetPriority(void) ;

void TimerISR_Enable(void) ;
uint8 TimerISR_GetState(void) ;
void TimerISR_Disable(void) ;

void TimerISR_SetPending(void) ;
void TimerISR_ClearPending(void) ;


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the TimerISR ISR. */
#define TimerISR_INTC_VECTOR            ((reg16 *) TimerISR__INTC_VECT)

/* Address of the TimerISR ISR priority. */
#define TimerISR_INTC_PRIOR             ((reg8 *) TimerISR__INTC_PRIOR_REG)

/* Priority of the TimerISR interrupt. */
#define TimerISR_INTC_PRIOR_NUMBER      TimerISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable TimerISR interrupt. */
#define TimerISR_INTC_SET_EN            ((reg8 *) TimerISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the TimerISR interrupt. */
#define TimerISR_INTC_CLR_EN            ((reg8 *) TimerISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the TimerISR interrupt state to pending. */
#define TimerISR_INTC_SET_PD            ((reg8 *) TimerISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the TimerISR interrupt. */
#define TimerISR_INTC_CLR_PD            ((reg8 *) TimerISR__INTC_CLR_PD_REG)



/* __TimerISR_INTC_H__ */
#endif
