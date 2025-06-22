#ifndef HSM_H
#define HSM_H

typedef struct State State;
typedef struct Transition Transition;
typedef struct StateMachine StateMachine;


typedef void (*StateFunc)(State *self);
typedef void (*EventFunc)(State *self, int event);

typedef struct Transition {
    State *target;
    int (*condition)(void);
} Transition;

typedef struct State {
    //const char *name;
    State *parent;  // NULL if top-level
    StateFunc on_entry;
    StateFunc on_run;
    StateFunc on_exit;
    EventFunc on_event;

    Transition *transitions;
    int num_transitions;

    // If this state is a composite, these fields are used
    StateMachine *submachine;
} State;

typedef struct StateMachine {
    State **states;
    int num_states;
    State *initial_state;
    State *current_state;
    State *previous_state;
} StateMachine;

void state_machine_init(StateMachine *sm);
void state_machine_tick(StateMachine *sm);
void state_machine_send_event(StateMachine *sm, int event);

#endif // HSM_H