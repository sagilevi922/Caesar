
// Includes --------------------------------------------------------------------

#include <windows.h>
#include "HardCodedData.h"

// Data structure --------------------------------------------------------------


// container of arguments for the thread
typedef struct thread_arguments {
	char* input_file;
	//char output_file[OUTPUT_FILE_NAME_SIZE];
	char* output_file;
	int start_pos;
	int end_pos;
	int key;
} thread_args;


// Function Declarations -------------------------------------------------------


//IO module?
//Gets a str of the input text file content: input_f_str, and the size of the content: file_size.
//returned value: the number of lines in the file.
int get_num_of_lines(char* input_f_str, int file_size);


// Gets number of threads: 'num_of_threads' and number of lines: 'num_of_lines' and returns an array which contains the number of lines for each thread.
int* init_lines_per_thread(int num_of_threads, int num_of_lines);

// free dynamic allocation gets array of all the threads args and its length, and free each one and then free the array itself, 
// also frees the array the keeps each thread start pos.
// frees only in case the ptr is not NULL
void free_memory(char* input_file_name, int* lines_per_thread, thread_args** thread_args_arr, int num_of_threads, char* output_file_name);

// Gets an array with lines for each thread: 'lines_per_thread' and retruns an array with the initial pos for each thread to start reading from the input file
//update after winapi:
void init_start_points(int* lines_per_thread, char* input_f_str, int file_size, int num_of_threads);

// gets the input args from the cmd, and returns true if one of them is not valid. 
int is_not_valid_input_args(int key, int num_of_threads, char* action_input, int num_of_args);
