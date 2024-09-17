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
    WAIT_USER_INPUT
} fsm_state_t;

fsm_state_t current_state = WAITING_START;

int verde = 0, amarillo = 0, rojo = 0, azul = 0;


uint8_t secuencia[MAX_SECUENCIA];  // Arreglo para almacenar la secuencia de LEDs
uint8_t nivel = 1;  // Nivel inicial del juego
uint16_t tiempo_encendido = 10000;  // Tiempo inicial de encendido de los LEDs

void pines();
void interrupciones();
void parpadear_dos_veces();
void parpadear_tres_veces();
void reiniciar_colores();
void maquina();
void apagar_leds();
void delay_variable_ms(uint16_t ms);

void inicializar_aleatorio();
void generate_sequence(uint8_t longitud);
void show_sequence(uint8_t longitud, uint16_t tiempo_encendido);
uint8_t get_user_input(uint8_t longitud);  // Función ficticiA


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
      generate_sequence(4 + nivel - 1);  // Genera una secuencia aleatoria con longitud basada en el nivel
      show_sequence(4 + nivel - 1, tiempo_encendido);  // Muestra la secuencia al jugador
      current_state = WAIT_USER_INPUT;  // Cambia al estado de espera de la entrada del usuario
      break;

    case WAIT_USER_INPUT:
      
      // nivel++;  // Incrementar el nivel
      // tiempo_encendido -= 8000;
      // _delay_ms(100000);
      // current_state = SHOW_SEQUENCE;

      if (get_user_input(4 + nivel - 1)) {  // Verificar si el jugador reproduce correctamente (función ficticia)
        nivel++;  // Incrementar el nivel
        tiempo_encendido -= 8000;
        current_state = SHOW_SEQUENCE;  // Volver a mostrar la secuencia
      } else {
        parpadear_tres_veces();  // Mostrar indicación de error si el jugador falla
        current_state = WAITING_START;  // Volver al estado inicial de espera
      }


      break;

    default:
      break;
  }
}


void generate_sequence(uint8_t longitud) {
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


void show_sequence(uint8_t longitud, uint16_t tiempo_encendido) {
    for (uint8_t i = 0; i < longitud; i++) {
        switch (secuencia[i]) {
            case 0:
                led_verde();
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
       _delay_ms(5000);  // Pequeña pausa entre las luces
    }
}

uint8_t get_user_input(uint8_t longitud) {
    // Verificar cada paso de la secuencia
    for (uint8_t i = 0; i < longitud; i++) {
        // Reiniciar las variables de entrada del jugador antes de verificar el próximo LED
        reiniciar_colores();

        // Esperar a que el jugador presione algún botón
        while (1) {
            // Verificar si la entrada es incorrecta
            if ((secuencia[i] == 0 && (amarillo || rojo || azul)) || 
                (secuencia[i] == 1 && (verde || rojo || azul)) || 
                (secuencia[i] == 2 && (verde || amarillo || azul)) || 
                (secuencia[i] == 3 && (verde || amarillo || rojo))) {
                return 0;  // El jugador se equivocó en la secuencia
            }
            
            // Verificar si la entrada es correcta
            if ((secuencia[i] == 0 && verde) || 
                (secuencia[i] == 1 && amarillo) || 
                (secuencia[i] == 2 && rojo) || 
                (secuencia[i] == 3 && azul)) {
                break;  // Salir del bucle y continuar con la siguiente entrada
            }

            _delay_ms(50);  // Pequeño retardo para evitar rebote de botones y estabilizar la entrada
        }

        _delay_ms(200);  // Pequeño retardo para evitar que el rebote de botón afecte la próxima lectura
    }

    return 1;  // El jugador reprodujo correctamente toda la secuencia
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

