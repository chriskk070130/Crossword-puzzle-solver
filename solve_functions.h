#include "structs.h"
int slot_is_okey(Slot myslot, int word, Slot* slots_table, int n_myslots, int* lengths, int**** bdict, char*** my_dict);
int find_next_slot(Slot* slots_table, int n);
void find_neighbors(char** table, Slot* slots_table, int n_myslots);
void find_conflicts(int slot_ind, Slot* slots_table, int n_myslots);
