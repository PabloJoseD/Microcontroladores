#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
  DDRB = 0x08; //Configuracion del puerto

  //Parpadear
  while (1) {
    PORTB &= ~(1 << PB3); // 
    _delay_ms(2000); 
    PORTB |=  (1 << PB3); // 
    _delay_ms(2000); 
  }
}
