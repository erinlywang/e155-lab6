// DS1722.c
// Erin Wang
// erinwang@g.hmc.edu
// 10/22/2025
// Source code for temperature sensor (DS1722) functions

#include "STM32L432KC.h"

/* configures the D1722 temperature sensor to 0b11100000 */
void configureTemp() {
    // Write to configure address to set to continuous mode and 8-bit res
    spiWrite(TEMPCONFIG_WRITE, 0);
}


/* sets the resolution of the D1722 temperature sensor
 *    -- resolution: a given char to set the resolution of the temp sensor
 *    -- return: none */
void tempRes(char resolution) {

    // Write the given resolution to the configure address
    spiWrite(TEMPCONFIG_WRITE, resolution);

}

/* calculates the temperature from the D1722 temperature sensor
 *    -- return: a double storing the calculated temperature (with signs)*/
double getTemp() {

    // Grab the MSB and the LSB
    volatile char msb = spiRead(TEMP_MSB);
    volatile char lsb = spiRead(TEMP_LSB);
    
    // convert the MSB using two's complement
    int negative = (msb & (1 << 7)) != 0;
    //printf("%i", negative);
    double decimalMSB;

    if (negative==1)
      decimalMSB = -(double)(~msb + 1);
    else
      decimalMSB = (double)msb;
    //printf("%f\n", decimalMSB);

    // convert the LSB with a fraction decimal
    double decimalLSB;
    int i;
    for (i = 0; i < 8; i++) {
        if (lsb & (1 << (7-i))) {
            decimalLSB += 1.0/(1 << (i+1)); 
        }
    } 

    // return the final sum
    return decimalMSB + decimalLSB;

}