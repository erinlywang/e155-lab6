// DS1722.h
// Erin Wang
// erinwang@g.hmc.edu
// 10/19/2025
// Header to define DS1722 addresses and function prototypes

// Define addresses
#define TEMPCONFIG_READ   0x00
#define TEMPCONFIG_WRITE  0x80
#define TEMP_LSB    0x01
#define TEMP_MSB    0x02

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* configures the D1722 temperature sensor to 0b11100000 */
void configureTemp(void);

/* sets the resolution of the D1722 temperature sensor
 *    -- resolution: a given char to set the resolution of the temp sensor
 *    -- return: none */
void tempRes(char resolution);

/* calculates the temperature from the D1722 temperature sensor
 *    -- return: a double storing the calculated temperature (with signs)*/
double getTemp();