/*
 Programa prueba AN0 = Nivel de Ruido y UART.
 */


#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#pragma intrinsic(_delay)

#define _XTAL_FREQ 20000000 // necessary for __delay_us

#include <xc.h>
#include <stdio.h>
 
char conta;
int c;

void init_adc0(void)
{
	ADCON1bits.ADFM = 1; //justificar resultado tras las conversion de 10 bits a la derecha-
	ADCON0bits.CHS = 0b0000; // canal analogico que vamos a usar. nos piden N0 por eso es el 0000-
	ADCON0bits.ADON = 1; // inicializo el adc-
	ADCON0bits.ADCS = 0b10; // es el que respeta el minimo de tiempo de 1,6 us-
	
    //interrupciones del adc
    PIE1bits.ADIE = 1; //Habilitar interrupciones del adc
	PIR1bits.ADIF=0;

    ADCON1bits.VCFG1 = 0; //vss 0v-
    ADCON1bits.VCFG0 = 0; //VDD 5V-
}
 
void init_uart(void)
{  
  TXSTAbits.BRGH = 0; //baja velocidad para mayor muestreo
  BAUDCTLbits.BRG16 =0 ;  // 1=  16 bit baudrate;   0= 8 bit baudrade

  // SPBRGH:SPBRG = ES un mismo registro que se descompone en estas 2 partes de 8 bits cada uno. 32 entra 
  //sin problema en SPBRG en binario por tanto no necesitamos el registro SPBRGH
  SPBRGH = 0;
  SPBRG = 32;  // 9600 baud rate with 20MHz Clock
  
  TXSTAbits.SYNC = 0; /* Asynchronous */
  TXSTAbits.TX9 = 0; /* TX 8 data bit */
  RCSTAbits.RX9 = 0; /* RX 8 data bit */

  PIE1bits.TXIE = 0; 
  PIE1bits.RCIE = 0;

  RCSTAbits.SPEN = 1; /* Serial port enable */
  //registro TXEN lo reseteamos y lo ponemos habilitado para interrupciones
  TXSTAbits.TXEN = 0; /* Reset transmitter */
  TXSTAbits.TXEN = 1; /* Enable transmitter */
 }
 
//Configurado para que cada 5ms haya interrupcion. preescalado =256
void init_tmr0(void)
{
    OPTION_REGbits.T0CS=0; //reloj interni
    OPTION_REGbits.PSA=0; //hay preescalador
    OPTION_REGbits.PS=0b111; //preescalado 256
    TMR0=158;  // ((5.10^6)/256)x 0,005=97,6 cojo 98.     256-98= 158
}
 
void __interrupt() int_routine(void)
{
    TMR0=158;
  
    if(conta==100) //interrupcion cada 0,5seg
    {
		ADCON0bits.GO=1; // se inicializa conversion;
		while(ADCON0bits.GO); // mientras sea 1 es que no ha terminado conversión.

		c= ADRESH<<8 | ADRESL; // contiene los 10 bits de la salida del  ADC tras la conversión
		printf("%d\r\n",c);
		PORTB = ADRESL; // puertob saca los 8 bits del registro ADRESL
		conta=0; //vuelve a empezar contador a 0 para que se verifique despues con el if
    }
	INTCONbits.T0IF = 0; // unable interrupcion del TMR0
	conta++; 
}
 
/* It is needed for printf */
void putch(char data) //necesario para el printf, sobrescrito el putch de libreria
{
    while ( !TXIF);
    TXREG =data;
}

void main(void)
{
	OSCCON = 0b00001000; //este es el registro de control del oscilador, cada bit es un flag
	OSCCONbits.OSTS = 1; // External cristal
	TRISB = 0; // configure PORTB as output 

	init_uart();
	init_adc4();
	init_tmr0();

	INTCONbits.GIE=1; // 1er nivel habilitado para interrupciones
    INTCONbits.T0IE=1; // 2do nivel habilidad para interrupciones
    
    while(1);
}
