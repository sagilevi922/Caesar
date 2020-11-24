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


HANDLE get_input_file_handle(char* input_file_name)
{
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
	return hFile;
}

HANDLE create_file_for_write()
{
	DWORD file_ptr;
	HANDLE hFile;
	hFile = CreateFileA(OUTPUT_FILE_NAME,               // file to open
		GENERIC_WRITE,          // open for reading
		FILE_SHARE_WRITE,       // share for reading
		NULL,                  // default security
		OPEN_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(("Terminal failure: unable to open file \"%s\" for write.\n"), OUTPUT_FILE_NAME);
		return NULL;
	}

	return hFile;
}

char* txt_file_to_str(HANDLE hFile,int start_pos, int input_size)
{
	DWORD  dwBytesRead = 0;
	char* input_txt = (char*)malloc((input_size+1) * sizeof(char));

	if (NULL == input_txt)
	{
		printf("Failed to allocate memory. exit\n");
		// TODO free exit properly
	}

	if (FALSE == ReadFile(hFile, input_txt, input_size, &dwBytesRead, NULL))
	{
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(hFile);
		return;
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

void decrypt_and_write(char* enc_str, int key, int enc_str_size, HANDLE oFile, int start_pos)
{
	for (int i = 0; i < enc_str_size; i++)
	{
		enc_str[i] = decrepted_char(enc_str[i], key);
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
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(oFile);
		return;
	}
	printf("\noutput:\n%s\n\n", enc_str);
	printf("\dwBytesWritten:\n%d\n\n", dwBytesWritten);


}


DWORD WINAPI decrypt_file(LPVOID lpParam)
{
	HANDLE hFile;
	HANDLE oFile;
	DWORD file_ptr;
	printf("\n\ncreated thread\n\n");

	thread_args* temp_arg = (thread_args*)lpParam;
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

	file_ptr = SetFilePointer(
		hFile,
		temp_arg->start_pos, //number of chars
		NULL, //no need of 32 high bits
		FILE_BEGIN //starting point- begin of file
	);

	if (file_ptr == INVALID_SET_FILE_POINTER) // Test for failure
	{
		printf("Can't set file pointer of outputfile. exit\n");
		//need to close hfile!!
		exit(1);
	}
	char* input_txt = NULL;
	int input_size = temp_arg->end_pos - temp_arg->start_pos;
	input_txt = txt_file_to_str(hFile, temp_arg->start_pos, input_size);
	//printf("lines to decrypt:\n%s", input_txt);


	oFile = create_file_for_write();
	decrypt_and_write(input_txt, temp_arg->key, input_size, oFile, temp_arg->start_pos);
	
	free(input_txt);

	int ret_val = 0;
	ret_val = CloseHandle(oFile);
	if (false == ret_val)
	{
		printf("Error when closing\n");
		return ERROR_CODE;
	}

	ret_val = CloseHandle(hFile);
	if (false == ret_val)
	{
		printf("Error when closing\n");
		return ERROR_CODE;
	}
	
	return 0;
}