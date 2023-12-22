/*
 Prueba PWM del SMA.
 */

#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#pragma intrinsic(_delay)

#define _XTAL_FREQ 20000000 // necessary for __delay_us

#include <xc.h>
#include <stdio.h>

 
int incrementar=1;
int contador =0;

void tmr2_init(void)
{
	TMR2=0;
	PR2=41;
	//No necesitamos postscale	
	T2CONbits.T2CKPS=0b01;  // 1:4 prescale
	T2CONbits.TMR2ON=1; //TMR2 ON
	
	PIE1bits.TMR2IE=1; // habilitar interrupcion de timer2
	PIR1bits.TMR2IF= 0; 
}
void init_PWM(void)
{
    CCP1CONbits.P1M = 0b00; //Single output, P1A modulated
    CCP1CONbits.DC1B = 0; //PWM mode
    CCP1CONbits.CCP1M = 0b1100; //PWM mode, all active-high
    CCPR1L = 166; //empieza en el 100%
    
}

void __interrupt() PWM_pasive(void)
{
    if(PIR1bits.TMR2IF)
    {
        if(cont == 1500) //50ms => 50ms/(1/30KHz) = 1500
        {
            contador=0;
            if(incrementar && CCPR1L == 166)
            {
                incrementar = 0;
            }
            else if(!incrementar && CCPR1L == 166)
            {
                incrementar = 1;
            }        
            if(incrementar) 
                CCPR1L++;
            else
                CCPR1L--;
            cont = 0;
        }
        
        cont ++;
    }
    
    PIR1bits.TMR2IF = 0;
}
 
void main(void)
{ 
  OSCCON = 0b00001000; // External cristal
  
  init_timer2();
  init_PWM();

  TRISC = 0; // Port c output
  ANSEL = 0; //Por tanto es I/O es digital
  ANSELH = 0; 
	

  INTCONbits.GIE=1; // 1er nivel habilitado para interrupciones
  INTCONbits.PEIE=1; // 2do nivel habilidad para interrupciones

  while(1);
 }
