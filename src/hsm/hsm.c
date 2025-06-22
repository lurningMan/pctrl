#include "hsm/hsm.h"
#include <stddef.h>

/**
 * @brief Computes the depth of a state in the state hierarchy.
 *
 * This function returns the number of levels from the given state
 * up to the root of the state hierarchy. A leaf state has a depth of 0,
 * and each parent level adds one to the depth.
 *
 * @param s Pointer to the state whose depth is to be calculated.
 *              Must not be NULL.
 *
 * @return The number of parent states above the given state.
 * 
 * \startuml
 * start
 * :Set depth = 0;

 * while (s != NULL?) 
   -yes-> s = s->parent;
           :depth++;
 * endwhile
 * :Return depth;
 * stop
 * \enduml
 */

static int get_depth(State *s)
{
    /* Count the levels between a given state and the top.  The minimum 
    output is 1. */
    int depth = 0;
    while (s) {
        s = s->parent;
        depth++;
    }
    return depth;
}

/**
 * @brief Finds the lowest common ancestor (LCA) between two states in the hierarchy.
 *
 * This function computes the depth of both states and then climbs up the hierarchy
 * to equalize their levels. Once they are on the same level, it continues walking
 * up both states’ parent chains in parallel until a common ancestor is found.
 *
 * This is useful when performing transitions between states that may not share
 * a direct parent, and a minimal exit/entry path needs to be determined.
 *
 * @param a Pointer to the first state.
 * @param b Pointer to the second state.
 * @return Pointer to the lowest common ancestor of both states.
 *
 * \startuml
 * start
 * :Compute depth_a = get_depth(a);
 * :Compute depth_b = get_depth(b);
 * 
 * start
 * 
 * if (depth_a > depth_b?) then (yes)
 *   :Move 'a' up until depths are equal;
 * elseif (depth_b > depth_a?) then (yes)
 *   :Move 'b' up until depths are equal;
 * endif
 * 
 * while (a and b are valid and a and b are not the same state)
 *   :a = a->parent;
 *   :b = b->parent;
 * endwhile
 * 
 * :Return a (common ancestor);
 * 
 * stop
 * \enduml
 */

static State* find_common_ancestor(State *a, State *b)
{
    /* Get the nesting depth of both states compared to root level */
    int da = get_depth(a);
    int db = get_depth(b);
    
    /* If a is deeper than b, count up from a until you get to b's level */
    while (da > db) 
    {
        a = a->parent; 
        --da;
    }

    /* If b is deeper than a, count up from b until you get to a's level */
    while (db > da)
    {
        b = b->parent;
        --db;
    }

    /* Now that you are at the lowest even point in each lineage, ascend
    the tree until a and b are the same state */
    while (a && b && a != b) 
    {
        a = a->parent;
        b = b->parent;
    }
    return a;
}

/**
 * @brief Recursively exits states up to the common ancestor.
 *
 * This function handles the first half of a hierarchical state transition by
 * exiting all states from the current state up to, but not including, the common ancestor.
 * It checks if each state has a valid `on_exit` function before calling it.
 *
 * @param from Pointer to the current state (starting point of the transition).
 * @param ancestor Pointer to the common ancestor state. Exiting stops when this state is reached.
 *
 * @note The transition should later be completed by calling enter_from_common_ancestor().
 *
 * 
 * \startuml
 * start
 * while (from != ancestor?) 
   -yes-> if (current->on_exit != NULL?)
           -yes-> :Call current->on_exit();
           -no->  :Skip on_exit;
         :Set from = from->parent;
 * endwhile
 * stop
 * \enduml
 */

static void exit_to_common_ancestor(State *from, State *ancestor)
{
    /* Iterate through the intervening states between from and ancestor. */
    while (from && from != ancestor)
    {
        /* Check if the on_exit function exists before executing. */
        if (from->on_exit)
        {
            from->on_exit(from);
        }
        /* Go one level higher for the next iteration. */
        from = from->parent;
    }
}

/**
 * @brief Recursively enters states from the common ancestor to the target state.
 *
 * This function handles the second half of a hierarchical state transition by entering
 * all intermediate states from the common ancestor to the target state, in proper order.
 * It ensures that composite state entry functions are called before their child states'
 * entry functions.
 *
 * @param to Pointer to the target state to enter.
 * @param ancestor Pointer to the common ancestor state between source and target.
 *
 * @note This function is typically called after exiting up to the common ancestor during
 *       a state transition in a hierarchical state machine using exit_to_common_ancestor.
 *
 * \startuml
 * participant "LeafState (Target)" as tgt
 * participant "CompositeState (Intermediate)" as int
 * participant "CompositeState (Ancestor)" as anc

 * tgt -> int: enter_from_common_ancestor(Intermediate, Ancestor)
 * int -> anc: enter_from_common_ancestor(Ancestor, Ancestor)
 * anc -> anc: on_entry(Ancestor)
 * anc -> anc: state_machine_init(Ancestor)
 * anc --> int: enter_from_common_ancestor(Ancestor, Ancestor)
 * int -> int: on_entry(Intermediate)
 * int -> int: state_machine_init(Intermediate)
 * int --> tgt: enter_from_common_ancestor(Intermediate, Ancestor)
 * tgt -> tgt: on_entry(Target)
 * tgt -> tgt: state_machine_init(Target)

 * \enduml
 */

static void enter_from_common_ancestor(State *to, State *ancestor)
{
    /* Recursive ascent from the target state to the ancestor. */
    if (to->parent != ancestor)
    {
        enter_from_common_ancestor(to->parent, ancestor);
    }
    /* Once we get past the above check, we can start executing the 
    on_entry functions from the top down. */

    /* Check if the on_entry function exists before executing. */
    if (to->on_entry) 
    {
        to->on_entry(to);
    }

    /* If the destination state happens to be a state machine in its own
    right (compound state), then initialize it. */
    if (to->submachine) 
    {
        state_machine_init(to->submachine);
    }
}

void state_machine_init(StateMachine *sm)
{
    sm->previous_state = NULL;
    sm->current_state = sm->initial_state;

    /* Check if the default state exists, if so execute the on_entry function. */
    if (sm->current_state)
    {
        enter_from_common_ancestor(sm->current_state, NULL);
    }
}

/**
 * @brief Executes one tick of the hierarchical state machine.
 *
 * This function checks all transitions from the current state (including from ancestor states,
 * depending on implementation), evaluates their conditions, and performs a transition
 * if any condition is met. It then calls the `on_run` handler of the active state.
 *
 * This tick-based execution model enables the state machine to advance deterministically
 * in response to external stimuli or internal logic without requiring asynchronous events.
 *
 * @param sm Pointer to the state machine to update.
 *
 * \startuml
 * start
 *
 * if (current state is null?) then (yes)
 *   stop
 * endif
 * 
 * if (current state is a state machine?) then (yes)
 *   :recursive call;
 * endif
 * 
 * :run this state's on_run function;
 * 
 * repeat
 *   if (this transition begins in the current state and has a valid condition function and the condition is active?) then (yes)
 *     :execute find_common_ancestor(current state, target state);
 *     :execute exit_to_common_ancestor(current state, common ancestor);
 *     :execute enter_from_common_ancestor(target state, common ancestor);
 *     :update state machine current state/previous state;
 *   endif
 * repeat while (valid transition not executed or transitions remaining to be evaluated) is (yes) not (no)
 * 
 * stop
 * \enduml
 */

void state_machine_tick(StateMachine *sm)
{
    if (!sm || !sm->current_state)
    {
        return;
    }

    State *current = sm->current_state;

    // Evaluate transitions from current state
    for (int i = 0; i < current->num_transitions; ++i) 
    {
        Transition *t = &current->transitions[i];
        if (t->condition && t->condition())
        {
            // Exit current state
            if (current->on_exit)
            {
                current->on_exit(current);
            }

            sm->previous_state = current;
            sm->current_state = t->target;

            // Enter new state
            if (sm->current_state->on_entry)
            {
                sm->current_state->on_entry(sm->current_state);
            }

            // Optional: initialize submachine
            if (sm->current_state->submachine)
            {
                state_machine_init(sm->current_state->submachine);
            }
            return;
        }
    }

    // If no transition taken, run the state
    if (current->on_run)
    {
        current->on_run(current);
    }
    // If this state has a submachine, tick it
    if (current->submachine)
    {
        state_machine_tick(current->submachine);
    }
}

void state_machine_send_event(StateMachine *sm, int event) {
    if (!sm->current_state) return;
    // Try submachine first
    if (sm->current_state->submachine) {
        state_machine_send_event(sm->current_state->submachine, event);
    }
    // Then local handler
    if (sm->current_state->on_event) {
        sm->current_state->on_event(sm->current_state, event);
    }
}

/*
#include <stdio.h>

void print_active_path(StateMachine *sm) {
    if (!sm || !sm->current_state) {
        printf("[!] No active state.\n");
        return;
    }

    // Temporary array to hold the hierarchy path
    const int MAX_DEPTH = 32;
    State *path[MAX_DEPTH];
    int depth = 0;

    // Walk up from current leaf to root, saving the path
    State *current = sm->current_state;
    while (current && depth < MAX_DEPTH) {
        path[depth++] = current;
        if (current->parent)
            current = current->parent->current_state;  // move up to parent state's current
        else
            break;
    }

    // Print path from root down to leaf
    printf("Active State Path: ");
    for (int i = depth - 1; i >= 0; --i) {
        printf("%s", path[i]->name); // assuming you add a `const char *name` field to State
        if (i > 0)
            printf(" → ");
    }
    printf("\n");
}
*/