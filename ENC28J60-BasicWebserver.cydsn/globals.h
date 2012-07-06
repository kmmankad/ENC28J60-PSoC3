/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Global Variables Header file
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines(extern) the global variables used.
 
 Do not edit variables here.Edit them in "globals.c" if you want.
*/

/*IP Address of the ENC28J60*/
extern unsigned char deviceIP[4];

/*MAC Address of the ENC28J60*/
extern unsigned char deviceMAC[6];

/*IP Address of the Router*/
extern unsigned char routerIP[4];

/*MAC Address of the Router*/
extern unsigned char routerMAC[6];

/*IP Address of a Server*/
/*For DNS and Webclient*/
 extern unsigned char serverIP[4];
 
/*IP Address of the DNS Server*/
extern unsigned char dnsIP[4];

/*Webserver Port*/
extern unsigned int WWWPort;

extern unsigned int WClientPort;

/*Source Port for UDP Transmission*/
extern unsigned int UDPPort;

extern unsigned char WebClientStatus;

extern unsigned char WebClientQuery[250];
#define TRUE  0
#define FALSE 1

/* [] END OF FILE */
