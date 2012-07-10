/*******************************************************************************
* File Name: DieTemp_DieTemp.h
* Version 1.80
*
* Description:
*  Defines the API to acquire the Die Temperature.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(__DieTemp_H__)
#define __DieTemp_H__


#include <CYTYPES.H>
#include <CYLIB.H>
#include <CYSPC.H>

#if !defined (CY_PSOC5LP)
    #error Component DieTemp_v1_80 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/***************************************
*       Function Prototypes
***************************************/

cystatus DieTemp_Start(void)                 ;
cystatus DieTemp_Query(int16 * temperature)  ;
cystatus DieTemp_GetTemp(int16 * temperature);
void DieTemp_Stop(void)                      ;


#endif /* __DieTemp_H__ */


/* [] END OF FILE */



