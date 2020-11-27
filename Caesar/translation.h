// decryption.h

//#ifndef __IO_THREAD_H__


// Includes --------------------------------------------------------------------

#include <windows.h>
#include "HardCodedData.h"

// Function Declarations -------------------------------------------------------

// Gets a char: 'curr_char' and a key: 'decr_key' and returns deycrpited/encryped char if the char is letter or digit
// and according to global variable 'action_mode'
char translate_char(char curr_char, int decr_key);

// Gets a number to round: 'to_round' and a max range val: 'top' and returns a moudlu number
int cyclic(int to_round, int top);


// translate the str:'enc_str' char by char according to action mode, and then write the str to
// the output file.
void translate_and_write(char* file_txt_str, int key, int enc_str_size, HANDLE oFile, int start_pos);

// The start function of the translation thread.
// gets a pointer to struct 'thread_arguments' which contains an input fille name to
// read text from, output file name to write the translted text to, gets a start and end position
// to know from where position to take the input text, and a key for encrypt/dycrpt.
DWORD WINAPI translate_file(LPVOID lpParam);

//#endif // __IO_THREAD_H__