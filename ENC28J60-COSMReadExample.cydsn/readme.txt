ENC28J60 with the PSoC3
----------------------------------------------------------------------
This project aims to create an Internet connected PSoC3 based device.

Webclient COSM Read Feed Example:
-This project implements a Webserver at its IP,which displays the 
 current Die Temperature on a simple HTML page.
-This project performs a DNS Request for api.cosm.com and if successful in
 getting its IP,proceeds to make an HTTP GET Request as follows.
-It sends a GET Request to COSM,with the intention of 
 reading a datastream of a feed and displays the result on an attached character LCD.

The feed used in this example can be viewed here : https://cosm.com/feeds/65875


-----------------------------------------------------------------------

This code has been tested with:
  Hardware: CY8CKIT-030 with ENC28J60 module from http://www.embeddedmarket.com/products/ENC28J60-Ethernet-Module/
  Software: PSoC Creator 2.1 with cy_boot 3.0
  
Based heavily on the work by Duncan Frost for the MSP430.
Credits and sincere thanks to him.


-kmmankad

http://kmmankad.blogspot.com

This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.

Last Updated on 10-July-12.