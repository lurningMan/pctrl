#include "fsm/fsm.h"
#include "stdio.h"

void state_machine_init(st_StateMachine *state_machine)
{
    // Check if current state is uninitialized
    if (!state_machine->current_state)
    {
        state_machine->current_state = state_machine->default_state;
    }

    // Check for null state/state entry function
    if (state_machine->current_state && state_machine->current_state->on_entry)
    {
        state_machine->current_state->on_entry();
    }
}

void state_machine_run(st_StateMachine *state_machine) 
{
    // Check for transitions
    for (int i = 0; i < state_machine->num_transitions; ++i) {
        if ((state_machine->transitions[i].source_state == state_machine->current_state) && 
            state_machine->transitions[i].condition()) {
            // Execute exit function of current state
            if ( state_machine->current_state->on_exit)
            {
                state_machine->current_state->on_exit();
            }
            // Transition to new state
            state_machine->prev_state = state_machine->current_state;
            state_machine->current_state = state_machine->transitions[i].target_state;

            // Execute entry function of new state
            if ( state_machine->current_state->on_entry)
            {
                state_machine->current_state->on_entry();
            }

            break; // Only one transition per cycle
        }
    }
    // Execute run function of current state
    if (state_machine->current_state->on_run)
    {
        state_machine->current_state->on_run();
    }
}
