// decryption.h

//#ifndef __IO_THREAD_H__
#define __IO_THREAD_H__
#define OUTPUT_FILE_NAME "decrypted.txt"
#define OUTPUT_FILE_NAME_SIZE 13
#define ERROR_CODE ((int)(-1))

// Includes --------------------------------------------------------------------

#include <windows.h>

// Function Declarations -------------------------------------------------------


typedef struct thread_arguments {
	char* input_file;
	char output_file[OUTPUT_FILE_NAME_SIZE];
	int start_pos;
	int end_pos;
	int key;
} thread_args;


char decrepted_char(char curr_char, int decr_key);
int cyclic(int to_round, int top);
DWORD WINAPI decrypt_file(LPVOID lpParam);


char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size);
HANDLE get_input_file_handle(char* input_file_name);
//#endif // __IO_THREAD_H__