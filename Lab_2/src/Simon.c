#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>  // Para rand() y srand()
#include <time.h>

#define WAITING_START 1
#define START_GAME 2
#define SHOW_SEQUENCE 4
#define WAIT_color 8
#define CHECK_DIGIT 16
#define GAME_WON 32
#define GAME_LOST 64

#define MAX_SECUENCIA 20  // Longitud máxima de la secuencia

int current_state;

int color; 
int user_index = 0;       // Index to track user's progress in the sequence
int habilitado;
int contador_unidades;
int semilla = 0xBAD;

uint8_t secuencia[MAX_SECUENCIA];   // Arreglo para almacenar la secuencia de LEDs
uint8_t nivel;                  // Nivel inicial del juego

volatile uint16_t tiempo_penalizado = 6;  // 200ms
volatile uint16_t tiempo_inicial = 61;  // 2s


void pines();
void interrupciones();
void parpadear_dos_veces();
void parpadear_tres_veces();
void maquina();
void apagar_leds();
void retardo(int desborde);

void generate_sequence(uint8_t longitud);
void show_sequence(uint8_t longitud, uint16_t tiempo_encendido);

int main(void){
  pines();
  interrupciones();
  current_state = WAITING_START;
  color = 0;
  sei(); // Habilita interrupcion global
  
  while (1) {
    maquina();
  }

}


void maquina() {
  switch (current_state) {

    case WAITING_START:
      semilla += 0xCAFE;
      if (color != 0) { //color == 1 || color == 2 || color == 4
        current_state = START_GAME;
        color         = 0;
        nivel         = 1;
        parpadear_dos_veces();
      }
      else {
        current_state = WAITING_START;
        semilla       += 0x13;
      }
      break;

    case START_GAME : 
      generate_sequence(3 + nivel);  // Genera una secuencia aleatoria con longitud basada en el nivel
      user_index = 0;        // Reset user input index
      color      = 0;
      retardo(30);          // Delay entre que se parpadean los leds 2 veces y se muestra la secuencia
      current_state = SHOW_SEQUENCE; // SHOW_SEQUENCE
      break;

    case SHOW_SEQUENCE:
      show_sequence(3 + nivel, tiempo_inicial);  // Muestra la secuencia al jugador
      user_index    = 0;    // Reset index for user input
      color         = 0;
      current_state = WAIT_color;  // Cambia al estado de espera de la entrada del usuario
      break;

    case WAIT_color:
      if (color != 0)
        current_state = CHECK_DIGIT;  // Se presiono algun boton
      else 
        current_state = WAIT_color;  // No se ha presionado ningun boton
      break;

    case CHECK_DIGIT: 
      if (color == secuencia[user_index]) {
        // Correct digit, move to the next one
        user_index++;
        if (user_index == 3 + nivel) {
          // User completed the sequence, game won
          current_state = GAME_WON;
        } else {
          // Continue waiting for next digit
          current_state = WAIT_color;
        }
      } else {
        current_state = GAME_LOST;
      }
      color = 0;
      break;

    case GAME_WON:
      semilla        += 0xFACE;
      tiempo_inicial -= tiempo_penalizado;
      nivel++;
      current_state  = START_GAME;
      break;


    case GAME_LOST:
      // Incorrect digit, user loses
      parpadear_tres_veces();
      current_state = WAITING_START;
      break;

    default:
      break;
  }
}


void generate_sequence(uint8_t longitud) {
    semilla += 0x77;
    srand(semilla);  // Seed the random number generator
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

void show_sequence(uint8_t longitud, uint16_t tiempo) {
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
        retardo(tiempo);
        apagar_leds();  // Apaga los LEDs después de que el temporizador lo controle
        retardo(30);  // Pequeña pausa entre las luces
    }
}

void retardo(int desborde){
  contador_unidades = 0;
  habilitado        = 1;
  TIMSK             = 0x02;
  while(contador_unidades < desborde){
    PORTB &= 0b11111111;
  }
  TIMSK = 0x00;
  habilitado = 0;
}

void pines(){
  DDRB = 0x0F; //Puertos PB0, PB1, PB2 y PB3 como salida
}

void interrupciones(){

  GIMSK = 0xD8; // Habilita interrupciones INT0, INT1, PCIE1 y PCIE2.

  PCMSK1 = 0b00000001; // Habilita PCINT8
  PCMSK2 = 0b00000010; // Habilita PCINT12
  
  MCUCR = 0b00001010; // Configura INTx para que se active por flanco negativo

  // Configura el Timer/Counter0 
  TCCR0A = 0b00000000;   
  TCCR0B = 0b00000101;   // prescale de 1024
  TCNT0 = 0x00;

  habilitado = 0;
  contador_unidades = 0;
}

void parpadear_dos_veces(){
  for (int i = 0; i < 2; i++){
    PORTB = 0x0F;  // Enciende los leds
    retardo(30);
    PORTB = 0x00;  // Apaga los Leds
    retardo(40);
  }
}

void parpadear_tres_veces(){
  for (int i = 0; i < 3; i++){
    PORTB = 0x0F;  // Enciende los leds
    retardo(30);
    PORTB = 0x00;  // Apaga los Leds
    retardo(40);
  }
}


ISR(INT0_vect){
  color = 1; //verde D2
}

ISR(INT1_vect){
  color = 2; //amarillo D3
}

ISR (PCINT2_vect){
  if (!(PIND & (1 << PD1))) {
    color = 3;  // rojo B7 pcint11
  }
}

ISR (PCINT1_vect) {
  if (!(PINA & (1 << PA0))) {
    color = 4;  //azul A0 pcint8
  }
}

ISR(TIMER0_OVF_vect) {
  if (habilitado)
    contador_unidades++;
}
