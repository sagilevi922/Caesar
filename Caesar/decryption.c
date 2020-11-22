// IO_Thread.c

// Includes --------------------------------------------------------------------

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "decryption.h"

// Constants -------------------------------------------------------------------

#define MAX_STRING 50
static const char P_EXIT_CMD[] = "exit";

// Function Declarations -------------------------------------------------------


// Function Definitions --------------------------------------------------------




// Gets a char: 'curr_char' and a key: 'decr_key' and returns deycrpited char if the char is letter or digit
char decrepted_char(char curr_char, int decr_key)
{
	char new_char = 'a';
	int num_expression = curr_char - decr_key;

	if (curr_char >= 'a' && curr_char <= 'z')
	{
		num_expression = (num_expression - 'a');
		if (num_expression < 0)
			num_expression = cyclic(num_expression, 26);
		new_char = 'a' + num_expression % 26;
	}

	else if (curr_char >= 'A' && curr_char <= 'Z')
	{
		num_expression = (num_expression - 'A');
		if (num_expression < 0)
			num_expression = cyclic(num_expression, 26);
		new_char = 'A' + num_expression % 26;
	}
	else if (curr_char >= '0' && curr_char <= '9')
	{
		num_expression = (num_expression - '0');
		if (num_expression < 0)
			num_expression = cyclic(num_expression, 10);
		new_char = '0' + num_expression % 10;
	}
	else
		new_char = curr_char;

	return new_char;
}

// Gets a number to round: 'to_round' and a max range val: 'top' and returns a moudlu number
int cyclic(int to_round, int top)
{
	return top + to_round;
}



DWORD WINAPI decrypt_file(LPVOID lpParam)
{
	thread_args* temp_arg = (thread_args*)lpParam;
	printf("key: %d\n", temp_arg->key);
	printf("start_pos: %d\n", temp_arg->start_pos);
	printf("end_pos: %d\n", temp_arg->end_pos);
	printf("output_file: %s\n", temp_arg->output_file);
	printf("input_file: %s\n", temp_arg->input_file);


	return 0;
}