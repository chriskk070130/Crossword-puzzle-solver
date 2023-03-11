#include <stdlib.h>
#include "solve_functions.h"
#include "checks.h"
#include "solve.h"

int solve(char** table, int n, char*** my_dict, int* lengths, int**** bdict, Slot* myslots, int n_myslots){ 
    Listptr mystack=NULL, temp_st;  //mystack στοίβα αποθήκευσης κάθε slot που συμπληρώνεται στο grid
	fc_Listptr temp_fc;
    int next_slot_ind, domain_word, j, last_slot_ind, first_slot_ind, backtracking, count=0; 
	while(check_if_completed(myslots, n_myslots) != 1){
		backtracking = 0;
		next_slot_ind = find_next_slot(myslots , n_myslots);
        if(count == 0){
            first_slot_ind = next_slot_ind;
        }
        count++;
		for(domain_word = myslots[next_slot_ind].index; domain_word < lengths[myslots[next_slot_ind].length]; domain_word++){
			if(myslots[next_slot_ind].current_domain[domain_word] == 1){         //Ελεγχος κάθε συμβατής λέξης του domain του slot
				if(slot_is_okey(myslots[next_slot_ind] , domain_word , myslots , n_myslots , lengths, bdict , my_dict)){
					find_conflicts(next_slot_ind, myslots, n_myslots);             //Αν η λέξη με index domain_word είναι έγκυρη τότε βρίσκουμε τα conflicts του slot 
					myslots[next_slot_ind].filled = 1;
					myslots[next_slot_ind].index = domain_word;
					temp_st = mystack;       //Eισαγωγή νέου κόμβου στην αρχή της λίστας (στην στοίβα)
					mystack = malloc(sizeof(struct listnode));
					mystack->slot_ind = next_slot_ind;
					mystack->next = temp_st;
					break; 
				}
			}
		}
		if(domain_word == lengths[myslots[next_slot_ind].length]){  //Αν φτάσουμε στο τέλος του domain και δεν βρούμε καμία έγκυρη λέξη
			backtracking = 1;                                       //αρχίζει η διαδικασία της οπισθοδρόμισης
            if(count == 1){ 
                return 0;  //Δεν έχουμε λύση
            }
		}
		while(backtracking == 1){ 
            backtracking = 0;
			last_slot_ind = mystack->slot_ind;
			temp_st = mystack;
			mystack = mystack->next;
			free(temp_st); 
			for(int sl_ind = 0; sl_ind < n_myslots; sl_ind++){ //Παίρνω το index του τελευταίου slot που τοποθετήθηκε στο grid και επαναφέρω τα domains των slot που άλλαξα
				if(myslots[last_slot_ind].neighbors[sl_ind] == 1 && myslots[sl_ind].filled == 0){  //για αυτό
					for(j = 0; j < lengths[myslots[sl_ind].length]; j++){
						myslots[sl_ind].current_domain[j] = myslots[sl_ind].prev_domains->domain[j];
					}
					myslots[sl_ind].remaining_words = myslots[sl_ind].prev_domains->remaining_values;
					temp_fc = myslots[sl_ind].prev_domains;
					myslots[sl_ind].prev_domains = myslots[sl_ind].prev_domains->next;
					free(temp_fc->domain);
					free(temp_fc);
				}
			}
			myslots[last_slot_ind].filled = 0;
			for(domain_word = myslots[last_slot_ind].index + 1; domain_word < lengths[myslots[last_slot_ind].length]; domain_word++){ //Συνεχίζω την αναζήτηση μίας έγκυρης λέξης
				if(myslots[last_slot_ind].current_domain[domain_word] == 1){                                                          //για το slot αυτό
					if(slot_is_okey(myslots[last_slot_ind], domain_word, myslots, n_myslots , lengths , bdict , my_dict)){ 
						myslots[last_slot_ind].filled = 1;
						myslots[last_slot_ind].index = domain_word;
						temp_st = mystack;
						mystack = malloc(sizeof(struct listnode));
						mystack->slot_ind = last_slot_ind;
						mystack->next = temp_st;
						break; 
					}
				}
			}	
			if(domain_word == lengths[myslots[last_slot_ind].length]){ //Αν δεν βρέθηκε έγκυρη λέξη η διαδικασία της οπισθδρόμισης επαναλαμβάνεται 
				myslots[last_slot_ind].index = 0;                     
				backtracking = 1;
                if(last_slot_ind == first_slot_ind){
                    return 0; //Εχουμε κάνει τόσες οπισθοδρομίσεις και τελικά φτάνουμε στο πρώτο slot, εφόσον δεν έχουμε βρει 
                }             //άλλη συμβατή τιμή για αυτό, τότε το σταυρόλεξο μας δεν έχει λύση για το δοσμένο λεξικό
			}
		}
	}
    while(mystack != NULL){ //Αποδέσμευσης της μνήμης που έχει δεσμευτεί απο την στοίβα ενεργειών
        temp_st = mystack;
        mystack = mystack->next;
        free(temp_st);
    }
    return 1;
}
