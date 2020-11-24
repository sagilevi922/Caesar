// decryption.h

//#ifndef __IO_THREAD_H__


// Includes --------------------------------------------------------------------

#include <windows.h>
#include "HardCodedData.h"

// Data structure --------------------------------------------------------------

// container of arguments for the thread
typedef struct thread_arguments {
	char* input_file;
	char output_file[OUTPUT_FILE_NAME_SIZE];
	int start_pos;
	int end_pos;
	int key;
} thread_args;

// Function Declarations -------------------------------------------------------

// Gets a char: 'curr_char' and a key: 'decr_key' and returns deycrpited/encryped char if the char is letter or digit
// and according to global variable 'action_mode'
char translate_char(char curr_char, int decr_key);

// Gets a number to round: 'to_round' and a max range val: 'top' and returns a moudlu number
int cyclic(int to_round, int top);

// gets a HANDLE and close it properly, return 1 in case of sucssful closing, else return errorcode
int close_handles_proper(HANDLE file_handle);

// Gets a handle for the input file, and a start position to where
// it should start reading, and how many byts to read
// and copy it to a dynamic string.
char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size);

// gets a string of the input file name, creates a file with thid name,
// and then return a handle to this file
HANDLE get_input_file_handle(char* input_file_name);

// create and returns handle for the output text file.
HANDLE create_file_for_write();

// The start function of the translation thread.
// gets a pointer to struct 'thread_arguments' which contains an input fille name to
// read text from, output file name to write the translted text to, gets a start and end position
// to know from where position to take the input text, and a key for encrypt/dycrpt.
DWORD WINAPI translate_file(LPVOID lpParam);

//#endif // __IO_THREAD_H__