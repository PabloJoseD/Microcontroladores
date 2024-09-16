#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_SEQUENCE_LENGTH 100


// FSM States
typedef enum {
    WAITING_START,
    START_GAME,
    SHOW_SEQUENCE,
    WAIT_USER_INPUT,
    CHECK_DIGIT,
    GAME_WON,
    GAME_LOST
} fsm_state_t;

fsm_state_t current_state = WAITING_START;


// Game variables
int sequence[MAX_SEQUENCE_LENGTH];
int sequence_length = 4;  // Starting length of the sequence
int user_index = 0;       // Index to track user's progress in the sequence
int delay_time = 2000;    // Initial delay time (2000ms)
int user_input; 


// Function to generate a random sequence
void generate_sequence() {
    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < sequence_length; i++) {
        sequence[i] = (rand() % 4) + 1;  // Random number between 1 and 4
    }
}


// Function the user must repeat
void show_sequence() {
    printf("Memorize the sequence:\n");
    for (int i = 0; i < sequence_length; i++) {
        printf("%d\n", sequence[i]);
        usleep(delay_time*1000);  // Wait for delayTime (in milliseconds)
        //system("clear");
    }
}


// Function to get user input digit-by-digit
int get_user_input() {
    int input;
    printf("Enter the next number (1-4): ");
    scanf("%d", &input);
    return input;
}


// Function to check the current digit against the sequence
void check_digit(int input) {
    if (input == sequence[user_index]) {
        // Correct digit, move to the next one
        user_index++;
        if (user_index == sequence_length) {
            // User completed the sequence, game won
            current_state = GAME_WON;
        } else {
            // Continue waiting for next digit
            current_state = WAIT_USER_INPUT;
        }
    } else {
        // Incorrect digit, user loses
        current_state = GAME_LOST;
    }
}


int main() {
    while (1) {
        switch (current_state) {
        case WAITING_START:
            printf("Enter a number from 1 to 4 to start\n");
            int startInput;
            scanf("%d", &startInput);
            if (startInput >= 1 && startInput <= 4) {
                current_state = START_GAME;
                printf("Hello! Let's start the game.\n");
            } else {
                current_state = WAITING_START;  // Ignore invalid input
            }
            break;

        case START_GAME:       
            generate_sequence();  // Generate the first sequence
            user_index = 0;        // Reset user input index
            current_state = SHOW_SEQUENCE;
            break;
        
        case SHOW_SEQUENCE:
            show_sequence();  // Display the sequence
            user_index = 0;    // Reset index for user input
            current_state = WAIT_USER_INPUT;
            break;

        case WAIT_USER_INPUT:
            // Wait for user to input the next digit
            user_input = get_user_input();  // Get user input
            current_state = CHECK_DIGIT;  // Move to check input   
            printf("It's your turn\n");
            break;
        
        case CHECK_DIGIT:
            if (user_input == sequence[user_index]) {
                // Correct digit, move to the next one
                user_index++;
                if (user_index == sequence_length) {
                    // User completed the sequence, game won
                    current_state = GAME_WON;
                } else {
                    // Continue waiting for next digit
                    current_state = WAIT_USER_INPUT;
                }
            } else {
                // Incorrect digit, user loses
                current_state = GAME_LOST;
            }
            break;

        case GAME_WON:
            printf("You've won\n");
            if (delay_time == 200) {
                printf("End of the game\n");
            } else {
                delay_time -= 200;
            }
            sequence_length += 1;
            current_state = START_GAME;
            break;
        
        case GAME_LOST:
            printf("You've lost, start again\n");
            current_state = WAITING_START;
            break;
        
        default:
            break;
        }
    }
    
}