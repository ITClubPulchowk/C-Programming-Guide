// This program file is dedicated to perform the validity of the digits and issue output warning if any found....

// first define the states to be used by the program
// we are leaving the potential error checking for now .. will revisit later if would like


// define the transition table
// NFA is responsible to extract the digit (not the string only) from the given string .. so we must modify accordingly 
// we will have current-input head and the max input to determine the string to be returned ...

// DFA states info
/*
  q0 -> starting state
  q1 -> integer accepting state
  q2 -> start of floating point (non-accepting)
  q3 -> recoverable state
  q4 -> floating point accepting state
  q6 -> identifier accepting state
  q7 -> Any other invalid id 
  q5 -> double period (.) in numbers 
  q8 -> Operator accepting state 
*/

#include <stdio.h>
#include "regex.h"
#include <assert.h>

// It is the direct naive simulation of the Deterministic finite automation that recognizes valid id, operators, digits (both int and float), and syntax errors


enum finite_states transition_func(enum finite_states current_state, char input_symbol)
{
    if (current_state == q0)
    {
	if (input_symbol == ' ')
	    return q0;
	if (input_symbol == '\n')
	    return null_state;
	if (is_op(input_symbol))
	    return q8;
	if (is_digit(input_symbol))
	    return q1;
	if (is_valid_id(input_symbol)) 
	    return q6;
	else
	    // assert(!"Syntax error");
	    return q7;
    }

    if(current_state == q1)
    {
	if (is_digit(input_symbol))
	    return q1;
	else if (input_symbol == '.')
	    return q2;
	else if (is_op(input_symbol) || is_valid_id(input_symbol) || is_valid_ch(input_symbol))
	    return q3; //  it tells the automaton to stop if further alphabets are read. It is the recoverable state.
    }

    if(current_state == q2)
    {
	if(is_digit(input_symbol))
	    return q4;
	else if ((input_symbol == '.') || (input_symbol == ' ') || (input_symbol == '\n'))
	    return q5; // will be used for error handling later on 
	    // assert(!"Two consecutive periods on the same number not allowed..."); // Irrecoverable error ... should issue warning
	else
	    return q7; // Unrecoverable error without different error messages
    }

    if(current_state == q4)
    {
	if(is_digit(input_symbol))
	    return q4;
	if (input_symbol == '.')
	{
	    return q5; // Let's just return state.. Error logging will be done later... 
	    // assert(!"Two periods on the same number are forbidden."); // Intentional
	}
	return q3;
    }
    

    if (current_state == q6)
    {
	if(is_valid_id(input_symbol) || is_digit(input_symbol))
	    return q6;
	else if (is_op(input_symbol) || is_valid_ch(input_symbol))
	    return q3;
	else 
	    return q7; // Invalid id
    }

    return null_state;
}

int str_len(const char* str)
{
    int counter = 0;
    while(*(str + counter++));
    return counter - 1;
}

int str_cpy(char* dest, const char* src,int len)
{
    for(int i = 0; i < len; ++i)
    {
	*(dest++) = src[i];                 // Error check/handling of any sort is not done 
    }
    return len;
}

// Need to write one helper function .. string to float
float str_to_float(const char* str, int len)
{
    float numeral = 0;
    // first count either to '.' or end of the string
    int dot_len = len;
    for(int i = 0; i < len; ++i)
    {
	if (str[i] == '.')
	{
	    dot_len = i;
	    break;
	}
    }
    int pos = 1;

    float temp = dot_len;
    while(dot_len--)
    {
	numeral += (str[dot_len] - 48) * pos;
	pos*=10;
    }
    
    // Remaining length exact opposite
    pos = 1;
    for(int i = temp+1; i < len; ++i)
    {
	pos *= 10;
	numeral+= (str[i] - 48)/(float) pos;
    }
    return numeral;
}

int str_to_int(const char* str, int len)
{
    int numeral = 0;
    int pos = 1;
    while(len--)
    {
	numeral += (str[len]-48) * pos;
	pos *= 10;
    }
    return numeral;
}
    

bool is_valid_id(char ch)
{
    return ( (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || ch == '_');
}

bool is_digit(char ch)
{
    return ch >= 48 && ch <= 57;
}

bool is_op(char ch)
{
    switch(ch)
    {
    case '+' :
    case '-' :
    case '*' :
    case '/' :
    case '%' :
    case '^' :
    case '=' :
    case '(' :
    case ')' :
	return true;
    }
    return false;
}

bool is_valid_ch(char ch)
{
    return (ch == ' ') || (ch == '\n');
}
