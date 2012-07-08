/*******************************************************************************
* File Name: TimerInt.h
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
#if !defined(__TimerInt_INTC_H__)
#define __TimerInt_INTC_H__


#include <cytypes.h>
#include <cyfitter.h>

#if(CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3)     
    #if(CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_3A_ES2)      
        #include <intrins.h>
        #define TimerInt_ISR_PATCH() _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
    #endif
#endif


/* Interrupt Controller API. */
void TimerInt_Start(void);
void TimerInt_StartEx(cyisraddress address);
void TimerInt_Stop(void) ;

CY_ISR_PROTO(TimerInt_Interrupt);

void TimerInt_SetVector(cyisraddress address) ;
cyisraddress TimerInt_GetVector(void) ;

void TimerInt_SetPriority(uint8 priority) ;
uint8 TimerInt_GetPriority(void) ;

void TimerInt_Enable(void) ;
uint8 TimerInt_GetState(void) ;
void TimerInt_Disable(void) ;

void TimerInt_SetPending(void) ;
void TimerInt_ClearPending(void) ;


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the TimerInt ISR. */
#define TimerInt_INTC_VECTOR            ((reg16 *) TimerInt__INTC_VECT)

/* Address of the TimerInt ISR priority. */
#define TimerInt_INTC_PRIOR             ((reg8 *) TimerInt__INTC_PRIOR_REG)

/* Priority of the TimerInt interrupt. */
#define TimerInt_INTC_PRIOR_NUMBER      TimerInt__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable TimerInt interrupt. */
#define TimerInt_INTC_SET_EN            ((reg8 *) TimerInt__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the TimerInt interrupt. */
#define TimerInt_INTC_CLR_EN            ((reg8 *) TimerInt__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the TimerInt interrupt state to pending. */
#define TimerInt_INTC_SET_PD            ((reg8 *) TimerInt__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the TimerInt interrupt. */
#define TimerInt_INTC_CLR_PD            ((reg8 *) TimerInt__INTC_CLR_PD_REG)



/* __TimerInt_INTC_H__ */
#endif
