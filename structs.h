#ifndef STRUCTS_H
#define STRUCTS_H
typedef struct fc_listnode *fc_Listptr;

typedef struct fc_listnode* fc_Listptr;

struct fc_listnode{ 
	int* domain; 
	int remaining_values;
	fc_Listptr next;
};

typedef struct{ 
	int i;
	int j;
	int row_col; //1 αν το slot ειναι οριζοντιο, αλλιως 0 αν ειναι καθετο
	int length;
    	int filled; 
	int index;  
	int remaining_words; 
	int conflicts[33];
	int* neighbors; 
	int** neighbors_information;
	int* current_domain; 
	fc_Listptr prev_domains; 
} Slot;

typedef struct listnode* Listptr;

struct listnode{ 
    int slot_ind;
    Listptr next;
};
#endif
