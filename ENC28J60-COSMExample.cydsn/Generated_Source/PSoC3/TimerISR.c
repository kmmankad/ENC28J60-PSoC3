/*******************************************************************************
* File Name: TimerISR.c  
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
#include <TimerISR.H>


/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START TimerISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: TimerISR_Start
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
void TimerISR_Start(void)
{
    /* For all we know the interrupt is active. */
    TimerISR_Disable();

    /* Set the ISR to point to the TimerISR Interrupt. */
    TimerISR_SetVector(TimerISR_Interrupt);

    /* Set the priority. */
    TimerISR_SetPriority(TimerISR_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TimerISR_Enable();
}

/*******************************************************************************
* Function Name: TimerISR_StartEx
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
void TimerISR_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    TimerISR_Disable();

    /* Set the ISR to point to the TimerISR Interrupt. */
    TimerISR_SetVector(address);

    /* Set the priority. */
    TimerISR_SetPriority(TimerISR_INTC_PRIOR_NUMBER);

    /* Enable it. */
    TimerISR_Enable();
}

/*******************************************************************************
* Function Name: TimerISR_Stop
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
void TimerISR_Stop(void) 
{
    /* Disable this interrupt. */
    TimerISR_Disable();
}

/*******************************************************************************
* Function Name: TimerISR_Interrupt
********************************************************************************
* Summary:
*   The default Interrupt Service Routine for TimerISR.
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
CY_ISR(TimerISR_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START TimerISR_Interrupt` */

    /* `#END` */

    /* PSoC3 ES1, ES2 RTC ISR PATCH  */ 
    #if(CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3)
        #if((CYDEV_CHIP_REVISION_USED <= CYDEV_CHIP_REVISION_3A_ES2) && (TimerISR__ES2_PATCH ))      
            TimerISR_ISR_PATCH();
        #endif
    #endif
}

/*******************************************************************************
* Function Name: TimerISR_SetVector
********************************************************************************
* Summary:
*   Change the ISR vector for the Interrupt. Note calling TimerISR_Start
*   will override any effect this method would have had. To set the vector before
*   the component has been started use TimerISR_StartEx instead.
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
void TimerISR_SetVector(cyisraddress address) 
{
    CY_SET_REG16(TimerISR_INTC_VECTOR, (uint16) address);
}

/*******************************************************************************
* Function Name: TimerISR_GetVector
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
cyisraddress TimerISR_GetVector(void) 
{
    return (cyisraddress) CY_GET_REG16(TimerISR_INTC_VECTOR);
}

/*******************************************************************************
* Function Name: TimerISR_SetPriority
********************************************************************************
* Summary:
*   Sets the Priority of the Interrupt. Note calling TimerISR_Start
*   or TimerISR_StartEx will override any effect this method would have had. 
*	This method should only be called after TimerISR_Start or 
*	TimerISR_StartEx has been called. To set the initial
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
void TimerISR_SetPriority(uint8 priority) 
{
    *TimerISR_INTC_PRIOR = priority << 5;
}

/*******************************************************************************
* Function Name: TimerISR_GetPriority
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
uint8 TimerISR_GetPriority(void) 
{
    uint8 priority;


    priority = *TimerISR_INTC_PRIOR >> 5;

    return priority;
}

/*******************************************************************************
* Function Name: TimerISR_Enable
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
void TimerISR_Enable(void) 
{
    /* Enable the general interrupt. */
    *TimerISR_INTC_SET_EN = TimerISR__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerISR_GetState
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
uint8 TimerISR_GetState(void) 
{
    /* Get the state of the general interrupt. */
    return (*TimerISR_INTC_SET_EN & TimerISR__INTC_MASK) ? 1:0;
}

/*******************************************************************************
* Function Name: TimerISR_Disable
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
void TimerISR_Disable(void) 
{
    /* Disable the general interrupt. */
    *TimerISR_INTC_CLR_EN = TimerISR__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerISR_SetPending
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
void TimerISR_SetPending(void) 
{
    *TimerISR_INTC_SET_PD = TimerISR__INTC_MASK;
}

/*******************************************************************************
* Function Name: TimerISR_ClearPending
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
void TimerISR_ClearPending(void) 
{
    *TimerISR_INTC_CLR_PD = TimerISR__INTC_MASK;
}
