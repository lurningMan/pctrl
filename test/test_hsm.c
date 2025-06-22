/*
 * nested_state_machine_test.c
 *
 * This file contains an interactive CLI test application that demonstrates
 * a hierarchical state machine handling a simulated device menu system.
 *
 * 
 * RootStateMachine
|
+-- MainMenu (Leaf)
|   
+-- AboutMenu (CompositeState)
|   |
|   +-- HomeView (Leaf)
|   +-- InfoView (Leaf)
|
+-- SettingsMenu (CompositeState)
|   |
|   +-- BrightnessAdjust (Leaf)
|   +-- VolumeAdjust (Leaf)
|
+-- Diagnostics (CompositeState)
|   |
|   +-- SelfTest (Leaf)
|   +-- Logview (Leaf)

 * 
 * 
 *   Default state is the Main Menu.  From here, press 
 *   a - Go to About
 *   s - Go to Settings
 *   d - Go to Diagnostics
 *   1 - Select first option in the current menu
 *   2 - Select second option in the current menu
 *   b - Go back to parent state
 *   q - Quit to main menu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hsm/hsm.h"  // Assumes your hierarchical state machine types and functions are defined here

// Forward declarations for states
extern State main_menu, settings_menu, diagnostics_menu;
extern State home_screen, info_screen;
extern State brightness_screen, volume_screen;
extern State selftest_screen, logview_screen;

// Function prototypes
void build_state_hierarchy();
void handle_input(char input);

// Entry/Exit/Run handlers for various states
#define DEFINE_STATE_FUNCS(name) \
    void name##_on_entry(State* self) { printf("Entered %s\n", #name); } \
    void name##_on_exit(State* self)  { printf("Exited %s\n", #name); } \
    void name##_on_run(State* self)   { printf("Running %s. Enter command: ", #name); }

// Leaf state handlers
DEFINE_STATE_FUNCS(home_screen);
DEFINE_STATE_FUNCS(info_screen);
DEFINE_STATE_FUNCS(brightness_screen);
DEFINE_STATE_FUNCS(volume_screen);
DEFINE_STATE_FUNCS(selftest_screen);
DEFINE_STATE_FUNCS(logview_screen);

// Composite state handlers (typically only implement entry/exit)
DEFINE_STATE_FUNCS(main_menu);
DEFINE_STATE_FUNCS(about_menu);
DEFINE_STATE_FUNCS(settings_menu);
DEFINE_STATE_FUNCS(diagnostics_menu);

// Define states
State home_screen;  
State info_screen;
State brightness_screen;
State volume_screen;
State selftest_screen;
State logview_screen;

State main_menu;
State about_menu;
State settings_menu;
State diagnostics_menu;

/*
{home_screen_on_entry, home_screen_on_run, home_screen_on_exit, NULL};
{info_screen_on_entry, info_screen_on_run, info_screen_on_exit, NULL};
{brightness_screen_on_entry, brightness_screen_on_run, brightness_screen_on_exit, NULL};
{volume_screen_on_entry, volume_screen_on_run, volume_screen_on_exit, NULL};
{selftest_screen_on_entry, selftest_screen_on_run, selftest_screen_on_exit, NULL};
{logview_screen_on_entry, logview_screen_on_run, logview_screen_on_exit, NULL};

{main_menu_on_entry, NULL, main_menu_on_exit, NULL};
{about_menu_on_entry, NULL, about_menu_on_exit, NULL};
{settings_menu_on_entry, NULL, settings_menu_on_exit, NULL};
{diagnostics_menu_on_entry, NULL, diagnostics_menu_on_exit, NULL};
*/



// Hold user input
char current_input = '\0';

// Transition functions
int detect_a(void) { return current_input == 'a'; }
int detect_s(void) { return current_input == 's'; }
int detect_d(void) { return current_input == 'd'; }
int detect_1(void) { return current_input == '1'; }
int detect_2(void) { return current_input == '2'; }
int detect_b(void) { return current_input == 'b'; }
int detect_q(void) { return current_input == 'q'; }

// Define transitions
Transition main_menu_transitions[] = {
    {&about_menu, detect_a},
    {&settings_menu, detect_s},
    {&diagnostics_menu, detect_d}
};

Transition about_menu_transitions[] = {
    {&home_screen, detect_1},
    {&info_screen, detect_2},
    {&main_menu, detect_q},
    {&main_menu, detect_b},
};

Transition settings_menu_transitions[] = {
    {&brightness_screen, detect_1},
    {&volume_screen, detect_2},
    {&main_menu, detect_b},
    {&main_menu, detect_q },
};

Transition diagnostics_menu_transitions[] = {
    {&selftest_screen, detect_1},
    {&logview_screen, detect_2},
    {&main_menu, detect_b},
    {&main_menu, detect_q},
};

Transition home_screen_transitions[] = {
    {&about_menu, detect_b},
    {&main_menu, detect_q}
};

Transition info_screen_transitions[] = {
    {&about_menu, detect_b},
    {&main_menu, detect_q}
};

Transition brightness_screen_transitions[] = {
    {&settings_menu, detect_b},
    {&main_menu, detect_q}
};

Transition volume_screen_transitions[] = {
    {&settings_menu, detect_b},
    {&main_menu, detect_q},
};

Transition logview_screen_transitions[] = {
    {&diagnostics_menu, detect_b},
    {&main_menu, detect_q}
};
    
Transition selftest_screen_transitions[] = {
    {&diagnostics_menu, detect_b},
    {&main_menu, detect_q},
};


// Assign fields
State main_menu = {NULL, &main_menu_on_entry, &home_screen_on_run, home_screen_on_exit, NULL, main_menu_transitions, 3};

State about_menu = {NULL, &about_menu_on_entry, &about_menu_on_run, about_menu_on_exit, NULL, about_menu_transitions, 3};
State settings_menu = {NULL, &settings_menu_on_entry, &settings_menu_on_run, settings_menu_on_exit, NULL, settings_menu_transitions, 3};
State diagnostics_menu = {NULL, &diagnostics_menu_on_entry, &diagnostics_menu_on_run, diagnostics_menu_on_exit, NULL, diagnostics_menu_transitions, 3};

State home_screen = {&about_menu, &home_screen_on_entry, &home_screen_on_run, &home_screen_on_exit, NULL, home_screen_transitions, 2};
State info_screen = {&about_menu, &info_screen_on_entry, &info_screen_on_run, &info_screen_on_exit, NULL, info_screen_transitions, 2};
State brightness_screen = {&settings_menu, &brightness_screen_on_entry, &brightness_screen_on_run, &brightness_screen_on_exit, NULL, brightness_screen_transitions, 2};
State volume_screen = {&settings_menu, &volume_screen_on_entry, &volume_screen_on_run, &volume_screen_on_exit, NULL, volume_screen_transitions, 2};
State logview_screen = {&diagnostics_menu, &logview_screen_on_entry, &logview_screen_on_run, &logview_screen_on_exit, NULL, logview_screen_transitions, 2};
State selftest_screen = {&diagnostics_menu, &selftest_screen_on_entry, &selftest_screen_on_run, &selftest_screen_on_exit, NULL, selftest_screen_transitions, 2};

State *States[] = {&main_menu,
                   &about_menu, 
                   &settings_menu, 
                   &diagnostics_menu, 
                   &home_screen, 
                   &info_screen, 
                   &brightness_screen, 
                   &volume_screen, 
                   &logview_screen, 
                   &selftest_screen};

StateMachine sm = {States,
                   10, 
                   &main_menu};
int main(void)
{
    state_machine_init(&sm);

    while (current_input != 'x') {
        current_input = (char)getchar();
        if (current_input == EOF) break;
        if (current_input == '\n') continue;
        state_machine_tick(&sm);
    }

    return 0;
}