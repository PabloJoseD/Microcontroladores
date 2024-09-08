#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

int main(void)
{
  DDRB = 0x08; //Configuracion del puerto PB3 como salida, el resto como entrada

  // Configurar PD2 (INT0) como entrada para el botón
  DDRD &= ~(1 << PD2); // Limpiar PD2 para configurarlo como entrada

  // Configurar INT0 para que se active en el flanco descendente (pulsación del botón)
  MCUCR |= (1 << ISC01);  // ISC01 = 1, ISC00 = 0 -> Flanco descendente de INT0 genera interrupción
  MCUCR &= ~(1 << ISC00);

  // Habilitar la interrupción externa INT0
  GIMSK |= (1 << INT0);
  
  sei(); // Habilita interrupcion global
  
  while (1) {

  }
}


ISR(INT0_vect){
  PORTB = 0x08;  // Enciende el LED
  _delay_ms(1000);
  PORTB = 0x00;  // Apaga el LED
    
}