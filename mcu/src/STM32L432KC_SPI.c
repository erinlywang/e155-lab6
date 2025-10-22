// STM32L432KC_SPI.c
// Erin Wang
// erinwang@g.hmc.edu
// 10/22/2025
// Source Code for SPI functions

#include "STM32L432KC_SPI.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include <stdio.h>


/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */


void initSPI(int br, int cpol, int cpha) {
      RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
      
      // SCLK GPIO Pin
      pinMode(SPI_SCLK, GPIO_ALT);
      
      // MISO GPIO Pin
      pinMode(SPI_MISO, GPIO_ALT);

      // MOSI GPIO Pin
      pinMode(SPI_MOSI, GPIO_ALT);
      
      //Chip Encoder GPIO pin
      pinMode(SPI_CE, GPIO_OUTPUT);

      // Set output speed type to high for SCLK
      GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);


      // Set alternate functions
      GPIOB -> AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
      GPIOB -> AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
      GPIOB -> AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

      
      SPI1-> CR1 |= _VAL2FLD(SPI_CR1_BR, br);         // set Baud Rate
      
      SPI1 -> CR1 |= (SPI_CR1_MSTR);                 // set to master mode MSTR

      SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_SSM);
      
      SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);     // set clock polarity
      SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);     // set clock phase
      SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);     // configure data size to be a char (1 byte or 8 bits) 
      SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);
      SPI1 -> CR1 |= (SPI_CR1_SPE);
      
        
}

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */

 
char spiSendReceive(char send) {
      // wait for queue to be empty (TXE to be 1)
      while (!(SPI1 -> SR & SPI_SR_TXE));
    
      // treat DR register as 8-bit register rather than 16-bit
      // then add the 8-bit char to the queue
      *(volatile char *) (&SPI1->DR) = send; 
      
      // wait for there to be data/RXNE to be not empty (RXNE to be 1)
      while (!(SPI1 -> SR & SPI_SR_RXNE));

      char rec = (volatile char) SPI1->DR;

      return rec; // Return received character
}


char spiRead(char address){
    // make the GPIO pin high
    digitalWrite(SPI_CE, 1);

    // pass in the address to read from
    spiSendReceive(address);

    // pass in the data to be written
    char dataRead = spiSendReceive(0);

    // make the GPIO low
    digitalWrite(SPI_CE, 0);

    // return the data I read
    return dataRead;
}

void spiWrite(char address, char sendData){
    // make the GPIO pin high
    digitalWrite(SPI_CE, 1);
    
    // pass in the address to write to
    spiSendReceive(address);

    // pass in the data to be written
    spiSendReceive(sendData);

    // make the GPIO low
    digitalWrite(SPI_CE, 0);
}


