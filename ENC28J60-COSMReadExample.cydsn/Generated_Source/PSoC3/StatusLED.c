/*******************************************************************************
* File Name: StatusLED.c  
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
#include "StatusLED.h"


/*******************************************************************************
* Function Name: StatusLED_Write
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
void StatusLED_Write(uint8 value) 
{
    uint8 staticBits = StatusLED_DR & ~StatusLED_MASK;
    StatusLED_DR = staticBits | ((value << StatusLED_SHIFT) & StatusLED_MASK);
}


/*******************************************************************************
* Function Name: StatusLED_SetDriveMode
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
void StatusLED_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(StatusLED_0, mode);
}


/*******************************************************************************
* Function Name: StatusLED_Read
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
*  Macro StatusLED_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 StatusLED_Read(void) 
{
    return (StatusLED_PS & StatusLED_MASK) >> StatusLED_SHIFT;
}


/*******************************************************************************
* Function Name: StatusLED_ReadDataReg
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
uint8 StatusLED_ReadDataReg(void) 
{
    return (StatusLED_DR & StatusLED_MASK) >> StatusLED_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(StatusLED_INTSTAT) 

    /*******************************************************************************
    * Function Name: StatusLED_ClearInterrupt
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
    uint8 StatusLED_ClearInterrupt(void) 
    {
        return (StatusLED_INTSTAT & StatusLED_MASK) >> StatusLED_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */ 
