#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hsm/hsm.h"  // Assumes your hierarchical state machine types and functions are defined here

void cpu_idle_on_entry(void) {printf("Entered CPU idle.\n");}
void cpu_busy_on_entry(void) {printf("Entered CPU busy.\n");}
void cpu_over_on_entry(void) {printf("Entered CPU overload.\n");}

State cpu_idle, cpu_busy, cpu_over;

int main(void)
{




}