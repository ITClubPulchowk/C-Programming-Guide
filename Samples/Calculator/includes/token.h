#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include "io_stream.h"
// Let's just forward declare the struct io_stream without including the relevant header file

// stream;

#define MAX_ID_LENGTH 50 // All define and enum are ad hoc.. They can be managed systematically

enum token_type { ID, OP, INT_NUM,FLOAT_NUM, ERR_TYPE,END_M };

enum errors { Q2, Q7, Q5, E_LVALUE,E_RVALUE, E_OPERATOR, E_SYM_ID, I_MOD, U_SYM,L_MISS, E_OP, SYN_ERROR, E_RPAREN}; // Will add other as necessary


// when token type is null, error will represent the error code

struct token
{
    enum token_type type;
    char op; // For operator
    char id [MAX_ID_LENGTH];
    int i_num;
    float f_num;
    enum errors error_code;
};


typedef struct token token;

token get_next_token(stream* ); // These pointer are passed to make the use of global variable redundant.. Thats functional (but not pure)

token next_token(stream*);

#endif

