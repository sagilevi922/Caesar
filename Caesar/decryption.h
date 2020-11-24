// decryption.h

//#ifndef __IO_THREAD_H__


// Includes --------------------------------------------------------------------

#include <windows.h>
#include "HardCodedData.h"

// Function Declarations -------------------------------------------------------


typedef struct thread_arguments {
	char* input_file;
	char output_file[OUTPUT_FILE_NAME_SIZE];
	int start_pos;
	int end_pos;
	int key;
} thread_args;


char translate_char(char curr_char, int decr_key);
int cyclic(int to_round, int top);
int close_handles_proper(HANDLE file_handle);
char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size);
HANDLE get_input_file_handle(char* input_file_name);
HANDLE create_file_for_write();
DWORD WINAPI translate_file(LPVOID lpParam);

//#endif // __IO_THREAD_H__