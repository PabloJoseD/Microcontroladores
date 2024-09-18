#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>  // Para rand() y srand()
#include <time.h>


#define MAX_SECUENCIA 20  // Longitud máxima de la secuencia
#define TIEMPO_INICIAL 10000  // Tiempo inicial de encendido de LEDs en milisegundos

// FSM States
typedef enum {
    WAITING_START,
    START_GAME,
    SHOW_SEQUENCE,
    WAIT_USER_INPUT,
    CHECK_DIGIT,
    GAME_LOST
} fsm_state_t;

fsm_state_t current_state = WAITING_START;

int user_input; 
int user_index = 0;       // Index to track user's progress in the sequence
int sequence_length = 4;

uint8_t secuencia[MAX_SECUENCIA];   // Arreglo para almacenar la secuencia de LEDs
uint8_t nivel = 1;                  // Nivel inicial del juego
uint16_t tiempo_encendido = 10000;  // Tiempo inicial de encendido de los LEDs

void pines();
void interrupciones();
void parpadear_dos_veces();
void parpadear_tres_veces();
void reiniciar_colores();
void maquina();
void apagar_leds();
void delay_variable_ms(uint16_t ms);

void generate_sequence(uint8_t longitud);
void show_sequence(uint8_t longitud, uint16_t tiempo_encendido);

int main(void){
  pines();
  interrupciones();
  sei(); // Habilita interrupcion global
  user_input = 6;
  
  while (1) {
    maquina();
  }

}


void maquina() {
  switch (current_state) {
    case WAITING_START:
      if (user_input == 1 || user_input == 2 || user_input == 4 || user_input == 0) { //user_input == 1 || user_input == 2 || user_input == 4
        current_state = START_GAME;
      }
      else {
        current_state = WAITING_START;
      }
      break;

    case START_GAME : 
      generate_sequence(3 + nivel);  // Genera una secuencia aleatoria con longitud basada en el nivel
      user_index = 0;        // Reset user input index
      parpadear_dos_veces();
      user_input = 0;
      _delay_ms(5000);
      current_state = WAITING_START; // SHOW_SEQUENCE
      break;

    // case SHOW_SEQUENCE:
    //   show_sequence(3 + nivel, tiempo_encendido);  // Muestra la secuencia al jugador
    //   user_index = 0;    // Reset index for user input
    //   user_input = 0;
    //   current_state = WAIT_USER_INPUT;  // Cambia al estado de espera de la entrada del usuario
    //   break;

    // case WAIT_USER_INPUT:
    //   if (user_input < 4)
    //     current_state = CHECK_DIGIT;  // Se presiono algun boton
    //   else 
    //     current_state = WAIT_USER_INPUT;  // No se ha presionado ningun boton


    case CHECK_DIGIT: 
      if (user_input == secuencia[user_index]) {
        // Correct digit, move to the next one
        user_index++;
        if (user_index == sequence_length) {
            // User completed the sequence, game won
            nivel++;
            tiempo_encendido -= 8000;
            current_state = SHOW_SEQUENCE;
        } else {
            // Continue waiting for next digit
            current_state = WAIT_USER_INPUT;
        }
      } else {
        // Incorrect digit, user loses
        parpadear_tres_veces();
        current_state = WAITING_START;
      }
      break;
    default:
      break;
  }
}


void generate_sequence(uint8_t longitud) {
    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < longitud; i++) {
        secuencia[i] = (rand() % 4 + 1);  // Random number between 0 and 3
    }
}

void led_verde(){
  PORTB = 0x04;  // Enciende el LED verde
}

void led_amarillo(){
  PORTB = 0x01;  // Enciende el LED amarillo
}

void led_rojo(){
  PORTB = 0x08;  // Enciende el LED rojo
}

void led_azul(){
  PORTB = 0x02;  // Enciende el LED azul
}

void apagar_leds(){
  PORTB = 0x00;  
}

void delay_variable_ms(uint16_t ms) {
    while (ms > 0) {
        _delay_ms(1);  // Retardo constante de 1 ms
        ms--;          // Decrementa el tiempo restante
    }
}


void show_sequence(uint8_t longitud, uint16_t tiempo_encendido) {
    for (uint8_t i = 0; i < longitud; i++) {
        switch (secuencia[i]) {
            case 1:
                led_verde();
                break;
            case 2:
                led_amarillo();
                break;
            case 3:
                led_rojo();
                break;
            case 4:
                led_azul();
                break;
        }
       delay_variable_ms(tiempo_encendido);  // Mantén el LED encendido por el tiempo actual
       apagar_leds();    // Apaga todos los LEDs
       _delay_ms(5000);  // Pequeña pausa entre las luces
    }
}


void pines(){
  DDRB = 0x0F; //Puertos PB0, PB1, PB2 y PB3 como salida
}

void interrupciones(){

  GIMSK = 0xE8; // Habilita interrupciones INT0, INT1, PCIE0 y PCIE1.

  // Configuración de interrupciones externas en INT0, INT1 y PCINT para PD3
  MCUCR |= (1 << ISC01) | (1 << ISC11); // ISC01 = 1, ISC00 = 0 -> Flanco descendente de INT0 genera interrupción
  MCUCR &= ~((1 << ISC00) | (1 << ISC10)); // ISC11 = 1, ISC10 = 0 -> Flanco descendente de INT1 genera interrupción
  
  // MCUCR = 0b00001010;

  //PCMSK2 |= (1 << PCINT12); // Habilita PCINT11
  //PCMSK1 |= (1 << PCINT8); // Habilita PCINT8

  // PCMSK2 = 0b00000001;
  PCMSK1 = 0b00000001;
  PCMSK = 0b10000000;
}

void parpadear_dos_veces(){
  for (int i = 0; i < 2; i++){
    PORTB = 0x0F;  // Enciende los leds
    _delay_ms(3000);
    PORTB = 0x00;  // Apaga los Leds
    _delay_ms(4000);
  }
}

void parpadear_tres_veces(){
  for (int i = 0; i < 4; i++){
    PORTB = 0x0F;  // Enciende los leds
    _delay_ms(3000);
    PORTB = 0x00;  // Apaga los Leds
    _delay_ms(4000);
  }
}


ISR(INT0_vect){
  user_input = 1; //verde D2
}

ISR(INT1_vect){
  user_input = 2; //amarillo D3
}

ISR (PCINT2_vect){
  user_input = 3;  // rojo B7 pcint11
}

ISR (PCINT1_vect) {
  user_input = 4;  //azul A0 pcint8
}

