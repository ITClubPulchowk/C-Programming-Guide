#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "symbol_table.h"

struct symbol_table init_symbol_table()
{
    struct symbol_table sym_table;
    sym_table.current_symbol_table_index = 0;
    
    sym_table.symbol_table = malloc(sizeof(struct symbol) * MAX_SYMBOL_TABLE_SIZE);
    if(!sym_table.symbol_table)
	assert(!"Failed to initialize symbol table...");
    return sym_table;
}

int find_symbol(struct symbol_table* sym_table, struct symbol sym)
{
    /* for(int i = 0; i < MAX_SYMBOL_TABLE_SIZE; ++i)  */
    for (int i = 0; i < sym_table->current_symbol_table_index; ++i)
    {
	enum Ordering Ord = cmp_id(sym.id,sym_table->symbol_table[i].id,MAX_SYMID_LEN);
	if (Ord == equal)
	    return i;
    }
    // Return -1 if no match found 
    return -1; 
}

int insert_symbol(struct symbol_table* sym_table, struct symbol sym)
{
    // if size is max return -1
    if (sym_table->current_symbol_table_index == MAX_SYMBOL_TABLE_SIZE)
	return -1;
    sym_table->symbol_table[sym_table->current_symbol_table_index++] = sym;

    // else return the index at which it was inserted
    return sym_table->current_symbol_table_index-1;
    
}
enum Ordering cmp_id(char *str1, char* str2, int max)
{
    int a = strncmp(str1,str2,max); // might do a rewrite but just leave it
    if (!a)
	return equal;
    if (a<0)
	return less;
    if (a>0)
	return greater;
}

int clean_up(struct symbol_table* sym_table)
{
    free(sym_table->symbol_table);
    return 0;
}
    
