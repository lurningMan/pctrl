typedef struct {
    void (*on_entry)(void);
    void (*on_run)(void);
    void (*on_exit)(void);
} st_State;

typedef struct {
    st_State *source_state;
    st_State *target_state;
    char (*condition)(void);
} st_Transition;

typedef struct {
    st_State *states;
    st_Transition *transitions;
    char num_states;
    char num_transitions;
    st_State *default_state;
    st_State *prev_state;
    st_State *current_state;
} st_StateMachine;

void state_machine_init(st_StateMachine *state_machine);

void state_machine_run(st_StateMachine *state_machine);