/*
File:   main.c
Author: Erin Wang
Email:  erinwang@hmc.edu
Date:   10/22/25
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "STM32L432KC.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* tempStr = "<p>Temperature Resolution Control:</p><form action=\"8bit\"><input type=\"submit\" value=\"Temp res 8-bit!\"></form>\
	<form action=\"9bit\"><input type=\"submit\" value=\"Temp res 9-bit!\"></form>\
	<form action=\"10bit\"><input type=\"submit\" value=\"Temp res 10-bit!!\"></form>\
	<form action=\"11bit\"><input type=\"submit\" value=\"Temp res 11-bit!\"></form>\
	<form action=\"12bit\"><input type=\"submit\" value=\"Temp res 12-bit!\"></form>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

/* Extrapolates the temperature resolution from an input request from the website
 *    -- request: char array string from user input on the website
 *    -- return: a char holding the temperature resolution to send */
char updateTempRes(char request[]) {
      char temp_res;
      if (inString(request, "8bit")==1)        temp_res = 0xE0; 
      else if (inString(request, "9bit")==1)   temp_res = 0xE2;
      else if (inString(request, "10bit")==1)  temp_res = 0xE4;
      else if (inString(request, "11bit")==1)  temp_res = 0xE6;
      else if (inString(request, "12bit")==1)  temp_res = 0xE8;
      //tempRes(temp_res);
      return temp_res;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  // GPIO pin for LED output
  pinMode(PA6, GPIO_OUTPUT);
  
  enableAPB2_TIM15();
  initTIM(TIM15);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // TODO: Add SPI initialization code
  initSPI(4, 0, 1);
  configureTemp();
  
  
  /*
  while(1) {
    spiWrite(0x80, 0xE0);
    spiRead(0x00);
  }
  */

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */
    
    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
    
    
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");
    

    // SPI code for reading temperature
    char temp_res = updateTempRes(request);
    tempRes(temp_res);
    delay_millis(TIM15, 350);
    double temp = getTemp(temp_res);
    delay_millis(TIM15, 10);
    
    // Update string with current LED state
    
    char tempStatusStr[40];
    if (temp_res == 0xE0)
      sprintf(tempStatusStr, "Temperature: %.0f degrees C", temp);
    else if (temp_res == 0xE2)
      sprintf(tempStatusStr, "Temperature: %.1f degrees C", temp);
    else if (temp_res == 0xE4) 
      sprintf(tempStatusStr, "Temperature: %.2f degrees C", temp);
    else if (temp_res == 0xE6) 
      sprintf(tempStatusStr, "Temperature: %.3f degrees C", temp);
    else if (temp_res == 0xE8) 
      sprintf(tempStatusStr, "Temperature: %.4f degrees C", temp);

    
    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, tempStr); // button for controlling temp resolution
    
    // show LED Status
    sendString(USART, "<h2>LED Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");
    
    
    // show Temp Status
    sendString(USART, "<h2>Temperature Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, tempStatusStr);
    sendString(USART, "</p>");
    
    
  }
}
/*************************** End of file ****************************/
