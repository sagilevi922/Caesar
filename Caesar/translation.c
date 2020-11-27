// translation.c
/*
Authors – Matan Achiel - 205642119, Sagi Levi - 205663545
Project – Ex2 - translation.
Description – This program is open by threads from the main program - main.c
gets an struct pointer to an arguments from the main
program and then encrypt/dycrypt depends on a global variable - action_mode
a specific part of the input text file and writes it to an output textfile.
*/
// Includes --------------------------------------------------------------------

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "translation.h"
#include "files_and_threads_handler.h"



// Function Definitions --------------------------------------------------------

char translate_char(char curr_char, int decr_key)
{
	char new_char = 'a';
	int num_expression;
	extern char action_mode;
	if (action_mode == 'd')
		num_expression = curr_char - decr_key;

	else
		num_expression = curr_char + decr_key;

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

int cyclic(int to_round, int top)
{
	return top + to_round;
}

void translate_and_write(char* file_txt_str, int key, int enc_str_size, HANDLE oFile, int start_pos)
{
	for (int i = 0; i < enc_str_size; i++)
	{
		file_txt_str[i] = translate_char(file_txt_str[i], key);
	}
	DWORD  dwBytesWritten = 0;
	DWORD  file_ptr;

	file_ptr = SetFilePointer(
		oFile,
		start_pos, //number of chars
		NULL, //no need of 32 high bits
		FILE_BEGIN //starting point- begin of file
	);


	if (FALSE == WriteFile(oFile, file_txt_str, enc_str_size, &dwBytesWritten, NULL))
	{
		printf("Terminal failure: Unable to write to file.\n GetLastError=%08x\n", GetLastError());
		close_handles_proper(oFile);
		return;
	}
	printf("\noutput:\n%s\n\n", file_txt_str);
	printf("\dwBytesWritten:\n%d\n\n", dwBytesWritten);


}

DWORD WINAPI translate_file(LPVOID lpParam)
{
	HANDLE hFile;
	HANDLE oFile;
	DWORD file_ptr;
	extern char action_mode;
	thread_args* temp_arg = (thread_args*)lpParam;
	char* input_txt = NULL;
	char* output_file_path = NULL;


	printf("\n\ncreated thread\n\n");
	printf("%c action", action_mode);
	printf("key: %d\n", temp_arg->key);
	printf("start_pos: %d\n", temp_arg->start_pos);
	printf("end_pos: %d\n", temp_arg->end_pos);
	printf("output_file: %s\n", temp_arg->output_file);
	printf("input_file: %s\n", temp_arg->input_file);

	if (temp_arg->start_pos == temp_arg->end_pos)
	{
		return 0;
	}

	hFile = get_input_file_handle(temp_arg->input_file);
	if (hFile == NULL)
	{
		printf("Program failed to proccess input file name. exit program");
		return 1;//no any initialized handles yet
	}
	file_ptr = SetFilePointer(
		hFile,
		temp_arg->start_pos, //number of chars
		NULL, //no need of 32 high bits
		FILE_BEGIN //starting point- begin of file
	);

	if (file_ptr == INVALID_SET_FILE_POINTER) // Test for failure
	{
		printf("Can't set file pointer of outputfile. exit\n");
		if (close_handles_proper(hFile) != 1)
			return 1;
	}

	int input_size = temp_arg->end_pos - temp_arg->start_pos;
	input_txt = txt_file_to_str(hFile, temp_arg->start_pos, input_size);
	output_file_path = temp_arg->output_file;
	if (input_txt != NULL)
	{
		oFile = create_file_for_write(output_file_path);
		translate_and_write(input_txt, temp_arg->key, input_size, oFile, temp_arg->start_pos);
		free(input_txt);
		if (close_handles_proper(oFile) != 1)
			return 1;
	}

	if (close_handles_proper(hFile)!= 1)
		return 1;

	return 0;
}