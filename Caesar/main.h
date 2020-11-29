#include <stdio.h>
#include <windows.h>
#include "translation.h"
#include "HardCodedData.h"
#include <stdbool.h>

// container of arguments for the thread
typedef struct thread_arguments {
	char* input_file;
	//char output_file[OUTPUT_FILE_NAME_SIZE];
	char* output_file;
	int start_pos;
	int end_pos;
	int key;
	HANDLE semaphore_gun;
} thread_args;

//Gets a str of the input text file content: input_f_str, and the size of the content: file_size.
//returned value: the number of lines in the file.
int get_num_of_lines(char* input_f_str, int file_size);

// Gets number of threads: 'num_of_threads' and number of lines: 'num_of_lines' and returns an array which contains the number of lines for each thread.
int* init_lines_per_thread(int num_of_threads, int num_of_lines);

// Gets an array with lines for each thread: 'lines_per_thread' and retruns an array with the initial pos
//for each thread to start reading from the input file
void init_start_points(int* lines_per_thread, char* input_f_str, int file_size, int num_of_threads);

//Gets the entrance function of the new thread: p_start_routine, it's id: p_thread_id,
//and as struct that contain its arguments: paths of input and output files, a key for operation,
//starting position of operation and the length of the section to operate.
//It creates a new thread and return a HANDLE to it.
static HANDLE create_thread_simple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, thread_args* args);

// gets paremters for thread and creates a thread argument struct pointer
thread_args* create_thread_arg(int key, int start_pos, int end_pos, char* input_file_name, char* output_file_name, HANDLE semaphore_gun);

//creates a output file according to the arguments: size: file_size, and number of lines: num_of_lines
//and according to the right mode: encryption or decryption.
//the function returns 0 for success or 1 else.
int init_output_file(int file_size, int num_of_lines, char* output_file_name);

// free dynamic allocation gets array of all the threads args and its length, and free each one and then free the array itself, 
// also frees the array the keeps each thread start pos.
// frees only in case the ptr is not NULL
void free_memory(char* input_file_name, int* lines_per_thread, thread_args** thread_args_arr, int num_of_threads, char* output_file_name);

// gets array of all the threads handles and their amount, and closes the handle for each thread 
void close_threads(HANDLE p_thread_handles[], int num_of_threads, DWORD wait_code, HANDLE semaphore_gun);

// gets number of thread and a pointer to array of thread args, and allocats dynamic memory for it. 
thread_args** init_thread_args(int num_of_threads, thread_args** thread_args_arr, int num_of_lines, int key, DWORD dwFileSize, char* input_file_name, int* lines_per_thread, char* output_file_name, HANDLE semaphore_gun);

//Gets the arguments from the command line: input_args[], their amount: num_of_args, check if they are valid,
//and initiate the following empty arguments: num_of_threads - number of threads to create
//, input_file_name - full path of the input file, and input_file_len - the length of the input file name
//In the end it returns 1 if at least one of them is not valid,or  0 else
int init_input_vars(char* input_args[], int num_of_args, int* key, int* num_of_threads, char** input_file_name, int* input_file_len);

