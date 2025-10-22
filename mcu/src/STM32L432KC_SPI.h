// STM32L432KC_SPI.h
// Erin Wang
// erinwang@g.hmc.edu
// 10/19/2025
// Header to define SPI addresses and function prototypes

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>

#define SPI_CE PB0
#define SPI_SCLK PB3
#define SPI_MISO PB4
#define SPI_MOSI PB5

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
void initSPI(int br, int cpol, int cpha);

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
char spiSendReceive(char send);

/* Reads a character (1 byte) over SPI and returns the received character.
 *    -- address:  the slave address to send to over SPI
 *    -- sendData:  null data not used since the address is being read from not written to
 *    -- return:    the character received over SPI */
char spiRead(char address);

/* Writes a character (1 byte) over SPI to a specified address of the slave
 *    -- address:  the slave address to send to over SPI
 *    -- sendData: the character to send over SPI */
void spiWrite(char address, char sendData);

#endif
