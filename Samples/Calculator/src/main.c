/* Calculator
Created by    : Tilak Chad (075BCT)
Modified date : 26 Apr 2021
*/ 

#include <stdio.h>
#include "parser.h"
#include "regex.h"
#include "token.h"
#include "io_stream.h"
#include <time.h>

void string_copy(char* str1, const char* str2)  // copy the string without taking length .. may be unsafe but works if string is null terminated and buffer doesn't overflow
{
    int last_index = 0;
    for (int i = 0; i < str_len(str2); ++i)
    {
	str1[i] = str2[i];
	last_index = i;
    }
    str1[last_index+1] = '\0';
}


int handle_return_type (stream* input_stream,return_type value)
{
    switch(value.type)
    {
    case INT_NUM : printf("-> %d\n",value.i_num);
	break;
    case FLOAT_NUM : printf("-> %.6g\n",value.f_num);
	break;
    case ID : printf("-> %s\n",value.id); // I guess it won't trigger anytime soon
	break;
    case ERR_TYPE :
    {
	// Error handling and logging procedure
	// Second line must display the ^^^ symbols where error occured which is captured by the input_stream somewhere
	char error_string[MAX_INPUT_LENGTH+5] = {'\0'};
	char buffer[MAX_INPUT_LENGTH+5];

	
	for (int i = 0; i < MAX_INPUT_LENGTH; ++i)
	    buffer[i] = ' ';

	for (int i = input_stream->log.start_index; i <= input_stream->log.end_index; ++i)
	{
	    buffer[i] = '^';
	    buffer[i+1] = '\0';
	}

	#ifdef _WIN32
	switch(value.error_code)
	{
	case E_LVALUE :
	    // Display the caret signs required by it
	    // make a buffer long enough as input buffer
	    // Display
	
	{
	
       	    string_copy(error_string, "   ERROR : Lvalue expected here .....\n");
	    break;
	}
	
	case E_SYM_ID :
	    string_copy(error_string,"    ERROR : Undefined identifier used here .....\n");
	    break;
	case E_RVALUE :
	    string_copy(error_string,"    ERROR : Expected rvalue here (digit or id) ....\n");
	    break;
	case L_MISS :
	    string_copy(error_string,"    ERROR : Expected left parenthesis here .....\n");
	    break;
	case E_OP :
	    string_copy(error_string,"    ERROR : Expected operand of the operator here .....\n");
	    break;
	case E_OPERATOR :
	    string_copy(error_string,"    ERROR : Expected operator here .... \n");
	    break;
	case U_SYM :
	    return 1;
	case Q7 :
	    string_copy(error_string, "    ERROR : Syntax error .... \n");
	    break;
        case Q2 :
	case Q5 :
	    string_copy(error_string, "    ERROR : Invalid floating point literal ... \n");
	    break;
	case I_MOD :
	    string_copy(error_string, "    ERROR : Mod (prec. remainder) not defined for other types than INT\n");
	    break;
	case E_RPAREN :
	    string_copy(error_string, "    \033[32mERROR : \033[33mUnexpected right parentheses ...\n");
	    break;	    
	default :
	    printf("This seems to be have not handled...");
	}
	printf("          %s\n",buffer);
	printf("%s",error_string);

	#elif __linux__
	switch(value.error_code)
	{
	case E_LVALUE :
	    // Display the caret signs required by it
	    // make a buffer long enough as input buffer
	    // Display
	
	{
	
       	    string_copy(error_string, "    \033[31mERROR : \033[33mLvalue expected here .....\n");
	    break;
	}
	
	case E_SYM_ID :
	    string_copy(error_string, "     \033[31mERROR : \033[33mUndefined identifier used here .....\n");
	    break;
	case E_RVALUE :
	    string_copy(error_string, "    \033[31mERROR : \033[33mExpected rvalue here (digit or id) ....\n");
	    break;
	case L_MISS :
	    string_copy(error_string, "    \033[31mERROR : \033[33mExpected left parentheses here .....\n");
	    break;
	case E_OP :
	    string_copy(error_string, "    \033[31mERROR : \033[33mExpected operand of the operator here .....\n");
	    break;
	case E_OPERATOR :
	    string_copy(error_string, "    \033[31mERROR : \033[33mExpected operator here .... \n");
	    break;
	case U_SYM :
	    return 1;
	case Q7 :
	    string_copy(error_string, "    \033[31mERROR : \033[33mSyntax error .... \n");
	    break;
        case Q2 :
	case Q5 :
	    string_copy(error_string, "    \033[31mERROR : \033[33mInvalid floating point literal ... \n");
	    break;
	case I_MOD :
	    string_copy(error_string, "    \033[31mERROR : \033[33mMod (prec. remainder) not defined for other types than INT\n");
	    break;
	case E_RPAREN :
	    string_copy(error_string, "    \033[31mERROR : \033[33mUnexpected right parentheses ...\n");
	    break;
	default :
	    printf("This seems to be have not handled...");
	}
	
	printf("\033[34m     %s\n",buffer);
	printf("%s\033[0m",error_string);
	#endif
    }
    
    }
    return 0;
}

void delay(float delay)
{
    clock_t now,then;
    now = clock();

    while(1)
    {
	then = clock();
	if ((double)(then-now)/CLOCKS_PER_SEC > delay)
	    return; // converted into seconds
    }
}
void low_quality_startup()
{
 
    /* char text[1000] = */
    /* { */
    /* 	"        " */
    /* 	" ((.))          \n" */
    /* 	"  \\|/           \n" */
    /* 	"  \\|/    \\_____/\n" */
    /* 	"  \\|/     \\\v// \n" */
    /* 	"  \\|/      |||  \n" */
    /* 	"  \\|/      |||  \n" */
    /* 	"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n" */
    /* }; */
    
    // setbuf(stdout,NULL); // Lets not make the output unbuffered.. It would be overkill
    char text[] = 
	"\nHere's a flower (handdrawn) for you .. \n"
        "Not good, huh ?  ..  Fine, I will draw a dragon next time ..\n"
        "\t\t\t\t\t\t - Tilak Chad (075)\n";

    for (int i = 0; i < str_len(text); ++i)
    {
	putc(text[i],stdout);
	fflush(stdout);
	
	// for(int i = 0; i < 100000; ++i); // low quality delay
	
	delay(0.01f);
	    
    }
	// might make some fancy text display using geometric transformation but later
    char flower[2000] =
	"\t\t                  . - .' '.-         \n "
	"\t\t             , -('           )-.     \n "
	"\t\t          /        \\  /       \\   \n "
	"\t\t           \\                    /    \n "
	"\t\t          (  ---    O O O  ---   )   \n "
	"\t\t  /`\\      /        O O O        \\  \n "
	"\t\t  \\   ;/|  \\        O O O  ---    /  \n "
	"\t\t  ;.    \\  \\ ---               .-   \n "
	"\t\t _; '.  :/|  '-(    /  \\       )    \n "
	"\t\t  |   '.  /   //'-'.          ;      \n "
	"\t\t  '--;  ' ;. //      -----.'         \n "
	"\t\t    '-,_ ';//                       \n "
	"\t\t          '((           /\\          \n "
	"\t\t            \\\\      /|/ .  |         \n "
	"\t\t            \\\\     \\/..  /         \n "
	"\t\t             \\\\, ./' .'  ; -        \n "
	"\t\t               ))'_, -'/..           \n "
	"\t\t              //-'                   \n "
	"\t\t             //                      \n " 
	"\t\t            //                       \n "
	" \t\t           |/                        \n "
	"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n" ;
    // What else is there a better way to pass time .. (>.<)
    putchar('\n');
    for (int i = 0; i < str_len(flower); ++i)
    {
	putc(flower[i],stdout);
	fflush(stdout);

	delay(0.005f);
    }
    
    putchar('\n');
    printf("\nType exit to quit ;");
    printf("\nCalculator :-> \n\n");
}


int main()
{
    
    stream input_stream;

    // Initialize the symbol table here
    input_stream.table = init_symbol_table();
    
    // Now need to implement a error system that gives meaningful error ... not some gibberish
    // so the usual way is to use return codes for error handling .. 

    // need a starting logo
    low_quality_startup();
    
    token lookahead;
    return_type value;
    while(1)
    {
	printf(" :-> ");
	fgets(input_stream.buffer, 50, stdin);
	input_stream.total_len = str_len(input_stream.buffer);
	input_stream.cur_pos = 0;
	input_stream.log.start_index = 0;
	input_stream.log.end_index = 0;
	lookahead = get_next_token(&input_stream);
	
	return_type value;
	
	if (lookahead.type == END_M); // Do nothing looool..
	else
	{
	    value = calc_run(&input_stream, &lookahead);
	    
	    if(handle_return_type(&input_stream, value)==1)
	    {
		clean_up(&input_stream.table);
		return 0;
	    }
	}
    }
    return 0;                                                         
}
