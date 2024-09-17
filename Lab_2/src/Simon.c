#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>  // Para rand() y srand()
#include <time.h>


#define MAX_SECUENCIA 20  // Longitud máxima de la secuencia
#define TIEMPO_INICIAL 10000  // Tiempo inicial de encendido de LEDs en milisegundos
#define REDUCCION_TIEMPO 200 // Reducción de tiempo de encendido por nivel

// FSM States
typedef enum {
    WAITING_START,
    START_GAME,
    SHOW_SEQUENCE,
    WAIT_USER_INPUT
} fsm_state_t;

fsm_state_t current_state = WAITING_START;

int verde = 0, amarillo = 0, rojo = 0, azul = 0;


uint8_t secuencia[MAX_SECUENCIA];  // Arreglo para almacenar la secuencia de LEDs
uint8_t nivel = 1;  // Nivel inicial del juego
uint16_t tiempo_encendido = TIEMPO_INICIAL;  // Tiempo inicial de encendido de los LEDs

void pines();
void interrupciones();
void parpadear_dos_veces();
void reiniciar_colores();
void maquina();
void apagar_leds();

void inicializar_aleatorio();
void generar_secuencia_aleatoria(uint8_t longitud);
void mostrar_secuencia(uint8_t longitud, uint16_t tiempo_encendido);
uint8_t jugador_reproduce_correctamente(uint8_t longitud);  // Función ficticiA


int main(void){
  pines();
  interrupciones();
  sei(); // Habilita interrupcion global

  while (1) {
    maquina();

  }

}


void maquina() {
  switch (current_state) {
    case WAITING_START:
      reiniciar_colores();
      if (verde != 0 || amarillo != 0 || rojo != 0 || azul != 0)
        current_state = START_GAME;
      else 
        current_state = WAITING_START;
      break;

    case START_GAME : 
      parpadear_dos_veces();
      reiniciar_colores();
      _delay_ms(5000);
      current_state = SHOW_SEQUENCE;
      break;

    case SHOW_SEQUENCE:
      generar_secuencia_aleatoria(4 + nivel - 1);  // Genera una secuencia aleatoria con longitud basada en el nivel
      mostrar_secuencia(4 + nivel - 1, tiempo_encendido);  // Muestra la secuencia al jugador
      current_state = WAIT_USER_INPUT;  // Cambia al estado de espera de la entrada del usuario
      break;

    // case WAIT_USER_INPUT:
    //   _delay_ms(10000);
    //   nivel++;  // Incrementar el nivel
    //   tiempo_encendido = (tiempo_encendido > REDUCCION_TIEMPO) ? tiempo_encendido - REDUCCION_TIEMPO : REDUCCION_TIEMPO;  // Reducir el tiempo de encendido
    //   current_state = SHOW_SEQUENCE;
    //   break;

    default:
      break;
  }
}

void inicializar_aleatorio() {
    srand(time(NULL));  // Inicializa el generador de números aleatorios con una semilla basada en el tiempo
}

void generar_secuencia_aleatoria(uint8_t longitud) {
    if (longitud > MAX_SECUENCIA) {
        longitud = MAX_SECUENCIA;
    }
    for (uint8_t i = 0; i < longitud; i++) {
        secuencia[i] = rand() % 4;  // Genera un número aleatorio entre 0 y 3 para determinar qué LED se enciende
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

void reiniciar_colores(){
  verde    = 0;
  amarillo = 0;
  rojo     = 0;
  azul     = 0;
}

void delay_variable_ms(uint16_t ms) {
    while (ms > 0) {
        _delay_ms(1);  // Retardo constante de 1 ms
        ms--;          // Decrementa el tiempo restante
    }
}


void mostrar_secuencia(uint8_t longitud, uint16_t tiempo_encendido) {
    for (uint8_t i = 0; i < longitud; i++) {
        switch (secuencia[i]) {
            case 0:
                led_verde();
                _delay_ms(1000);
                break;
            case 1:
                led_amarillo();
                break;
            case 2:
                led_rojo();
                break;
            case 3:
                led_azul();
                break;
        }
       delay_variable_ms(tiempo_encendido);  // Mantén el LED encendido por el tiempo actual
        apagar_leds();    // Apaga todos los LEDs
        _delay_ms(10000);  // Pequeña pausa entre las luces
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

  GIMSK = 0xD8; // Habilita interrupciones INT0, INT1, PCIE2 y PCIE1.

  // Configuración de interrupciones externas en INT0, INT1 y PCINT para PD3
  MCUCR |= (1 << ISC01) | (1 << ISC11); // ISC01 = 1, ISC00 = 0 -> Flanco descendente de INT0 genera interrupción
  MCUCR &= ~((1 << ISC00) | (1 << ISC10)); // ISC11 = 1, ISC10 = 0 -> Flanco descendente de INT1 genera interrupción
  
  PCMSK2 |= (1 << PCINT11); // Habilita PCINT11
  PCMSK1 |= (1 << PCINT8); // Habilita PCINT8

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
  verde = 1;
}

ISR(INT1_vect){
  amarillo = 1;
}

ISR (PCINT2_vect){
  rojo = 1;
}

ISR (PCINT1_vect) {
  azul = 1;
}

