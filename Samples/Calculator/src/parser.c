
// Our calculator is only going to support following operations for the time being ..
/*
 +---------------------------------+-----------------------------------------+
 |  +          	      		   | Unary plus               	             |
 |  -          	      		   | Unary minus     	   	             |
 |---------------------------------|-----------------------------------------|
 |  ^          	      		   | Exponentiation operator                 |
 |---------------------------------|-----------------------------------------|
 |  %        	      	           | Remainder operator                      |
 |  /                              | Division 	     	   	             |
 |  *                          	   | Multiplication    	 	             |
 |---------------------------------|-----------------------------------------|
 |  +                              | Binary addition 	       	             |
 |  -                              | Binary subtraction   	             |
 |                                 |                 	   	             |
 |                                 |                                         |
 +---------------------------------+-----------------------------------------+
*/
// Add parentheses at the top

// We are going to define the grammar for our calculator's expressions

/*
  expr   -> expr + term | expr - term | term
  term   -> term * factor | term / factor | term % factor | factor
  factor -> factor ^ base |  base
  base     -> digit | (expr)
  digit  -> 0|1|2|3|4|5|6|7|8|9|. // Need some regex here
*/
// Ok .. TOC comes in handy here .. But this grammar can't be used to parse the given expression with LL(1) grammar and predictive parsing ....

// Need to define regular languages for the digit and id which are done is regex.h & regex.c

/*
 +--------------------------------------------------------------------------------------------------------------------------------------------------------+
 |    	    						    												  |
 |    	    	    (0-9)	      			    												  |
 |    	    	(int accept)	     	(valid-id)  /-------- +---+											  |
 |    	    	      +---+  	    /---------------          |q3 |   --\										  |
 |    	    	      |q1 |---------         		      +---+	 ---\										  |
 |    	           />>+---+-- 				  (Recoverable)     ---\									  |
 |    	   (0-9)  /-   	      \----   			    		         ---\									  |
 |    	       /-   		   \-----   		    			     ---\     								  |
 |    	     /-  		         \----    (.) 	    			         ---\    							  |
 |    	    / 				      \----    	    				     ---\     							  |
 |	+---+					   \-----   					 ---\  							  |
 |  -->	| q0|----  (op)    +----+		         \----   +----+ 	(0-9)		    +---+  						  |
 |	+---+	\--------> | q8	|			      \--| q2 | 	      /-------------|q4 |(accepting state)				  |
 |	  \ 		   +----+			         +----+ --------------              +---+  (0-9)					  |
 |         \						  (float-intermediate)        		      --						  |
 |	   |						    					    -/ 							  |
 |	    \						    					 --/ 							  |
 | (valid-id) \						    				      --/   							  |
 |           |                                              				    -/     							  |
 |	      \									         --/  (.)   							  |
 |	       +---+     							      --/       							  |
 |	       | q6|----  						  +----+    -/	      								  |
 |	       +---+    \-------     	     +----+  			  | q5 |  -/	 								  |
 |	(valid id + 0-9)         \------      |q7  |  			  +----+ 								          |
 |				       \---  +----+  													  |
 |                                          (Irrecoverable)                                                                                                |
 |                                                                                                                                                        |
 +--------------------------------------------------------------------------------------------------------------------------------------------------------+
*/
// So now we need to convert the above grammar into equivalent grammar without left recursion

// fig is the mess .. but should give general idea

// Left recursion would destroy the left assosciativity of the operators

// Still mentioning grammar without left recursion

// expr   -> term Y
// Y      -> + term Y | - term Y | e (null production)
// term   -> factor Z
// Z      -> * factor Z | / factor Z | % factor Z | e
// factor -> base X
// X      -> ^ X | e
// base   -> digit | (expr)
// digit  -> 0|1|2|3|4|5|6|7|8|9

// Note : 
// Above grammar is LL(1) grammar and is free from left recursion
// This grammar doesn't preserve the left assosciativity of the preceding grammar but is equivalent in terms of language recognizing/generating ability.

// The above grammar without converting to left recursion can be parsed with a more powerful LR(k) grammar and is a bit complex and I don't know how to do. Have no interest in opening the infamous dragon book until I do this calculator naively myself.

// So we refer to another classic solution for this kind of things
// Make a new grammar which is unambiguous as well as preserve associativity and precedence


// Now let's start by building the grammar that will be used for parsing and evaluating the expression


/*
  expr   -> term (+ term)* | term (- term)* | term  // Here ()* is the kleene star
  term   -> factor (* factor)* | factor (\ factor)* | factor (% factor)* | factor
  factor -> base | base (^ factor)
  base   -> digits | (expr) | +digits | -digits
  digits -> output from DFA

*/

// This grammar is subjected to change

// Ok changed ..

// Only added production will be listed .. regex already can handle id
/* expr   -> assign
   assign -> id = id || id = expr ( which maybe anything now )
   base   -> id
*/

// Need to modify the parser now

// Note :
/* The above grammar is not LL(1) grammar and is not straightforward to parse.
   It is unambiguous for certain precedence of operator, preserves
   assosciativity and is efficient;

   The important thing to understand is that :
   left associative operator like '+' , '-' , '*' ,'/' or '%' are consumed in
   iterative while loop.
   right assosciative operator like '^' are consumed in the right recursive call 
*/



#include "regex.h"
#include "parser.h"
#include "io_stream.h"
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

// Now the actual implemntation of the expression evaluator

// Grammar got changed the parsing got changed .. No LL(k) or LR(k) used

#include "symbol_table.h"


return_type calc_run(struct stream* input_stream, token* lookahead)
{
    // With this extra bool, I can now add extra error messages
    return expression(input_stream,lookahead,false);
}

// 
return_type expression(struct stream* input_stream, token* lookahead, bool called_from_base) // lookahead is the token that consumes 1 token ahead before parsing
{

    return_type term1 = term(input_stream,lookahead);

    if (term1.type == ERR_TYPE)
    {
	return term1;
    }

    if ( (lookahead->type == OP) && ( lookahead->op == '='))
    {
	if ( (term1.type == INT_NUM) || (term1.type == FLOAT_NUM))
	{
	    input_stream->log.end_index = input_stream->cur_pos-2;
	    // assert(!"Literal not accepted as lvalues.");
	    // return from error with l_value error .. ");
	    
	    return_type error;
	    error.type = ERR_TYPE;
	    error.error_code = E_LVALUE; // Expected lvalue here..
	    return error; // calling function will handle others .. This way of error propagation .. Whatever. The semantics or syntax of the input is already unusable 
	}
	// call the assign function with term1 as the input
	*lookahead = get_next_token(input_stream);
	term1 = assign(input_stream,&term1,lookahead); // Take id of term to assign rhs

	if (term1.type == ERR_TYPE)
	    return term1;
    }

    do
    {
	if (lookahead->type == OP)
	{
	    if ( !(lookahead->op == '+') && !(lookahead->op == '-')) // Negation of ~(a v b)
		break;
	}
	else
	    break;

	// Weird huh . .but it is equivalent to
	// while ((lookahead->op == '+') or (lookahead->op == '-') { .. }

	char operator = lookahead->op;


	*lookahead = get_next_token(input_stream);

	return_type term2 = term(input_stream,lookahead);

	if (term2.type == ERR_TYPE)
	    return term2;
	
	// add the term1 and term2 accordingly
	switch (operator) // whoo.. its getting complex for just int and float
	{
	case '+' :
	    // LOL there are four different combinations .. Need a better way than this
	    if (term1.type == INT_NUM)
	    {
		if (term2.type == INT_NUM)
		    term1.i_num += term2.i_num;
		else
		{
		    term1.type = FLOAT_NUM;
		    term1.f_num = term1.i_num + term2.f_num;
		}
	    }
	    else
	    {
		if (term2.type == INT_NUM)
		    term1.f_num += term2.i_num;
		else
		    term1.f_num += term2.f_num;
	    }
	    break;
	case '-' :
	    // copy paste above
	    if (term1.type == INT_NUM)
	    {
		if (term2.type == INT_NUM)
		    term1.i_num -= term2.i_num;
		else
		{
		    term1.type = FLOAT_NUM;
		    term1.f_num = term1.i_num - term2.f_num;
		}
	    }
	    else
	    {
		if (term2.type == INT_NUM)
		    term1.f_num -= term2.i_num;
		else
		    term1.f_num -= term2.f_num;
	    }
	    break;
	default :
	    assert(!"Error in the expression term .. .");
	}
    } while(1);
    // Above must be the top ridiculous way to do stupid things.. could have been a one liner if everything were considered float
    // This function returns after the evaluation of first terms

    if (lookahead->type == END_M )
	return term1;
    else
    {

	// LOL.. is this even a grammar at this point . .All look like stitch and patch work (*.*) 
	if ((lookahead->type == OP) && (lookahead->op == ')')) 
	{
	    if (called_from_base)
	    {
		called_from_base = false;
		return term1;
	    }
	    else
	    {
		term1.type = ERR_TYPE;
		term1.error_code = E_RPAREN;
		input_stream->log.start_index = input_stream->prev_index;
		input_stream->log.end_index = input_stream->cur_pos;
		return term1;
	    }
	}
      
    	term1.type = ERR_TYPE;
	term1.error_code = E_OPERATOR;
	input_stream->log.start_index = input_stream->prev_index;
	input_stream->log.end_index = input_stream->cur_pos;
    	return term1;
    }

    return term1;
}

return_type term(struct stream* input_stream, token* lookahead)
{
    return_type factor1 = factor(input_stream,lookahead);

    // Error propagation
    if (factor1.type == ERR_TYPE)
	return factor1;
    
    do
    {
    	char operator = lookahead->op;
	int start_error_index = input_stream->prev_index;
    	if(lookahead->type == OP)
	{
	    if ( !(operator == '*' ) && !(operator == '/') && !(operator == '%'))
    		break;
	}
	else
	    break;


    	*lookahead = get_next_token(input_stream);
	int end_error_index = input_stream->prev_index;
    	return_type factor2 = factor(input_stream,lookahead);
	if(factor2.type == ERR_TYPE)
	    return factor2;

    	switch(operator)
    	{
    	case '%':
    	    if ( (factor1.type != INT_NUM) || (factor2.type != INT_NUM))
    	    {
    		// assert(!"Illegal operation .. Modulus (precisely remainder) not defined for any other types than INT.");
		// We need error recovery not forceful exiting of the program
		// We already have factor2, so don't create temp but use it for error back propagation
		factor2.type = ERR_TYPE;
		factor2.error_code = I_MOD;
		input_stream->log.start_index = start_error_index;
		input_stream->log.end_index = end_error_index;
		return factor2;
    	    }
    	    else
    	    {
    		factor1.i_num  = factor1.i_num % factor2.i_num;
    	    }
    	    break;
    	case '*':
    	    // copy paste again
    	    if (factor1.type == INT_NUM)
    	    {
    		if (factor2.type == INT_NUM)
    		    factor1.i_num *= factor2.i_num;
    		else
    		{
    		    factor1.type = FLOAT_NUM;
    		    factor1.f_num = factor1.i_num * factor2.f_num;
    		}
    	    }
    	    else
    	    {
    		if (factor2.type == INT_NUM)
    		    factor1.f_num *= factor2.i_num;
    		else
    		    factor1.f_num *= factor2.f_num;
    	    }
    	    break;
    	case '/' :
    	    if (factor1.type == INT_NUM)
    	    {
    		if (factor2.type == INT_NUM)
    		    factor1.i_num /= factor2.i_num;
    		else
    		{
    		    factor1.type = FLOAT_NUM;
    		    factor1.f_num = factor1.i_num / factor2.f_num;
    		}
    	    }
    	    else
    	    {
    		if (factor2.type == INT_NUM)
    		    factor1.f_num /= factor2.i_num;
    		else
    		    factor1.f_num /= factor2.f_num;
    	    }
    	    break;

    	}
    } while(1);
    return factor1;
}

return_type factor(struct stream* input_stream, token* lookahead)
{
    // base will be either digit or digit ^ base
   
    return_type base1 = base(input_stream,lookahead);
    if (base1.type == ERR_TYPE)
	return base1;
    
    if (lookahead->type == OP)
    {
	if(lookahead->op == '^')
	{
	    *lookahead = get_next_token(input_stream);
	    return_type base2 = base(input_stream,lookahead);
	    // calculate the exponentation

	    if (base2.type == ERR_TYPE)
		return base2;

	    if (base1.type == INT_NUM)
	    {
		if(base2.type == INT_NUM)
		{
		    // return the integer exponentiation of base 1 wrt to base 2
		    // guess I will write it myself
		    
		    // It doesn't handle negative exponentiation
		    // will modify later
		    
		    base1.i_num = exponentiation(base1.i_num, base2.i_num);
		}
		else
		{
		    base1.type = FLOAT_NUM;
		    base1.f_num = pow(base1.i_num,base2.f_num);
		}
	    }
	    else
	    {
		if (base2.type == INT_NUM)
		    base1.f_num = pow(base1.f_num,base2.i_num);
		else
		    base1.f_num = pow(base1.f_num,base2.f_num);
	    }
	}
    }
    return base1;
}

int exponentiation(int base, int index)
{
    if ( (base == 0) && (index == 0))
	return 1;
    // Need a lot of things to be done here ...
    // lets use tail recursion
    // Reason : Nothing.. just fun
    return tail_exp(base, index, 1);
}

int tail_exp(int base, int index, int result)
{
    if (index == 0)
	return result;
    return tail_exp(base,index-1,result*base);
}


return_type base(struct stream* input_stream, token* lookahead)
{
    // Four different cases
    // (expr) | digit | '+' digit | '-' digit
    token current_token = *lookahead;

    if (current_token.type == ERR_TYPE)
	return current_token;
    
    if ((current_token.type == INT_NUM) ||
	(current_token.type == FLOAT_NUM))
    {

	return_type temp;
	if(current_token.type == INT_NUM)
	{
	    temp.type = INT_NUM;
	    temp.i_num = current_token.i_num;
	}
	else
	{
	    temp.type = FLOAT_NUM;
	    temp.f_num = current_token.f_num;
	}

	// should increae the lookahead right ...
	*lookahead = get_next_token(input_stream);
	if (lookahead->type == ERR_TYPE)
	    return *lookahead;
	return temp;
    }

    if (current_token.op == '(')
    {
	// Start shenagians
	// consume the left parenthesis
	*lookahead = get_next_token(input_stream);
	if (lookahead->type == ERR_TYPE)
	    return *lookahead;
	// return_type expr = expression(input_stream, lookahead);
	return_type expr = expression(input_stream, lookahead,true);
	

	if (expr.type == ERR_TYPE)
		return expr;

	if(lookahead->type!=OP)
	{
	    // assert(!"Left parenthesis missing.");
	    // again take expr and return the corresponding error ..
	    if (lookahead->op != ')')
	    {
		expr.type = ERR_TYPE;
		expr.error_code = L_MISS;
		input_stream->log.start_index = input_stream->prev_index;
		input_stream->log.end_index = input_stream->cur_pos;
		return expr;
	    }
	}

	
	*lookahead = get_next_token(input_stream);
	// match ')'
	// return
	if (lookahead->type == ERR_TYPE)
	    return *lookahead;
	return expr;
    }

    if(current_token.type == ID)
    {
	return_type value = *lookahead;
	int record_index = input_stream->prev_index;

	*lookahead = get_next_token(input_stream);

	if (lookahead->type == ERR_TYPE)
	    return *lookahead;

	
	// if lookahead -> '=' return ID .. otherwise return the decoded value from the symbol table

	// This func determines if expr need to call assign .. There's no straight way
	if ((lookahead->type == OP) && (lookahead->op == '='))
	    return value;

	// else convert it into it valid from from the symbol tabl e

	// just do it here instead of making new functions only

	// Read from the symbol table and decipher it ...

	struct symbol sym;
	str_cpy(sym.id, value.id, MAX_SYMID_LEN);

	// Check if it is exit literal before checking the symbol table
	if (cmp_id(value.id, "exit", 4) == equal)
	{
	    value.type = ERR_TYPE;
	    value.error_code = U_SYM;
	    return value;
	}
	int table_index = find_symbol(&input_stream->table, sym);
	if (table_index == -1)
	{
	    // assert(!"unknown identifier appeared on the expression.");

	    input_stream->log.start_index = record_index;
	    input_stream->log.end_index = input_stream->prev_index;
	    value.type = ERR_TYPE;
	    value.error_code = E_SYM_ID; // error_code to represnt identifier not available on the symbol table
	   
	    return value;
	    
	}

	sym = input_stream->table.symbol_table[table_index];

	str_cpy(value.id, sym.id, MAX_SYMID_LEN);

	switch (sym.type)
	{
	case INT_TYPE :
	    value.type = INT_NUM;
	    value.i_num = *((int*)&sym.num); // Again that arcane estoreic casting
	    return value;
	case FLOAT_TYPE :
	    value.type = FLOAT_NUM;
	    value.f_num = *((float*)&sym.num);
	    return value;
	default :
	{
	    value.type = ERR_TYPE;
	    value.error_code = SYN_ERROR;
	    return value;
	    assert(!"No other things should've allowed here .. ");
	}
	}
    }

    switch(current_token.op)
    {
    case '+' :
    {
	// again look ahead and return number with proper sign
	*lookahead = get_next_token(input_stream);
	if(lookahead->type == ERR_TYPE)
	    return *lookahead;
	return_type temp;
	if (lookahead->type == INT_NUM)
	{
	    temp.type = INT_NUM;
	    temp.i_num = lookahead->i_num;
	}
	else if (lookahead->type == FLOAT_NUM)
	{
	    temp.type = FLOAT_NUM;
	    temp.f_num = lookahead->f_num;
	}
	else
	{
	    // Throw an error of rvalue_expected 
	    // assert(!"Syntax error is base func...");
	    // reuse temp
	    temp.type = ERR_TYPE;
	    temp.error_code = E_RVALUE;
	    input_stream->log.start_index = input_stream->prev_index;
	    input_stream->log.end_index = input_stream->cur_pos;
	    return temp;
	}
	*lookahead = get_next_token(input_stream);
	if(lookahead->type == ERR_TYPE)
	    return *lookahead;
	return temp;
	break;
    }
    case '-' :
    {
	// same but return -ve
	*lookahead = get_next_token(input_stream);
	if(lookahead->type == ERR_TYPE)
	    return *lookahead;
	return_type temp;
	if (lookahead->type == INT_NUM)
	{
	    temp.type = INT_NUM;
	    temp.i_num = - lookahead->i_num;
	}
	else if (lookahead->type == FLOAT_NUM)
	{
	    temp.type = FLOAT_NUM;
	    temp.f_num = - lookahead->f_num;
	}
	else
	{
	    // Expected literal here
	    // Need lvalue here
	    // assert(!"Syntax error is base func...");

	    temp.type = ERR_TYPE;
	    temp.error_code = E_RVALUE;
	    input_stream->log.start_index = input_stream->prev_index;
	    input_stream->log.end_index = input_stream->cur_pos;
	    return temp;
	}
	*lookahead = get_next_token(input_stream);
	if(lookahead->type == ERR_TYPE)
	    return *lookahead;
	return temp;
	break;
    }
    default ://  print_token(*lookahead); assert(!"Somethings off here idiot..");
    {
	return_type temp;
	temp.type = ERR_TYPE;
	temp.error_code = E_OP;
	input_stream->log.start_index = input_stream->prev_index;
	input_stream->log.end_index = input_stream->cur_pos;
	return temp;
    }
    }
    return current_token;
}


return_type assign(struct stream* input_stream, token* lhs, token* lookahead)
{
    // make token lhs concrete
    token lvalue = *lhs;
    token rvalue = expression(input_stream,lookahead,false);
    if(rvalue.type == ERR_TYPE)
	return rvalue;

    // Examine the rvalue and check whether it is digit or id ...
    // if it is digit -> assign lvalue -> rvalue
    // if rhs is id
    //     -> check if it is already available in symbol_table
    //        -> if yes , retrieve its value
    //     -  -> else issue warning

    // Convert the token into symbol first
    struct symbol sym;

    str_cpy(sym.id, lvalue.id, MAX_SYMID_LEN); // both id_len have same max size -> no potential overflow

    // Search the symbol table if it is already present
    // Lol .. symbol table hasn't been passed yet... So guess what .. lets embed symbol table in the input stream ... Hahahaha

    int search_index = find_symbol(&input_stream->table,sym);
    // if it is -1 it is not in the symbol table .. so add it to symbol table
    if (search_index == -1)
    {
	if (cmp_id(lvalue.id, "exit",4) == equal)
	{
	    lvalue.type = ERR_TYPE;
	    lvalue.error_code = U_SYM;
	    return lvalue;
	}
	
	int err = insert_symbol(&input_stream->table, sym); // err is either -1 or index of the inserted symbol

	if (err == -1)
	    assert(!"Failed to insert the symbol into the symbol table.");

	search_index = err;
    }
    // Fill in the relevant detail now

    if ( (rvalue.type == INT_NUM) ||
	 (rvalue.type == FLOAT_NUM))
    {
	// assign rvalue to lvalue
	// before check if lvalue is in symbol_table or not
	if (rvalue.type == INT_NUM)
	{
	    input_stream->table.symbol_table[search_index].type = INT_TYPE;
	    // input_stream->table.symbol_table[search_index].num = (union num)rvalue.i_num; // Don't worry .. we will recover the value from num by reverse casting
	    // Note : 
	    // The above style of casting to union type is GNU extension .. so it should not be casted but assigned ..
	    input_stream->table.symbol_table[search_index].num.i_num = rvalue.i_num;
	    return rvalue;
	}
	else
	{
	    input_stream->table.symbol_table[search_index].type = FLOAT_TYPE;
	    // input_stream->table.symbol_table[search_index].num = (union num)rvalue.f_num; // Same .. can be recovered
	    input_stream->table.symbol_table[search_index].num.f_num = rvalue.f_num;
	    return rvalue;
	}

    }

    // Now need of else
    if (rvalue.type == ID)
    {
	// Look for this symbol in the symbol_table
	// Use the above sym
	str_cpy(sym.id,rvalue.id,MAX_SYMID_LEN);

	int rvalue_index = find_symbol(&input_stream->table, sym);
	if (rvalue_index == -1)
	    assert(!"Id appearing on the right side is not valid variable_name.");

	else
	{
	    // copy the info of rvalue into lvalue
	    input_stream->table.symbol_table[search_index].type = input_stream->table.symbol_table[rvalue_index].type;
	    input_stream->table.symbol_table[search_index].num = input_stream->table.symbol_table[rvalue_index].num;
	}
	// Return the result of assignment to make it fancyyy
	// So we will use rvalue
	switch (input_stream->table.symbol_table[rvalue_index].type)
	{
	case INT_TYPE:
	    rvalue.type = INT_NUM;
	    rvalue.i_num = *((int*)&input_stream->table.symbol_table[rvalue_index].num); // This needs some attention and review
	    return rvalue;

	case FLOAT_TYPE :
	    rvalue.type = FLOAT_NUM;
	    rvalue.f_num = *((float*)&input_stream->table.symbol_table[rvalue_index].num); // Such an arcane thing I did
	    return rvalue;
	default : assert(!"No other possible switches left...");
	}
    }

    assert(!"There's nothing to assign here .. ");
    return rvalue; // Its not going to be called anyway .. So
}

