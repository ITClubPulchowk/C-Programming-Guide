#include "token.h"
#include "io_stream.h"
#include "regex.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

struct token get_next_token(struct stream* input_stream) // This function was added later to not modify next_token for previous accepted regex
{
    input_stream->prev_index = input_stream->cur_pos;
    return next_token(input_stream);
}

struct token next_token(struct stream* input_stream)
{
    struct token temp_token;
    
    // Initial setup
    temp_token.id[0] = '\0';
    temp_token.type = ERR_TYPE;
    temp_token.f_num = 0;
    temp_token.i_num = 0;
    temp_token.error_code = SYN_ERROR;
    temp_token.op = ' ';
    
    /* memset(temp_token.id,'\0',sizeof(char) * MAX_ID_LENGTH); */

    // It is the dfa simulation for the lexer.
    char buffer[MAX_ID_LENGTH] = {'\0'};
    enum finite_states current_state = q0;
    enum finite_states last_accepting_state = null_state;

    int last_accepted_index = -1;
    int init_stream_pos = input_stream->cur_pos;
    
    if (input_stream->cur_pos >= input_stream->total_len)
    {
	temp_token.type = ERR_TYPE;
	return temp_token;
    }
    
    while(input_stream->cur_pos < input_stream->total_len)
    {
	current_state = transition_func(current_state, input_stream->buffer[input_stream->cur_pos++]);

	// first priority -> check if it is the operator
	if(current_state == q0) // Stupid me
	    init_stream_pos++; 
	if (current_state == q8)
	    break; 
	// if current state is accepting record, it
	// Trying new way of formatting 
	if ((current_state == q1) ||
	    (current_state == q4) ||
	    (current_state == q6))
	{
	    last_accepting_state = current_state;
	    last_accepted_index = input_stream->cur_pos;
	}

	// if there's irrecoverable error, just break from here
	if((current_state == null_state) ||
	   (current_state == q3) ||
	   (current_state == q7) ||
	   (current_state == q5))
	    break;

	// Point to the next character
    };

    if ((current_state == q0) || (current_state == null_state))
    {
	temp_token.type = END_M;
	return temp_token;
    }

    // Return the operator matched token
    if (current_state == q8)
    {
	temp_token.type = OP;
	temp_token.id[0] = '\0';
	temp_token.i_num = 0;
	temp_token.op = input_stream->buffer[input_stream->cur_pos-1];
	return temp_token;
    }

    if ((current_state == q6) || // Trying different styles of formatting
	(current_state == q1) ||
	(current_state == q4))
    {
	// It is the name of the identifier
	int total_consumed_character = input_stream->cur_pos - init_stream_pos;
	memcpy(temp_token.id,input_stream->buffer+init_stream_pos,total_consumed_character * sizeof(char));
	// Incase add the null character
	temp_token.id[total_consumed_character] = '\0';

	if (current_state == q6)
	{
	    temp_token.type = ID;
	    temp_token.op = 0;
	    temp_token.i_num = 0;
	    temp_token.f_num = 0;
	    return temp_token;
	}

	if (current_state == q1)
	{
	    temp_token.type = INT_NUM;
	    temp_token.op = 0;
	    temp_token.i_num = str_to_int(temp_token.id,total_consumed_character); //  str_to_int(temp_token.id,str_len(temp_token.id));
	    temp_token.f_num = temp_token.i_num;
	    return temp_token;
	}

	temp_token.type = FLOAT_NUM;
	temp_token.op = 0;
	temp_token.f_num = str_to_float(temp_token.id,total_consumed_character);
	temp_token.i_num = temp_token.f_num;
	return temp_token;
    }

    // All direct accepting states done .. Now the important parts i.e all the input that halts at q3
    // first let's do the error parts
    if ((current_state == q7) ||
	(current_state == q5))
    {
	temp_token.type = ERR_TYPE;
	if (current_state == q7)
	{ 
	     // fprintf(stderr,"Error undefined symbols used ... ");
	    input_stream->log.start_index = input_stream->prev_index;
	    input_stream->log.end_index = input_stream->cur_pos;
	    temp_token.error_code = Q7; 
	    return temp_token; 
	} 

        // printf(stderr,  "Two periods are forbidden on the same number....");
	temp_token.error_code = Q5;
	input_stream->log.start_index = input_stream->prev_index;
	input_stream->log.end_index = input_stream->cur_pos;
	return temp_token;
    }
    // More on this error later

    if(current_state == q2)
    {
	// Don't know how to trigger it
	// Maybe need a seperate state .. but not going that far ... 
	fprintf(stderr,"Incomplete floating point literal ...");
	
	return temp_token;
    }

    // Now the important one

    if (current_state == q3)
    {
	// It is the recoverable state and shouldn't warn any error
	// Might be the longest one to be implemented
	// I guess the above and this block can be merget with some tweaks .. But let's just not do it

	input_stream->cur_pos--;
	assert(input_stream->cur_pos == last_accepted_index);
	
	int total_consumed_characters = last_accepted_index - init_stream_pos;
	// put the input_stream 1 step back
	
	memcpy(temp_token.id,input_stream->buffer+init_stream_pos,sizeof(char) * total_consumed_characters);
	temp_token.id[total_consumed_characters] = '\0';
	// All others same .. maybe
	
	if (last_accepting_state == q6)
	{
	    temp_token.type = ID;
	    temp_token.op = 0;
	    temp_token.i_num = 0;
	    temp_token.f_num = 0;
	    return temp_token;
	}

	if (last_accepting_state == q1)
	{
	    temp_token.type = INT_NUM;
	    temp_token.op = 0;
	    temp_token.i_num = str_to_int(temp_token.id,total_consumed_characters); //  str_to_int(temp_token.id,str_len(temp_token.id));
	    temp_token.f_num = temp_token.i_num;
	    return temp_token;
	}

	temp_token.type = FLOAT_NUM;
	temp_token.op = 0;
	temp_token.f_num = str_to_float(temp_token.id,total_consumed_characters);
	temp_token.i_num = temp_token.f_num;
	return temp_token;
    
    }
    return temp_token;
}
