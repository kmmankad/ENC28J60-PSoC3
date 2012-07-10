/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Global Variables declaration file
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains declarations of
               the global variables used.

 You may edit Router IP,DNS Server IP and Webserver,Webclient Port here.
*/

#include "globals.h"

/**Edit values as per your configuration**/

/*IP Address of the Router*/
unsigned char routerIP[4]={192,168,1,1};

/*IP Address of a Server*/
/*Which the DNS Lookup will fill up,and the webclient will query to*/
 unsigned char serverIP[4];
 
/*IP Address of the DNS Server*/
unsigned char dnsIP[4]={192,168,1,1};
 
/*Webserver Port*/
unsigned int WWWPort=80;

/*WebClient Port*/
unsigned int WClientPort=19979;

/*Source UDP Packet for sending UDP Packets*/
unsigned int UDPPort=1200;
/***********************************************/
/***Do not assign values to variables below.***/

/*IP Address of the ENC28J60*/
unsigned char deviceIP[4];

/*MAC Address of the ENC28J60*/
unsigned char deviceMAC[6];

/*MAC Address of the Router*/
unsigned char routerMAC[6];
unsigned char WebClientStatus=0;

unsigned char WebClientQuery[250];
/* [] END OF FILE */