/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Webclient functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Webserver client functionality that allows for basic requests/reply processing.
*/
#ifndef WEBCLIENT_H
#define WEBCLIENT_H
/*******************************************************************************
* Function Name: WebClient_ProcessReply
********************************************************************************
* Summary:
*   Process the data sent as a reply from the webserver.
*   **Add all your reply parsing code in this function.**
* Parameters:
*   Rpacket - pointer to a TCP Packet that has the data.
*              
* Returns:
*   customize as required.
*******************************************************************************/
unsigned int WebClient_ProcessReply(TCPhdr* Rpacket);

/*******************************************************************************
* Function Name: WebClient_BrowseURL
********************************************************************************
* Summary:
*   Sends the query stored in the WebClientQuery global variable.
*   Do not call this function in usercode,its part of the state machine in IPStack.C
*	It is called by the state machine after the SYN-SYNACK-ACK handshake is over.
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the Query was successfully sent.
*   FALSE(1) - if the Query was not successful in transmission.
*******************************************************************************/
unsigned int WebClient_BrowseURL(TCPhdr* Tpacket);

/*******************************************************************************
* Function Name: WebClient_Send
********************************************************************************
* Summary:
*   Sends the query stored in the WebClientQuery global variable.
*	(Actually just commands the SYN Packet to initiate the HTTP Request with the server.)
*   Ensure that you have stored the appropriate query in,before calling this.
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the SYN for the Query was successfully signalled for a send.
*   FALSE(1) - if the SYN for the Query was not successful in signalled for a send.
*******************************************************************************/
unsigned int WebClient_Send();

/*******************************************************************************
* Function Name: WebClient_SendSYN
********************************************************************************
* Summary:
*   Sends a SYN packet to the server to initiate the 3 way TCP handshake.
*
* Parameters:
*  none.
*              
* Returns:
*   TRUE(0)- if the SYN Packet was successfully sent.
*   FALSE(1) - if the SYN Packet was not successful in transmission.
*******************************************************************************/
unsigned int WebClient_SendSYN(void);

#endif
/* [] END OF FILE */
