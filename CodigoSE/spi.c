#include <xc.h>
#include <stdint.h>
#include "spi-master-v1.h"

#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#pragma intrinsic(_delay)

#define _XTAL_FREQ 20000000 // necessary for __delay_us

// Definiciones de pines para SPI
//#define SS   RC2
#define SCK  RC0
#define SDI  RC4
#define SDO  RC5

void SPI_Init_Master();
void SK9822_SendColor(uint8_t brigthness, uint8_t red, uint8_t green, uint8_t blue, int n);

void main() {
    TRISC2 = 0; // Configurar el pin del esclavo como salida

    SPI_Init_Master(); // Inicializar el modo maestro SPI

    while (1) {
        // Encender el LED SK9822 en blanco usando la funci?n spi_write_read
        SK9822_SendColor(31,255, 255, 255, 10); // se encienden 10 leds
        __delay_ms(1000);
    }
}

void SPI_Init_Master() {
    TRISC0 = 0; // SCK como salida (reloj)
    TRISC5 = 0; // SDO como salida (Maestro manda esclavo)


}
/*
    La funcion spi_write_read envia cada ciclo de reloj 8 bits (1 byte).
    El inicio de la trama son 32 bits (4 bytes) a 0 .
    Cada LED Frame tiene 3 bits a 1 (obligatorio), luego 5 bits a elegir (Global)
    y luego 24 bits para el color.
    El fin de la trama son 32 bits(4 bytes) a 1.
*/
void SK9822_SendColor(uint8_t brigthness, uint8_t red, uint8_t green, uint8_t blue, int n) {

    uint8_t init = 0b11100000;
    uint8_t global = init | brigthness;
    // inicio de trama
    for(int i = 0; i < 4; i++)
    {
        spi_write_read((char)0x00);
    }

    // inicio de LED Frame

    for (int i = 0; i < n; i++)
    {
        spi_write_read((char)global);
        spi_write_read((char)blue);// Blue
        spi_write_read((char)green);// Green
        spi_write_read((char)red);// Red
    }

    // fin de trama
    for(int i = 0; i < 4; i++)
    {
        spi_write_read((char)0xFF);
    }

}