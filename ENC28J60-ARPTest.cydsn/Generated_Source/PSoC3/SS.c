/*******************************************************************************
* File Name: SS.c  
* Version 1.70
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "cytypes.h"
#include "SS.h"


/*******************************************************************************
* Function Name: SS_Write
********************************************************************************
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  void 
*  
*******************************************************************************/
void SS_Write(uint8 value) 
{
    uint8 staticBits = SS_DR & ~SS_MASK;
    SS_DR = staticBits | ((value << SS_SHIFT) & SS_MASK);
}


/*******************************************************************************
* Function Name: SS_SetDriveMode
********************************************************************************
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to this drive mode.
*
* Return: 
*  void
*
*******************************************************************************/
void SS_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(SS_0, mode);
}


/*******************************************************************************
* Function Name: SS_Read
********************************************************************************
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  void 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro SS_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 SS_Read(void) 
{
    return (SS_PS & SS_MASK) >> SS_SHIFT;
}


/*******************************************************************************
* Function Name: SS_ReadDataReg
********************************************************************************
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  void 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 SS_ReadDataReg(void) 
{
    return (SS_DR & SS_MASK) >> SS_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(SS_INTSTAT) 

    /*******************************************************************************
    * Function Name: SS_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  void 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 SS_ClearInterrupt(void) 
    {
        return (SS_INTSTAT & SS_MASK) >> SS_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */ 
