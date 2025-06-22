#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "fsm/fsm.h"

//#define TEXT "HELLO"
//#define TEXT_LEN (sizeof(TEXT) - 1)

char current_input = '\0';

// Condition functions
char is_H_pressed(void) { return current_input == 'H'; }
char is_E_pressed(void) { return current_input == 'E'; }
char is_L1_pressed(void) { return current_input == 'L'; }
char is_L2_pressed(void) { return current_input == 'L'; }
char is_O_pressed(void) { return current_input == 'O'; }

// Entry and exit functions
void entry_H(void) { printf("Enter 'H': "); }
void exit_H(void) { printf("Received 'H'\n"); }

void entry_E(void) { printf("Enter 'E': "); }
void exit_E(void) { printf("Received 'E'\n"); }

void entry_L1(void) { printf("Enter first 'L': "); }
void exit_L1(void) { printf("Received first 'L'\n"); }

void entry_L2(void) { printf("Enter second 'L': "); }
void exit_L2(void) { printf("Received second 'L'\n"); }

void entry_O(void) { printf("Enter 'O': "); }
void exit_O(void) { printf("Received 'O'\n"); }

void entry_DONE(void) { printf("Success! You spelled 'HELLO'.\n"); }
void exit_DONE(void) {}

// Define states
st_State state_H = { entry_H, NULL, exit_H };
st_State state_E = { entry_E, NULL, exit_E };
st_State state_L1 = { entry_L1, NULL, exit_L1 };
st_State state_L2 = { entry_L2, NULL, exit_L2 };
st_State state_O = { entry_O, NULL, exit_O };
st_State state_DONE = { entry_DONE, NULL, exit_DONE };

// Define transitions
st_Transition transitions[] = {
    { &state_H, &state_E, is_H_pressed },
    { &state_E, &state_L1, is_E_pressed },
    { &state_L1, &state_L2, is_L1_pressed },
    { &state_L2, &state_O, is_L2_pressed },
    { &state_O, &state_DONE, is_O_pressed }
};



int main(void) {

    // Define state machine
    st_State states[] = { state_H, state_E, state_L1, state_L2, state_O, state_DONE };

    st_StateMachine sm = {
    .states = states,
    .transitions = transitions,
    .num_states = sizeof(states) / sizeof(states[0]),
    .num_transitions = sizeof(transitions) / sizeof(transitions[0]),
    .default_state = &state_H,
    .prev_state = NULL,
    .current_state = NULL
};

    state_machine_init(&sm);

    while (sm.current_state != &state_DONE) {
        int ch = getchar();
        if (ch == EOF) break;
        if (ch == '\n') continue;
        current_input = (char)ch;
        state_machine_run(&sm);
    }

    return 0;
}
