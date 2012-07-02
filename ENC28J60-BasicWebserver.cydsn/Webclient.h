/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Webclient functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Webserver client functionality that allows for basic requests/reply processing.
*/
unsigned int WebClient_ProcessReply(TCPhdr* Rpacket);
unsigned int WebClient_BrowseURL(unsigned char* query,TCPhdr* Tpacket);
unsigned int WebClient_Send(const char* Querydat);
//[] END OF FILE
