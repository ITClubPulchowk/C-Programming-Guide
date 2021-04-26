#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

//size of symbol table is fixed .. No more than 100 variable declarations
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_SYMID_LEN 50
enum Ordering { less, equal, greater};

enum symbol_type {INT_TYPE,FLOAT_TYPE,NO_TYPE};

// already available token could've been used as symbol...
// but this file was written earlier .. So no changing now 
struct symbol
{
    // The first is name
    char id[MAX_SYMID_LEN]; // Heavenly restriction
    enum symbol_type type;
    union num
    {
	int i_num;
	float f_num; // who cares whatever the name is ... we'll just cast whatever we need
    } num;
};

struct symbol_table
{
    struct symbol* symbol_table;
    int current_symbol_table_index;
};

struct symbol_table init_symbol_table(); // we could've used hash table for better searching and insertion but who cares.. It is the small project so let's just stick with linear array

// return the index of the current symbol in the symbol table .. Return -1 if no matching symbol is found

int find_symbol(struct symbol_table* sym_table,struct symbol);

int insert_symbol(struct symbol_table* sym_table,struct symbol);

// hash entries needn't be deleted during the current interactive session
enum Ordering cmp_id(char*, char*,int ); // last parameter checks the maximum length of the string that is allowed to be compared
// returns less, equal or greater (first parameter with respect to second)

int clean_up(struct symbol_table* ); // Gracefully clean up the program

// create a wrapper for the symbol table

    
#endif 



