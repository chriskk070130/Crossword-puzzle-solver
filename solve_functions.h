#include "structs.h"

//Ελέγχει εάν μία λέξη είναι έγκυρη για το slot ή όχι
int slot_is_okey(Slot myslot, int word, Slot* slots_table, int n_myslots, int* lengths, int**** bdict, char*** my_dict);

//Βρίσκει το επόμενο slot για συμπλήρωση με την μέθοδο MRV-heuristics
int find_next_slot(Slot* slots_table, int n);

//Βρίσκει τα slot που επηρρεάζει κάθε slot του πίνακα <slots_table>
//και αρχικοποιεί τους πίνακες <neighbors> και <neighbors_information> του
void find_neighbors(char** table, Slot* slots_table, int n_myslots);

//Βρίσκει τις θέσεις που είναι conflicts του slot με index <slot_ind> 
//στον πίνακα <slots_table> και αρχικοποιεί τον πίνακα <conflicts> του
void find_conflicts(int slot_ind, Slot* slots_table, int n_myslots);
