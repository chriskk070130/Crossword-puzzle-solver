#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "checks.h"

int check_solution(FILE* dictionary, char** crossw, int dimensions, Slot* slots_table, int n_myslots){ 
    Slot next;    //Κάθε φορά συμπληρώνω ένα-ένα slot με την σειρά αφού για να βρεθούν σαρώθηκαν πρώτα οι γραμμές και μετά οι στήλες.
	int found, letter, i, j, stop=0, index=0;
	char *word, *str;
	str = malloc(33 * sizeof(char));
	word = malloc(33 * sizeof(char));
	if(str == NULL || word == NULL)
		return -1;
	while(fgets(str , 33 , stdin) != NULL){
		found = 0;
		while(fgets(word , 33 , dictionary) != NULL){
			if(!strcmp(word , str)){
				found = 1;
			}
		}
		str[strlen(str) - 1] = '\0';
		if(!found){
			printf("Word \"%s\" not in dictionary\n" , str);
			free(word);
			free(str);
			return 0;
		}
		rewind(dictionary);
		if(check_if_completed(slots_table , n_myslots) == 1 && stop){
			printf("More words than needed\n");
			free(word);
			free(str);
			return 0;
		}
		next = slots_table[index]; 
        index++;
		if(next.length != strlen(str)){				   
			printf("Word \"%s\" cannot be placed\n" , str);
			free(word);
			free(str);
			return 0;
		}
		else{
			i = next.i;
			j = next.j;
			if(next.row_col == 1){ 
				for(letter = 0; letter < strlen(str); letter++){ //Στις γραμμές δεν υπάρχουν conflicts αφού αυτές συμπληρώνονται πρώτες
					crossw[i][j] = str[letter];
					j++;
				}
			}
			else{ 
				for(letter = 0; letter < strlen(str); letter++){
					if(crossw[i][j] == ' '){        
						crossw[i][j] = str[letter];
						i++;
					}
					else if(crossw[i][j] == str[letter]){ 
						i++;							  
						continue;
					}
					else{ 
						printf("Word \"%s\" cannot be placed\n" , str);
						free(word);
						free(str);
						return 0;
					}
				}	
			}
		}
        if(index == n_myslots){ //Το check-mode έχει τελειώσει
            stop = 1;
        }
	}
	if(!stop){ 
		printf("Not enough words\n");
		free(word);
		free(str);
		return 0;
	}
	free(word);
	free(str);
	return 1;
}

int check_if_completed(Slot* slots_table, int n_myslots){ 
    for(int sl_ind = 0; sl_ind < n_myslots; sl_ind++)
        if(!slots_table[sl_ind].filled)
            return 0;
    return 1;
}
