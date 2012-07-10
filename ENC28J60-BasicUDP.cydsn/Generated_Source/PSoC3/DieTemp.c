/*******************************************************************************
* File Name: DieTemp_DieTemp.c  
* Version 1.80
*
* Description:
*  Provides the API to acquire the die temperature.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <CYDEVICE_TRM.H>
#include <DieTemp.H>


#define DieTemp_NUMBER_OF_SAMPLES  0x1u
#define DieTemp_TIMER_PERIOD       0xFFFu
#define DieTemp_CLK_DIV_SELECT     0x4u

#define SPC_CLK_PERIOD                      120 /* nano sec. */

#define NANO_TO_MILLI_FACTOR                1000000
#define FRM_EXEC_TIME                       (1000) /* In nano seconds. */
#define GET_TEMPR_TIME                      (1 << (DieTemp_NUMBER_OF_SAMPLES + 1)) * \
                                            (SPC_CLK_PERIOD * DieTemp_CLK_DIV_SELECT) * \
                                            DieTemp_TIMER_PERIOD + \
                                            FRM_EXEC_TIME

#define DieTemp_MAX_WAIT           ((GET_TEMPR_TIME + NANO_TO_MILLI_FACTOR - 1) / NANO_TO_MILLI_FACTOR) /* In milli seconds. */


/*******************************************************************************
* Function Name: DieTemp_Start
********************************************************************************
* 
* Summary:
*  Starts the SPC command to get the die temperature. If this function is
*  called successfuly. The SPC will be locked and DieTemp_QueryDieTemp
*  will have to be successfuly called to unlock it. CySpcUnlock() can also
*  be called if the caller decides not to finish the temperature reading.
*
* Parameters:
*  void.
*
* Return:
*  CYRET_STARTED if the spc command was started sucessfuly.
*  CYRET_UNKNOWN if the spc command failed.
*  CYRET_LOCKED if the spc was busy.
*
*******************************************************************************/
cystatus DieTemp_Start(void) 
{
    cystatus status;
    
    /* Plan for failure. */
    status = CYRET_UNKNOWN;
    
    /* Power up the SPC. */
    CySpcStart();

    if(CySpcLock() == CYRET_SUCCESS)
    {
        #if(CY_PSOC5A)
            if(CYRET_STARTED == CySpcGetTemp(DieTemp_NUMBER_OF_SAMPLES,
                                             DieTemp_TIMER_PERIOD,
                                             DieTemp_CLK_DIV_SELECT))
            {
                status = CYRET_STARTED;
            }
        #else
            if(CYRET_STARTED == CySpcGetTemp(DieTemp_NUMBER_OF_SAMPLES))
            {
                status = CYRET_STARTED;
            }
        #endif  /* (CY_PSOC5A) */
        CySpcUnlock();
    }
    else
    {
        status = CYRET_LOCKED;
    }
    return (status);
}


/*******************************************************************************
* FUNCTION NAME:   DieTemp_Stop
********************************************************************************
*
* Summary:
*  Stops the temperature reading
*
* Parameters:  
*  void
*
* Return: 
*  void
*
*******************************************************************************/
void DieTemp_Stop(void) 
{
    CySpcStop();
}


/*******************************************************************************
* Function Name: DieTemp_Query
********************************************************************************
*
* Summary:
*  Querys the SPC to see if the termperature command is finished.
*   
* Parameters:
*  temperature: Address to store the two byte temperature value.
* 
* Return:
*  CYRET_SUCCESS if the temperature command completed sucessfully.
*  CYRET_UNKNOWN if the there was an spc failure.
*  CYRET_STARTED if the temperature command has not completed.
*  CYRET_TIMEOUT if waited to long before reading data.
*
*******************************************************************************/
cystatus DieTemp_Query(int16 * temperature) 
{
    uint8 temp[2];
    cystatus status;


    if(CY_SPC_BUSY)
    {
        if((CY_SPC_STATUS_REG>>2) == CYRET_SUCCESS)
        {
            /* See if any data is ready. */
            if(CySpcReadData(temp, 2) == 2u)
            {
                if(temp[0])
                {
                    *temperature = (int16) ((uint16) temp[1]);
                }
                else
                {
                    *temperature = (int16) - ((uint16) temp[1]);
                }
                status = CYRET_SUCCESS;
            }
            else
            {
                status = CYRET_UNKNOWN;
            }
        }
        else
        {
            status = CYRET_STARTED;
        }
    }
    else
    {
        status = CYRET_TIMEOUT;
    }

    if(status != CYRET_STARTED)
    {
        /* Unlock the SPC so someone else can use it. */
        CySpcUnlock();
    }
    return (status);
}


/*******************************************************************************
* Function Name: DieTemp_GetTemp
********************************************************************************
*
* Summary:
*  Sets up the command to get the temperature and blocks until finished. After
*  DieTemp_MAX_WAIT ticks the function will return even if the
*  SPC has not finished.
*   
* Parameters:
*  temperature: Address to store the two byte temperature value.
*
* Return:
*  CYRET_SUCCESS if the command was completed sucessfuly.
*  Status code from DieTemp_Start or DieTemp_Query
*
*******************************************************************************/
cystatus DieTemp_GetTempT(int16 * temperature) 
{
    uint16 ms;
    cystatus status;

    /* Start the temp reading */
    status = DieTemp_Start();

    if(status == CYRET_STARTED)
    {
        for (ms=DieTemp_MAX_WAIT; ms>0; ms--)
        {
            status = DieTemp_Query(temperature);
            if(status != CYRET_STARTED)
            {
                break;
            }
            CyDelay(1);
        }
    }

    return status;
}
cystatus DieTemp_GetTemp(int16 * temperature) 
{
    cystatus status;
    uint8 count = 2;
    while (count!=0)
    {
        status = DieTemp_GetTempT(temperature);
        if (status != CYRET_SUCCESS) 
        {
            break;
        }
        count--;
    }
    return status;
}


/* [] END OF FILE */
