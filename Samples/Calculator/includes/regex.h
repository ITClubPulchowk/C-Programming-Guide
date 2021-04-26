#ifndef REGEX_H
#define REGEX_H
#include <stdbool.h>

enum finite_states {q0,q1,q2,q3,q4,q5,q6,q7,q8,null_state};

enum finite_states transition_func(enum finite_states, char );

float simulate_dfa(const char* input_string,char* , int);

// Return the length of the supplied null terminated string
int str_len(const char* );

// Return the number of bytes transfered from src to destination string
int str_cpy(char* dest, const char* src, int len);

float str_to_float(const char* string, int len);
int str_to_int(const char* string, int len);

bool is_valid_id(char ch);

// check whether given char is digit or not
bool is_digit(char ch);

// check whether given char is operator or not
bool is_op(char ch);
bool is_valid_ch(char ch);

#endif
