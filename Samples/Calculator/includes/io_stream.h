#ifndef IO_STREAM_H
#define IO_STREAM_H

#include "symbol_table.h"

#define MAX_INPUT_LENGTH 150

// Just its name is stream.. It does every other thing

struct error_log
{
    int start_index;
    int end_index;
};

struct stream
{
    char buffer[MAX_INPUT_LENGTH];
    int total_len;
    int cur_pos;
    struct symbol_table table;
    int prev_index; // Used for error handling

    struct error_log log;
};

typedef struct stream stream;
// struct stream in_stream;
// struct stream out_stream;

#endif
