// decryption.h

//#ifndef __IO_THREAD_H__


// Includes --------------------------------------------------------------------

#include <windows.h>
#include "HardCodedData.h"

// Data structure --------------------------------------------------------------


// Function Declarations -------------------------------------------------------

// Gets a char: 'curr_char' and a key: 'decr_key' and returns deycrpited/encryped char if the char is letter or digit
// and according to global variable 'action_mode'
char translate_char(char curr_char, int decr_key);

// Gets a number to round: 'to_round' and a max range val: 'top' and returns a moudlu number
int cyclic(int to_round, int top);

//Gets a section to translate from the input file: enc_str, it's lentgh:enc_str_size , a HANDLE, to the output file: oFile,
//and the position at the output file from which to start to write: start_pos
//Finally, it writes the translated section to the output file
void decrypt_and_write(char* enc_str, int key, int enc_str_size, HANDLE oFile, int start_pos);

// gets a HANDLE and close it properly, return 1 in case of sucssful closing, else return errorcode
int close_handles_proper(HANDLE file_handle);

// Gets a handle for the input file, and a start position to where
// it should start reading, and how many byts to read
// and copy it to a dynamic string.
char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size);

// gets a string of the input file name, open a file with this name if exist,
// and then return a handle to this file
HANDLE get_input_file_handle(char* input_file_name);

//Gets the full path of the input file: input_path, and it's length: input_file_len
//defines the output file name and path to create, and returns it
char* init_output_file_name(char* input_path, int input_file_len);

// create and returns handle for the output text file.
HANDLE create_file_for_write(char* output_file_name);

// The start function of the translation thread.
// gets a pointer to struct 'thread_arguments' which contains an input fille name to
// read text from, output file name to write the translted text to, gets a start and end position
// to know from where position to take the input text, and a key for encrypt/dycrpt.
DWORD WINAPI translate_file(LPVOID lpParam);

//#endif // __IO_THREAD_H__