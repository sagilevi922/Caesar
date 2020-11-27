#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#include "files_and_threads_handler.h"
#include "translation.h"
#include "HardCodedData.h"


// global variable for the thread to know wheater to encrypt or decrypt
char action_mode = 'd';


int get_num_of_lines(char* input_f_str, int file_size)
{
	int num_of_lines = 0;
	char tav;
	for (int i= 0; i<file_size;i++)
	{
		if (input_f_str[i] == '\n')
			num_of_lines++;
	}
	return num_of_lines+1;
}

int* init_lines_per_thread(int num_of_threads, int num_of_lines)
{
	

	int* lines_per_thread = (int*)malloc((num_of_threads) * sizeof(int));

	if (NULL == lines_per_thread)
	{
		printf("memory allocation failed");

		return lines_per_thread;
	}
	int i = 0;

	for (i = 0; i < num_of_threads; i++)
	{
		lines_per_thread[i] = 0;
	}

	i = 0;

	while (num_of_lines > 0)
	{
		if (i > num_of_threads-1)
		{
			i = 0;
		}
		lines_per_thread[i]++;
		num_of_lines--;
		i++;
	}

	for (i = 0; i < num_of_threads; i++)
	{
		printf("lines_per_thread[%d] : %d\n",i, lines_per_thread[i]);

	}

	return lines_per_thread;
}

void free_memory(char* input_file_name, int* lines_per_thread, thread_args** thread_args_arr, int num_of_threads, char* output_file_name)
{
	for (int i = 0; i < num_of_threads; i++)
	{
		if (thread_args_arr[i])
			free(thread_args_arr[i]);
	}

	if (thread_args_arr)
		free(thread_args_arr);

	if (lines_per_thread)
		free(lines_per_thread);

	if (output_file_name)
		free(output_file_name);
}

void init_start_points(int* lines_per_thread, char* input_f_str, int file_size, int num_of_threads)
{
	int pos = 0;
	int num_of_lines = 0;
	char tav;
	int i = 0;
	int num_of_lines_to_seek = lines_per_thread[0]; // intial how many lines to look for
	lines_per_thread[0] = 0; // first line starts at zero

	while (pos < file_size)
	{

		while (num_of_lines < num_of_lines_to_seek && pos < file_size)
		{

			if (input_f_str[pos] == '\n')
				num_of_lines++;
			pos++;
		}

		i++;
		if (i == num_of_threads)
			break;

		num_of_lines_to_seek = lines_per_thread[i];
		num_of_lines = 0;
		lines_per_thread[i] = pos - 1;
	}
	for (i = 0; i < num_of_threads; i++)
	{
		printf("lines_per_thread2 [%d] : %d\n", i, lines_per_thread[i]);

	}
}

int is_not_valid_input_args(int key, int num_of_threads, char* action_input, int num_of_args)
{
	if (num_of_args != 5)
	{
		//Not enough arguments.
		printf("Invalid input, Please provide encrypted file path, enc/dec key, number of threads and action mode('e'/'d')");
		return 1;
	}

	if (key < 0)
	{
		printf("Invalid key, enter only postive number"); //Invalid key.
		return 1;
	}

	if (num_of_threads <= 0)
	{
		printf("Invalid threads number, enter only postive number"); //Invalid key.
		return 1;
	}

	if (strcmp(action_input, "-d") != 0 && strcmp(action_input, "-e") != 0)
	{
		printf("Invalid last argument, for encryption type '-e' for decryption type '-d'"); //invalid dec/enc mode.
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	char* input_txt = NULL;
	char* action_input ="";
	char* input_file_name = NULL;
	char* output_file_name = NULL;
	int* lines_per_thread = NULL;
	int input_file_len = 0, num_of_lines = 0, ret_val = 0, num_of_threads = 0, key = 0;
	thread_args** thread_args_arr = NULL;
	HANDLE hFile;
	DWORD dwFileSize, wait_code;
	HANDLE p_thread_handles[THREADS_LIMIT];
	DWORD p_thread_ids[THREADS_LIMIT];

	key = *argv[2] - '0';
	num_of_threads = *argv[3] - '0';
	action_input = argv[4];
	input_file_len = strlen(argv[1]);


	if (is_not_valid_input_args(key, num_of_threads, action_input, argc))
		return 1;

	input_file_name = argv[1];
	action_mode = action_input[1];

	hFile = get_input_file_handle(input_file_name);
	if (NULL == hFile)
	{
		printf("Error while opening input file. Exit program\n");
		return 1;
	}

	dwFileSize = GetFileSize(hFile, NULL);

	input_txt = txt_file_to_str(hFile, 0, dwFileSize);
	if (NULL == input_txt)
	{
		printf("Error while reading input file. Exit program\n");
		close_handles_proper(hFile);
		return 1;
	}

	num_of_lines = get_num_of_lines(input_txt, dwFileSize);
	
	lines_per_thread = init_lines_per_thread(num_of_threads, num_of_lines);
	if (NULL == lines_per_thread)
	{
		free(input_txt);
		close_handles_proper(hFile);
		return 1;
	}

	init_start_points(lines_per_thread, input_txt, dwFileSize, num_of_threads);
	
	free(input_txt);
	close_handles_proper(hFile);

	output_file_name = init_output_file_name(argv[1], input_file_len);
	if (1 == init_output_file(dwFileSize, num_of_lines, output_file_name))
	{
		free(lines_per_thread);
		free(output_file_name);
		return 1;
	}

	thread_args_arr = init_thread_args(num_of_threads, thread_args_arr, num_of_lines, key, dwFileSize, input_file_name, lines_per_thread, output_file_name);
	if (NULL == thread_args_arr)
	{
		free(lines_per_thread);
		free(output_file_name);
		return 1;
	}

	for (int i = 0; i < num_of_threads; i++)
		p_thread_handles[i] = CreateThreadSimple(translate_file, p_thread_ids[i], thread_args_arr[i]);
	wait_code = WaitForMultipleObjects(num_of_threads, p_thread_handles, TRUE, MAX_WAITING_TIME);

	// gets array of all the threads handles and their amount, and closes the handle for each thread 
	close_threads(p_thread_handles, num_of_threads, wait_code);
	free_memory(input_file_name, lines_per_thread, thread_args_arr, num_of_threads, output_file_name);

	return 0;
}