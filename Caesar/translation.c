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

// Constants -------------------------------------------------------------------


// Function Declarations -------------------------------------------------------


// Function Definitions --------------------------------------------------------


HANDLE get_input_file_handle(char* input_file_name)
{
	if(strstr(input_file_name, ".txt")==NULL)
	{
		//printf("Invalid input file name");
		return NULL;
	}
	HANDLE hFile;
	hFile = CreateFileA(input_file_name,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(("Terminal failure: unable to open file \"%s\" for read.\n"), input_file_name);
		return NULL;
	}
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		printf("Invalid input file name. no such file existing. Finish program");
		return NULL;
	}
	return hFile;
}
//Gets the full path of the input file: input_path, and it's length: input_file_len
//defines the output file name and path to create, and returns it
char* init_output_file_name(char* input_path, int input_file_len)
{
	char* output_file_name = NULL;
	int i = 0, last_backslash_pos = 0;
	
	while (input_path[i] != '\0')
	{
		if (input_path[i] == '\\')
			last_backslash_pos = i;
		i++;
	}
	output_file_name = (char*)malloc((last_backslash_pos+2+OUTPUT_FILE_NAME_SIZE) * sizeof(char));
	if (NULL == output_file_name)
	{
		printf("Failed to allocate memory.\n");
		return NULL;
	}
	for (i = 0; i < last_backslash_pos; i++)
	{
		*(output_file_name + i) = input_path[i];
	}
	*(output_file_name + i) = '\0';
	extern char action_mode;
	if (action_mode == 'd')
		strcat(output_file_name, OUTPUT_FILE_NAME_DEC);
	else
		strcat(output_file_name, OUTPUT_FILE_NAME_ENC);
	printf("output_file_name is: %s", output_file_name);
	return output_file_name;
}
HANDLE create_file_for_write(char* output_file_name)
{
	DWORD file_ptr;
	HANDLE hFile;
	extern char action_mode;
	hFile = CreateFileA(output_file_name,               // file to open
		GENERIC_WRITE,          // open for reading
		FILE_SHARE_WRITE,       // share for reading
		NULL,                  // default security
		OPEN_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(("Terminal failure: unable to open the output file for write.\n"));
		return NULL;
	}

	return hFile;
}

int close_handles_proper(HANDLE file_handle)
{
	//arguments check - exrported function
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		printf("Invalid HANDLE value, can't close this HANDLE.\n");
	}
	int ret_val = 0;
	ret_val = CloseHandle(file_handle);
	if (false == ret_val)
	{
		printf("Error when closing\n");
		return ERROR_CODE;
	}
	return 1;

}

char* txt_file_to_str(HANDLE hFile,int start_pos, int input_size)
{
	
	//arguments check - exrported function
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	if (start_pos < 0 || input_size < 0)
	{
		printf("invalid starting position for current thread");

		close_handles_proper(hFile);
		return NULL;
	}
	DWORD  dwBytesRead = 0;

	char* input_txt = NULL;
	input_txt = (char*)malloc((input_size+1) * sizeof(char));
	if (NULL == input_txt)
	{
		printf("Failed to allocate memory.\n");
		return NULL;
	}

	if (FALSE == ReadFile(hFile, input_txt, input_size, &dwBytesRead, NULL))
	{
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		close_handles_proper(hFile);
		free(input_txt);
		return NULL;
	}

	if (dwBytesRead > 0 && dwBytesRead <= input_size)
	{
		input_txt[dwBytesRead] = '\0'; // NULL character
		//printf("succsfull read %d bytes: \n", dwBytesRead);
		//printf("\ninput txt:\n%s\n\n", input_txt);
	}
	else if (dwBytesRead == 0)
	{
		printf("No data read from file\n");
	}

	return input_txt;
}

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

void decrypt_and_write(char* enc_str, int key, int enc_str_size, HANDLE oFile, int start_pos)
{
	for (int i = 0; i < enc_str_size; i++)
	{
		enc_str[i] = translate_char(enc_str[i], key);
	}
	DWORD  dwBytesWritten = 0;
	DWORD  file_ptr;

	file_ptr = SetFilePointer(
		oFile,
		start_pos, //number of chars
		NULL, //no need of 32 high bits
		FILE_BEGIN //starting point- begin of file
	);


	if (FALSE == WriteFile(oFile, enc_str, enc_str_size, &dwBytesWritten, NULL))
	{
		printf("Terminal failure: Unable to write to file.\n GetLastError=%08x\n", GetLastError());
		close_handles_proper(oFile);
		return;
	}
	printf("\noutput:\n%s\n\n", enc_str);
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
		decrypt_and_write(input_txt, temp_arg->key, input_size, oFile, temp_arg->start_pos);
		free(input_txt);
		if (close_handles_proper(oFile) != 1)
			return 1;
	}

	if (close_handles_proper(hFile)!= 1)
		return 1;

	return 0;
}