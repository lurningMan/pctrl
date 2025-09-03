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
    void (*action)(void);

    // Optional partial entry overrides for orthogonal regions
    State **parallel_targets;  // One per region (indexed by region index)
    int num_parallel_targets;  // Must match number of regions in target->submachine[]
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
    int num_submachines;
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