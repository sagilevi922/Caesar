#pragma once
// Includes --------------------------------------------------------------------


#include <windows.h>
#include "HardCodedData.h"
#include "main.h"

// Function Declarations -------------------------------------------------------

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

int close_handles_proper(HANDLE file_handle);

char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size);

//Gets the entrance function of the new thread: p_start_routine, it's id: p_thread_id,
//and as struct that contain its arguments: paths of input and output files, a key for operation,
//starting position of operation and the length of the section to operate.
//It creates a new thread and return a HANDLE to it.
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, thread_args* args);

//creates a output file according to the arguments: size: file_size, and number of lines: num_of_lines
//and according to the right mode: encryption or decryption.
//the function returns 0 for success or 1 else.
int init_output_file(int file_size, int num_of_lines, char* output_file_name);

// gets array of all the threads handles and their amount, and closes the handle for each thread 
void close_threads(HANDLE p_thread_handles[], int num_of_threads, DWORD wait_code);

// gets number of thread and a pointer to array of thread args, and allocats dynamic memory for it. 
thread_args** init_thread_args(int num_of_threads, thread_args** thread_args_arr, int num_of_lines, int key, DWORD dwFileSize, char* input_file_name, int* lines_per_thread, char* output_file_name);

// gets paremters for thread and create a thread argument struct pointer
thread_args* create_thread_arg(int key, int start_pos, int end_pos, char* input_file_name, char* output_file_name);
