#ifndef PARSER_H
#define PARSER_H
#include "token.h"

// #include "io_stream.h"

// struct stream; // Forward declaration
// typedef messed the forward declaration .. so let's just include io_stream.h
// Again listing the grammar for reference purpose

/*  expr     -> term (+ term)* | term (- term)* | term | assign
    assign   -> id = base 
    term     -> factor (* factor)* | (factor - factor)* | factor 
    factor   -> base | base (^ factor)*
    base     -> digits | ( expr ) | + digits | - digits
    digits   -> DFA results
We will exclude the identifer for now
*/

typedef token return_type; // just typdef

// Need to pass input stream explicitly in order to avoid global mess
// Similarly we are passing the struct token instead of making it global .. That's functional style

// make symbol_table global or let the stream* carry it);

return_type expression (stream * , struct token* , bool);

return_type term (stream *, struct token* );

return_type factor (stream*, struct token* );

return_type base (stream *, struct token* );
return_type assign (stream *, struct token* lhs, struct token* lookahead );

// for proper ad hoc error handling


// Needed to change the function declaration for left parenthesis messages ..
// There must be a better way .. But let's leave it for now

return_type calc_run(stream*, struct token*);

// return_type expression(stream* input_stream, struct token* lookahead, bool called_from_base);


int exponentiation(int base, int index);
int tail_exp(int base, int index, int result);


#endif
