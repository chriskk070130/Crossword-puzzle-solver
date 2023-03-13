#include <stdio.h>
#include "structs.h"

//Ελέγχει εάν η λύση που δίνει ο χρήστης απο την πρότυπη είσοδο(stdin) είναι σωστή 
//για το δοσμένο σταυρόλεξο και λεξικό
int check_solution(FILE* dictionary, char** crossw, int dimensions, Slot* slots_table, int n_myslots);

//Ελέγχει εαν όλα τα τετραγωνίδια του grid είναι συμπληρωμένα
int check_if_completed(Slot* slots_table, int n_myslots);
