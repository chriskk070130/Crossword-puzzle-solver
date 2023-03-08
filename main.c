#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "checks.h"
#include "solve.h"
#include "solve_functions.h"

void print_board(char** table, int n);

int main(int argc, char* argv[]){ 
	int i, j, dimensions, a, b, ind_dict, length, row, col, first, ind, max_slots, position, ch;
    int check=0, dict=0, draw=0, count_slots=0, num_of_words[33], save_words[33], ****bdict, *needed_lengths; 
	char ***words, **crossw, *str; 
    Slot *myslots;
    fc_Listptr temp;
	FILE *crossword_file=NULL, *dictionary_file=NULL;
	for(i = 1; i < argc; i++){
		if(!strcmp(argv[i] , "-check")){
			check = 1;
		}
		else if(!strcmp(argv[i] , "-dict")){
			dict = 1;
			ind_dict = i;
		}	
		else if(!strcmp(argv[i] , "-draw")){
			draw = 1;
		}
	}
	if((crossword_file = fopen(argv[1] , "r")) == NULL){
		fprintf(stderr , "Could not open file\n");
		exit(EXIT_FAILURE);
	}
	else{ //Δαμόρωση του grid
		fscanf(crossword_file , "%d" , &dimensions);
		if((crossw = malloc(dimensions * sizeof(char*))) == NULL){ 
            fprintf(stderr , "Error allocating memory\n");
            exit(EXIT_FAILURE);
        }
		for(i = 0; i < dimensions; i++){
			if((crossw[i] = malloc(dimensions * sizeof(char))) == NULL){
                fprintf(stderr , "Error allocating memory\n");
                exit(EXIT_FAILURE);
            }
			for(j = 0; j < dimensions; j++){
				crossw[i][j] = ' ';
			}
		}
		while((fscanf(crossword_file , "%d" , &a) != EOF) && (fscanf(crossword_file , "%d" , &b) != EOF)){
			crossw[a-1][b-1] = '#';
		}
	}
	if((needed_lengths = calloc(33 , sizeof(int))) == NULL){ //Πίνακας όπου στην θέση i έχει 1 αν το μήκος i χρειάζεται για το σταυρόλεξο αλλιώς 0
		fprintf(stderr, "Error allocating mamory\n");
        exit(EXIT_FAILURE);
	}
	if(dict){
		if((dictionary_file = fopen(argv[ind_dict + 1] , "r")) == NULL){
            fprintf(stderr , "Could not open file\n");
            exit(EXIT_FAILURE);
        }
	}
	else{
		if((dictionary_file = fopen("Words.txt" , "r")) == NULL){
            fprintf(stderr , "Could not open file\n");
            exit(EXIT_FAILURE);
        }
	}
    max_slots = dimensions*dimensions/2 + 1;
	if((myslots = malloc(max_slots * sizeof(Slot))) == NULL){
        fprintf(stderr , "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    for(row = 0; row < dimensions; row++){ //Ευρεση των slot και αρχικοποίηση των βασικών μελών τους
        col = 0;                           //που αρκούν για την υλοποίηση της -check λειτουργίας
        while(col < dimensions  - 1){
            length = 1;
			first = col;
            while(crossw[row][col] == ' ' && crossw[row][col+1] == ' '){
                length++;
                col++;
                if(col > dimensions - 1){
          		    break;
                }
            }
            if(length >= 2){
				if(needed_lengths[length] == 0)
                	needed_lengths[length] = 1;
				ind = count_slots++;
				myslots[ind].i = row;
				myslots[ind].j = first;
				myslots[ind].length = length;
				myslots[ind].row_col = 1;
            }
            if(col >= dimensions - 1){
                continue;
            }
            col++;
        }
    }
    for(col = 0; col < dimensions; col++){
        row = 0;
        while(row < dimensions  - 1){
            length = 1;
			first = row;
            while(crossw[row][col] == ' ' && crossw[row+1][col] == ' '){
                length++;
                row++;
                if(row >= dimensions - 1){
                    break;
                }
            }
            if(length >= 2){
				if(needed_lengths[length] == 0)
                	needed_lengths[length]++;
				ind = count_slots++;
				myslots[ind].i = first;
				myslots[ind].j = col;
				myslots[ind].length = length;
				myslots[ind].row_col = 0;
            }
            if(row >= dimensions - 1){
                continue;
            }
            row++;
        }
    }
	if((myslots = realloc(myslots , count_slots * sizeof(Slot))) == NULL){
        fprintf(stderr, "Error allocating memory");
        exit(EXIT_FAILURE);
    }    
    if(check){ //check mode
		int check_result = check_solution(dictionary_file, crossw, dimensions, myslots, count_slots);
        free(myslots);
        if(check_result == -1){ //Αποτυχία δέσμευσης μνήμης μέσα στην συνάρτηση check 
			fprintf(stderr , "Error allocating memory\n");
			exit(EXIT_FAILURE);
		}
        else if(check_result == 1 && draw == 1){ //το check-mode τερμάτισε επιτυχώς 						
             print_board(crossw, dimensions);
        }
    }
	else{ //solve crossword puzzle
		if((bdict = malloc(33 * sizeof(int***))) == NULL){ //Βοηθητικός πίνακας για την γρήγορη εύρεση του domain κάθε slot. 
            fprintf(stderr, "Error allocating memory\n");   //Η δομή του είναι η εξής: πλήθος γραμμάτων->θέση γράμματος->γράμμα->πίνακας από 0 και 1
            exit(EXIT_FAILURE);								//μεγέθους όσο το πλήθος των λέξεων του μήκους <πλήθος γραμμάτων>, 								
        }													//1 αν η αντίστοιχη λέξη ανήκει στο domain αλλιώς 0. Λειτουργεί παράλληλα με τον πίνακα words.
		words = malloc(33 * sizeof(char**)); //Βασικός πίνακας αποθήκευσης συμβολοσειρών με κριτήριο το μέγεθος τους
		str = malloc(33 * sizeof(char)); 
		if(words == NULL || str == NULL){
			fprintf(stderr , "Error allocating memory\n");
			exit(EXIT_FAILURE);
		} 
		for(i = 0; i < 33; i++){
			words[i] = NULL;
			num_of_words[i] = 0; //Πλήθος λέξεων μήκους i
			save_words[i] = 0;
			bdict[i] = NULL;
		}
		while(fgets(str , 33 , dictionary_file) != NULL){ 
			str[strlen(str) - 1] = '\0';
			length = strlen(str);
			if(needed_lengths[length])
				num_of_words[length]++;
		}
		rewind(dictionary_file);
		for(i = 2; i < 33; i++){ 
			if(!needed_lengths[i]){  
				continue;
			}
			else{
				bdict[i] = malloc(i * sizeof(int**)); 
				words[i] = malloc(num_of_words[i] * sizeof(char*));
				if(words[i] == NULL || bdict[i] == NULL){
					fprintf(stderr , "Error allocating memory \n");
					exit(EXIT_FAILURE);
				}
				for(j = 0; j < i; j++){
					if((bdict[i][j] = malloc(26 * sizeof(int*))) == NULL){
                        fprintf(stderr , "Error allocating memory\n");
					    exit(EXIT_FAILURE);
                    }
					for(int k = 0; k < 26; k++){ 
						bdict[i][j][k] = calloc(num_of_words[i] , sizeof(int)); //Αρχικοποίηση με 0
                        if(bdict[i][j][k] == NULL){
                            fprintf(stderr , "Error allocating memory\n");
					        exit(EXIT_FAILURE);
                        }
					}
				} 
				for(j = 0; j < num_of_words[i]; j++){ 
					if((words[i][j] = malloc((i+1) * sizeof(char))) == NULL){
                        fprintf(stderr, "Error allocating memory\n");
                        exit(EXIT_FAILURE);
                    }
				}
			}
		}
		while(fgets(str , 33 , dictionary_file) != NULL){ 
			str[strlen(str) - 1] = '\0';  
			length = strlen(str);
			if(needed_lengths[length]){ 
				strcpy(words[length][save_words[length]] , str);  //Aρχικοποίηση του words
				for(position = 0; position < length; position++){ //Αρχικοποίηση του bdict
					ch = str[position] - 'a';
					bdict[length][position][ch][save_words[length]] = 1;
				}
				save_words[length]++; 
			}
		}
		for(i = 0; i < count_slots; i++){ 
			myslots[i].current_domain = malloc(num_of_words[myslots[i].length] * sizeof(int)); 
            if(myslots[i].current_domain == NULL){
                fprintf(stderr, "Error allocating memory\n");
                exit(EXIT_FAILURE);
            }
			for(j = 0; j < num_of_words[myslots[i].length]; j++){
				myslots[i].current_domain[j] = 1; 
			}
			for(j = 0; j < 33; j ++){
				myslots[i].conflicts[j] = 0;
			}
			myslots[i].remaining_words = num_of_words[myslots[i].length]; 
			myslots[i].prev_domains = NULL; //Στοίβα στην οποία κρατάω τα προηγούμενα domains ενός slot μετά από αλλαγή του current_domain του
			myslots[i].index = 0; //Κρατάω το index του current_domain στο οποίο βρέθηκε η λέξη για το slot αυτό(αρχικά είναι 0)
			myslots[i].filled = 0;
		}
		find_neighbors(crossw, myslots, count_slots); 
		int solve_result, return_value = 0;
        solve_result = solve(crossw , dimensions , words , num_of_words, bdict, myslots, count_slots);
		if(solve_result == 0){
			printf("Cannot find solution\n");
			return_value = 1;
		}
		else{
			for(i = 0; i < count_slots; i++){ 
				printf("%s\n" , words[myslots[i].length][myslots[i].index]);
			}
		}
		if(draw){
			int is_horizontal, letter;
			for(int slot_index = 0; slot_index < count_slots; slot_index++){
				is_horizontal = myslots[slot_index].row_col;
				length = myslots[slot_index].length;
				row = myslots[slot_index].i;
				col = myslots[slot_index].j;
				letter = 0;
				if(is_horizontal){
					for(int pos = col; pos <= col+length-1; pos++){
						crossw[row][pos] = words[length][myslots[slot_index].index][letter++];
					}
				}
				else{
					for(int pos = row; pos <= row+length-1; pos++){
						crossw[pos][col] = words[length][myslots[slot_index].index][letter++];
					}
				}
			}
			print_board(crossw, dimensions);
		}
        //Αποδέσμευση της μνημης
		for(i = 0; i < 33; i++){
			for(j = 0; j < num_of_words[i]; j++){
				free(words[i][j]);
			}
			free(words[i]);
		}
		free(words);
        for(i = 0; i < 33; i++){
            if(needed_lengths[i]){
                for(j = 0; j < i; j++){
                    for(int k = 0; k < 26; k++){
                        free(bdict[i][j][k]);
                    }
                    free(bdict[i][j]);
                }
                free(bdict[i]);
            }
        }
        free(bdict);
        free(needed_lengths);
        for(int sl_ind = 0; sl_ind < count_slots; sl_ind++){
            free(myslots[sl_ind].current_domain);
            while(myslots[sl_ind].prev_domains != NULL){
                temp = myslots[sl_ind].prev_domains;
                myslots[sl_ind].prev_domains = myslots[sl_ind].prev_domains->next;
                free(temp->domain);
                free(temp);
            }
            free(myslots[sl_ind].neighbors);
            for(int slot_index = 0; slot_index < count_slots; slot_index++){
                free(myslots[sl_ind].neighbors_information[slot_index]);
            }
            free(myslots[sl_ind].neighbors_information);
        }
        free(myslots);
        free(str);
		if(return_value == 1){
            for(i = 0; i < dimensions; i++)
		        free(crossw[i]);
	        free(crossw);
			exit(EXIT_FAILURE);
        }
	}
	for(i = 0; i < dimensions; i++)
		free(crossw[i]);
	free(crossw);
	fclose(dictionary_file);
	fclose(crossword_file);
	exit(EXIT_SUCCESS);
}

void print_board(char** table, int n){ 
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			if(table[i][j] == '#')
				printf("###");
			else
				printf(" %c ", table[i][j]);
		}
		printf("\n");
	}	
}

