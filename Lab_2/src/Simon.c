#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>


void pines();
void interrupciones();

int main(void){

  pines();
  interrupciones();
  sei(); // Habilita interrupcion global

  while (1) {

  }

}

void pines(){
  DDRB = 0x0F; //Puertos PB0, PB1, PB2 y PB3 como salida

  // Puertos PD0, PD1, PD2 y PD3 como entrada para los botones
  DDRD &= ~(1 << PD0); 
  DDRD &= ~(1 << PD1); 
  DDRD &= ~(1 << PD2);
  DDRD &= ~(1 << PD3); 

}

void interrupciones(){

  GIMSK = 0xD0;

  // Configuración de interrupciones externas en INT0, INT1 y PCINT para PD3
  MCUCR |= (1 << ISC01) | (1 << ISC11); // ISC01 = 1, ISC00 = 0 -> Flanco descendente de INT0 genera interrupción
  MCUCR &= ~((1 << ISC00) | (1 << ISC10)); // ISC11 = 1, ISC10 = 0 -> Flanco descendente de INT1 genera interrupción

  
  PCMSK2 |= (1 << PCINT8) | (1 << PCINT9); // Habilita PCINT8 y PCINT9

}


ISR(INT0_vect){
  PORTB = 0x04;  // Enciende el LED verde
  _delay_ms(1000);
  PORTB = 0x00;  // Apaga el LED
    
}

ISR(INT1_vect){
  PORTB = 0x01;  // Enciende el LED amarillo
  _delay_ms(1000);
  PORTB = 0x00;  // Apaga el LED
    
}

ISR (PCINT2_vect){
  PORTB = 0x08;  // Enciende el LED rojo
  _delay_ms(1000);
  PORTB = 0x00;  // Apaga el LED
}


// ISR (PCINT2_vect) {
//   PORTB = 0x02;  // Enciende el LED azul
//   _delay_ms(1000);
//   PORTB = 0x00;  // Apaga el LED
// }

