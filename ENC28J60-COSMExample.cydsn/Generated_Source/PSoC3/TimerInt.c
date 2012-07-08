/*******************************************************************************
* File Name: TimerInt.c  
* Version 1.60
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/


#include <CYDEVICE.H>
#include <CYDEVICE_TRM.H>
#include <CYLIB.H>
#include <TimerInt.H>


/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START TimerInt_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: TimerInt_Start
********************************************************************************
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   void.
*
*
* Return:
*   void.
*
*******************************************************************************/
void TimerInt_Start(void)
{
    /* For all we know the interrupt is active. */
    TimerInt_Disable();

    /* Set the ISR to point to the TimerInt Interrupt. */
    TimerInt_SetVector(TimerInt_Interrupt);

    /* Set the priority. */
    TimerInt_SetPriority(TimerInt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TimerInt_Enable();
}

/*******************************************************************************
* Function Name: TimerInt_StartEx
********************************************************************************
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
*
* Return:
*   void.
*
*******************************************************************************/
void TimerInt_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    TimerInt_Disable();

    /* Set the ISR to point to the TimerInt Interrupt. */
    TimerInt_SetVector(address);

    /* Set the priority. */
    TimerInt_SetPriority(TimerInt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TimerInt_Enable();
}

/*******************************************************************************
* Function Name: TimerInt_Stop
********************************************************************************
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
*
* Return:
*   void.
*
*******************************************************************************/
void TimerInt_Stop(void) 
{
    /* Disable this interrupt. */
    TimerInt_Disable();
}

/*******************************************************************************
* Function Name: TimerInt_Interrupt
********************************************************************************
* Summary:
*   The default Interrupt Service Routine for TimerInt.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
*
*
* Parameters:  
*
*
* Return:
*   void.
*
*******************************************************************************/
CY_ISR(TimerInt_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START TimerInt_Interrupt` */

    /* `#END` */

    /* PSoC3 ES1, ES2 RTC ISR PATCH  */ 
    #if(CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3)
        #if((CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_3A_ES2) && (TimerInt__ES2_PATCH ))      
            TimerInt_ISR_PATCH();
        #endif
    #endif
}

/*******************************************************************************
* Function Name: TimerInt_SetVector
********************************************************************************
* Summary:
*   Change the ISR vector for the Interrupt. Note calling TimerInt_Start
*   will override any effect this method would have had. To set the vector before
*   the component has been started use TimerInt_StartEx instead.
*
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_SetVector(cyisraddress address) 
{
    CY_SET_REG16(TimerInt_INTC_VECTOR, (uint16) address);
}

/*******************************************************************************
* Function Name: TimerInt_GetVector
********************************************************************************
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*
*******************************************************************************/
cyisraddress TimerInt_GetVector(void) 
{
    return (cyisraddress) CY_GET_REG16(TimerInt_INTC_VECTOR);
}

/*******************************************************************************
* Function Name: TimerInt_SetPriority
********************************************************************************
* Summary:
*   Sets the Priority of the Interrupt. Note calling TimerInt_Start
*   or TimerInt_StartEx will override any effect this method would have had. 
*	This method should only be called after TimerInt_Start or 
*	TimerInt_StartEx has been called. To set the initial
*	priority for the component use the cydwr file in the tool.
*
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 7, 0 being the highest.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_SetPriority(uint8 priority) 
{
    *TimerInt_INTC_PRIOR = priority << 5;
}

/*******************************************************************************
* Function Name: TimerInt_GetPriority
********************************************************************************
* Summary:
*   Gets the Priority of the Interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   Priority of the interrupt. 0 - 7, 0 being the highest.
*
*
*******************************************************************************/
uint8 TimerInt_GetPriority(void) 
{
    uint8 priority;


    priority = *TimerInt_INTC_PRIOR >> 5;

    return priority;
}

/*******************************************************************************
* Function Name: TimerInt_Enable
********************************************************************************
* Summary:
*   Enables the interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_Enable(void) 
{
    /* Enable the general interrupt. */
    *TimerInt_INTC_SET_EN = TimerInt__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerInt_GetState
********************************************************************************
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   1 if enabled, 0 if disabled.
*
*
*******************************************************************************/
uint8 TimerInt_GetState(void) 
{
    /* Get the state of the general interrupt. */
    return (*TimerInt_INTC_SET_EN & TimerInt__INTC_MASK) ? 1:0;
}

/*******************************************************************************
* Function Name: TimerInt_Disable
********************************************************************************
* Summary:
*   Disables the Interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_Disable(void) 
{
    /* Disable the general interrupt. */
    *TimerInt_INTC_CLR_EN = TimerInt__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerInt_SetPending
********************************************************************************
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
*
* Parameters:
*   void.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_SetPending(void) 
{
    *TimerInt_INTC_SET_PD = TimerInt__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerInt_ClearPending
********************************************************************************
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   void.
*
*
* Return:
*   void.
*
*
*******************************************************************************/
void TimerInt_ClearPending(void) 
{
    *TimerInt_INTC_CLR_PD = TimerInt__INTC_MASK;
}
