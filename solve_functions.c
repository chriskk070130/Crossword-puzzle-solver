#include <stdio.h>
#include <stdlib.h>
#include "solve_functions.h"

int find_next_slot(Slot* slots_table, int n){ 
	int min_ind=-1, min_words, count=0; 	  
	for(int i = 0; i < n; i++){
		if(slots_table[i].filled == 0){
			if(count == 0){
				min_ind = i;
				min_words = slots_table[i].remaining_words;
				count++;
			}
			else if(slots_table[i].remaining_words < min_words){
				min_ind = i;
				min_words = slots_table[i].remaining_words;
			}
		}
	}
	return min_ind;
}

void find_conflicts(int slot_ind, Slot* slots_table, int n_myslots){
    for(int sl_ind = 0; sl_ind < n_myslots; sl_ind++){
        if(slots_table[slot_ind].neighbors[sl_ind] == 1 && slots_table[sl_ind].filled == 1){
            slots_table[slot_ind].conflicts[slots_table[slot_ind].neighbors_information[sl_ind][0]] = 1;
        }
    }
}

void find_neighbors(char** table, Slot* slots_table, int n_myslots){ 
	int row, col, is_horizontal, length, search_row, search_col, start, letter; 
	for(int i = 0; i < n_myslots; i++){												
		row = slots_table[i].i;													
		col = slots_table[i].j;
		is_horizontal = slots_table[i].row_col;
		length = slots_table[i].length;
		slots_table[i].neighbors = calloc(n_myslots , sizeof(int));                //Πίνακας μεγέθους όσο το πλήθος των slot που για κάθε slot έχει στην θέση i το 1
		slots_table[i].neighbors_information = malloc(n_myslots * sizeof(int *));  //αν επηρρεάζει το slot στην θέση i του πίνακα myslots και το 0 αν όχι
		for(int j = 0; j < n_myslots; j++){
			slots_table[i].neighbors_information[j] = malloc(2 * sizeof(int));  //Για κάθε slot που επηρρεάζει το συγκεκριμένο slot έχω τις εξής πληροφορίες
			slots_table[i].neighbors_information[j][0] = -1;                    //Η θέση του τρέχων slot που γίνεται το conflict 
			slots_table[i].neighbors_information[j][1] = -1;                    //H Θέση του άλλου slot που γίνεται το conflict
		}														  
		letter = 0;
		if(is_horizontal){
            for(int pos = col; pos <= col+length-1; pos++){ 
				search_row = row;
        	    while(search_row >= 0 && table[search_row][pos] != '#'){
                    search_row--;
                }
                start = search_row + 1; 
                for(int k = 0; k < n_myslots; k++){
                    if(slots_table[k].row_col == 0 && slots_table[k].i == start && slots_table[k].j == pos){
                        slots_table[i].neighbors[k] = 1;					
						slots_table[i].neighbors_information[k][0] = letter; 
						slots_table[i].neighbors_information[k][1] = row - start; 
						break;
                    }
                }
				letter++;
			}
		}
		else{
            for(int pos = row; pos <= row+length-1; pos++){
				search_col = col;
                while(search_col >= 0 && table[pos][search_col] != '#'){
                    search_col--;
                }
                start = search_col + 1; 
                for(int k = 0; k < n_myslots; k++){
                    if(slots_table[k].row_col == 1 && slots_table[k].i == pos && slots_table[k].j == start){
                        slots_table[i].neighbors[k] = 1;
						slots_table[i].neighbors_information[k][0] = letter; 
						slots_table[i].neighbors_information[k][1] = col - start; 
						break;
                    }
                }
				letter++;
			}
		}
	}
}

int slot_is_okey(Slot myslot, int word, Slot* slots_table, int n_myslots, int* lengths, int**** bdict, char*** my_dict){
	int count, pos_myslot, pos_neighbor, ch;
	int length = myslot.length;         //Για κάθε συμβατή λέξη του domain του slot ελέγχω αν τα domains των slot που επηρρεάζει είναι κενά	
	fc_Listptr temp;                    //Εάν είναι κενά τότε ελέγχω μία άλλη συμβατή λέξη για το slot. Αν όχι, η λέξη είναι έγκυρη.
	for(int sl_ind = 0; sl_ind < n_myslots; sl_ind++){
		if(myslot.neighbors[sl_ind] == 1 && slots_table[sl_ind].filled == 0){       //Το slot με index: sl_ind στον πίνακα myslots επηρρεάζεται απο το τρέχων slot
			temp = slots_table[sl_ind].prev_domains;                                //και δεν έχει τοποθετηθεί ακόμα λέξη σε αυτό, άρα πρέπει να αλλαχθεί το domain του
			slots_table[sl_ind].prev_domains = malloc(sizeof(struct fc_listnode));  //και το current_domain να μπει στην στοίβα prev_domains έτσι ώστε να το επαναφέρουμε 
			slots_table[sl_ind].prev_domains->domain = malloc(lengths[slots_table[sl_ind].length] * sizeof(int));  //σε περίπτωση οπισθοδρόμισης
			slots_table[sl_ind].prev_domains->remaining_values = slots_table[sl_ind].remaining_words;  //Το μέλος remaining_words είναι το πλήθος των συμβατών λέξεων
			slots_table[sl_ind].prev_domains->next = temp;                                             //του slot αυτή την στιγμή
			pos_myslot = myslot.neighbors_information[sl_ind][0]; 
			pos_neighbor = myslot.neighbors_information[sl_ind][1]; 
			count = 0;
			ch = my_dict[length][word][pos_myslot] - 'a';             //Ο χαρακτήρας του slot που βρίσκεται πάνω στην διασταύρωση με το slot που επηρρεάζει
			for(int j = 0; j < lengths[slots_table[sl_ind].length]; j++){ 
				slots_table[sl_ind].prev_domains->domain[j] = slots_table[sl_ind].current_domain[j];  //Βάζω το current_domain στην στοίβα prev_domains
				slots_table[sl_ind].current_domain[j] = slots_table[sl_ind].current_domain[j] & bdict[slots_table[sl_ind].length][pos_neighbor][ch][j];  
				if(slots_table[sl_ind].current_domain[j] == 1){       //Στον πίνακα bdict πάμε στις λέξεις που αντιστοιχούν στο μήκος
					count++;                                          //slots_table[sl_ind].length και στην θέση pos_neighbor έχουν το γράμμα ch
				}												     
			}
			slots_table[sl_ind].remaining_words = count;
			if(count == 0){                                                    //Αν το domain του slot δεν έχει καμία συμβατή λέξη 
				for(int slot_index = 0; slot_index <= sl_ind; slot_index++){   //τότε επαναφέρω όλα τα domains των slots που έχω αλάγξει μέχρι τώρα
					if(myslot.neighbors[slot_index] == 1 && slots_table[slot_index].filled == 0){
						for(int j = 0; j < lengths[slots_table[slot_index].length]; j++){
							slots_table[slot_index].current_domain[j] = slots_table[slot_index].prev_domains->domain[j];
						}
						slots_table[slot_index].remaining_words = slots_table[slot_index].prev_domains->remaining_values;
						temp = slots_table[slot_index].prev_domains;
						slots_table[slot_index].prev_domains = slots_table[slot_index].prev_domains->next;
						free(temp->domain);
						free(temp);
					}
				}
				return 0;
			}
		}
	}
	return 1;
}
