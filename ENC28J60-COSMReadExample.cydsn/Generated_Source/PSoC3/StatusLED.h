/*******************************************************************************
* File Name: StatusLED.h  
* Version 1.70
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PINS_StatusLED_H) /* Pins StatusLED_H */
#define CY_PINS_StatusLED_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "StatusLED_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    StatusLED_Write(uint8 value) ;
void    StatusLED_SetDriveMode(uint8 mode) ;
uint8   StatusLED_ReadDataReg(void) ;
uint8   StatusLED_Read(void) ;
uint8   StatusLED_ClearInterrupt(void) ;

/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define StatusLED_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define StatusLED_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define StatusLED_DM_RES_UP          PIN_DM_RES_UP
#define StatusLED_DM_RES_DWN         PIN_DM_RES_DWN
#define StatusLED_DM_OD_LO           PIN_DM_OD_LO
#define StatusLED_DM_OD_HI           PIN_DM_OD_HI
#define StatusLED_DM_STRONG          PIN_DM_STRONG
#define StatusLED_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define StatusLED_MASK               StatusLED__MASK
#define StatusLED_SHIFT              StatusLED__SHIFT
#define StatusLED_WIDTH              1u

/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define StatusLED_PS                     (* (reg8 *) StatusLED__PS)
/* Data Register */
#define StatusLED_DR                     (* (reg8 *) StatusLED__DR)
/* Port Number */
#define StatusLED_PRT_NUM                (* (reg8 *) StatusLED__PRT) 
/* Connect to Analog Globals */                                                  
#define StatusLED_AG                     (* (reg8 *) StatusLED__AG)                       
/* Analog MUX bux enable */
#define StatusLED_AMUX                   (* (reg8 *) StatusLED__AMUX) 
/* Bidirectional Enable */                                                        
#define StatusLED_BIE                    (* (reg8 *) StatusLED__BIE)
/* Bit-mask for Aliased Register Access */
#define StatusLED_BIT_MASK               (* (reg8 *) StatusLED__BIT_MASK)
/* Bypass Enable */
#define StatusLED_BYP                    (* (reg8 *) StatusLED__BYP)
/* Port wide control signals */                                                   
#define StatusLED_CTL                    (* (reg8 *) StatusLED__CTL)
/* Drive Modes */
#define StatusLED_DM0                    (* (reg8 *) StatusLED__DM0) 
#define StatusLED_DM1                    (* (reg8 *) StatusLED__DM1)
#define StatusLED_DM2                    (* (reg8 *) StatusLED__DM2) 
/* Input Buffer Disable Override */
#define StatusLED_INP_DIS                (* (reg8 *) StatusLED__INP_DIS)
/* LCD Common or Segment Drive */
#define StatusLED_LCD_COM_SEG            (* (reg8 *) StatusLED__LCD_COM_SEG)
/* Enable Segment LCD */
#define StatusLED_LCD_EN                 (* (reg8 *) StatusLED__LCD_EN)
/* Slew Rate Control */
#define StatusLED_SLW                    (* (reg8 *) StatusLED__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define StatusLED_PRTDSI__CAPS_SEL       (* (reg8 *) StatusLED__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define StatusLED_PRTDSI__DBL_SYNC_IN    (* (reg8 *) StatusLED__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define StatusLED_PRTDSI__OE_SEL0        (* (reg8 *) StatusLED__PRTDSI__OE_SEL0) 
#define StatusLED_PRTDSI__OE_SEL1        (* (reg8 *) StatusLED__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define StatusLED_PRTDSI__OUT_SEL0       (* (reg8 *) StatusLED__PRTDSI__OUT_SEL0) 
#define StatusLED_PRTDSI__OUT_SEL1       (* (reg8 *) StatusLED__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define StatusLED_PRTDSI__SYNC_OUT       (* (reg8 *) StatusLED__PRTDSI__SYNC_OUT) 


#if defined(StatusLED__INTSTAT)  /* Interrupt Registers */

    #define StatusLED_INTSTAT                (* (reg8 *) StatusLED__INTSTAT)
    #define StatusLED_SNAP                   (* (reg8 *) StatusLED__SNAP)

#endif /* Interrupt Registers */

#endif /* End Pins StatusLED_H */


/* [] END OF FILE */
