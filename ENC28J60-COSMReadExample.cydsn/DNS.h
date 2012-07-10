/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : DNS Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines the functions required for
               carrying out DNS Lookups.
*/

#ifndef DNS_H
#define DNS_H

/*******************************************************************************
* Function Name: DNSLookup
********************************************************************************
* Summary:
*   This function performs a DNSLookup and saves the returned IP Address,
*   (if any) in the variable serverIP defined in "globals.c"
*
* Parameters:
*   url - a constant character string that is the domain name to be
*         looked up. eg: DNSLookup("google.com");
*             
* Returns:
*   TRUE(0)- if the lookup was successful,and serverIP has been updated properly.
*   FALSE(1) - if the lookup was not successful.
*******************************************************************************/
unsigned int DNSLookup( const char* url );

#endif

/* [] END OF FILE */
