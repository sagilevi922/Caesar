#include <stdio.h>
#include <windows.h>
#include "decryption.h"
#include <stdbool.h>

#define THREADS_LIMIT 64
#define BRUTAL_TERMINATION_CODE 0x55
#define MAX_WAITING_TIME 10000

//
//typedef struct thread_arguments {
//	char* input_file;
//	char output_file[OUTPUT_FILE_NAME_SIZE];
//	int start_pos;
//	int end_pos;
//	int key;
//} thread_args;


// global variable for the thread to know wheater to encrypt or decrypt
char action_mode = 'd';



//IO
int get_num_of_lines(char* input_f_str, int file_size)
{
	// TODO error handle when num of threads is bigger than num of lines
	int num_of_lines = 0;
	char tav;
	for (int i= 0; i<file_size;i++)
	{
		if (input_f_str[i] == '\n')
			num_of_lines++;
	}
	return num_of_lines+1;
}

//static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
//	LPDWORD p_thread_id)
//{
//	/* Should check for NULL pointers. Skipped for the sake of simplicity. */
//
//	return CreateThread(
//		NULL,            /*  default security attributes */
//		0,               /*  use default stack size */
//		p_start_routine, /*  thread function */
//		NULL,            /*  argument to thread function */
//		0,               /*  use default creation flags */
//		p_thread_id);    /*  returns the thread identifier */
//}

// Gets number of threads: 'num_of_threads' and number of lines: 'num_of_lines' and returns an array which contains the number of lines for each thread.
int* init_lines_per_thread(int num_of_threads, int num_of_lines)
{
	

	int* lines_per_thread = (int*)malloc((num_of_threads) * sizeof(int));

	if (NULL == lines_per_thread)
	{
		printf("memory allocation failed");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
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

// Gets an array with lines for each thread: 'lines_per_thread' and retruns an array with the initial pos for each thread to start reading from the input file
//update after winapi:
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

		while (num_of_lines < num_of_lines_to_seek && pos<file_size)
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
		lines_per_thread[i] = pos-1;
	}
	for (i = 0; i < num_of_threads; i++)
	{
		printf("lines_per_thread2 [%d] : %d\n", i, lines_per_thread[i]);

	}
}


static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, thread_args* args)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		args,            /*  argument to thread function */
		0,               /*  use default creation flags */
		&p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_CODE);
	}

	return thread_handle;
}

// gets paremters for thread and create a thread argument struct pointer, 
thread_args* create_thread_arg(int key, int start_pos, int end_pos, char* input_file_name)
{
	thread_args* temp_arg = (thread_args*)malloc(sizeof(thread_args));
	if (NULL == temp_arg)
	{
		printf("memory allocation failed");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
	}
	temp_arg->key = key;
	temp_arg->start_pos = start_pos;
	temp_arg->end_pos = end_pos;
	if (action_mode =='d')
		strcpy(temp_arg->output_file, OUTPUT_FILE_NAME_DEC);
	else
		strcpy(temp_arg->output_file, OUTPUT_FILE_NAME_ENC);

	temp_arg->input_file = input_file_name;
	
	return temp_arg;
}

void init_p_thread_handles(int num_of_threads, HANDLE* p_thread_handles, DWORD* p_thread_ids)
{
	p_thread_handles = (HANDLE*)malloc(num_of_threads*sizeof(HANDLE));
	if (NULL == p_thread_handles)
	{
		printf("memory allocation failed");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
	}

	p_thread_ids = (DWORD*)malloc(num_of_threads * sizeof(DWORD));
	if (NULL == p_thread_ids)
	{
		printf("memory allocation failed");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
	}
}

void init_output_file(int file_size, int num_of_lines)
{
	HANDLE hfile;
	DWORD end_file_ptr;

	if (action_mode == 'd')
	{
		hfile = CreateFileA(
			OUTPUT_FILE_NAME_DEC,
			GENERIC_WRITE, //Open file with write mode
			FILE_SHARE_WRITE, //the file should be shared by the threads.
			NULL, //default security mode
			CREATE_ALWAYS, //first time we open the file.
			FILE_ATTRIBUTE_NORMAL, //normal attribute
			NULL //not relevant for open file operations.
		);
	}
	else
	{
		hfile = CreateFileA(
			OUTPUT_FILE_NAME_ENC,
			GENERIC_WRITE, //Open file with write mode
			FILE_SHARE_WRITE, //the file should be shared by the threads.
			NULL, //default security mode
			CREATE_ALWAYS, //first time we open the file.
			FILE_ATTRIBUTE_NORMAL, //normal attribute
			NULL //not relevant for open file operations.
		);
	}



	if (hfile == INVALID_HANDLE_VALUE)
	{
		printf("Can't open the outputfile. exit\n");
		exit(1);
	}

	end_file_ptr = SetFilePointer(
		hfile,
		file_size, //number of chars
		NULL, //no need of 32 high bits
		FILE_BEGIN //starting point- begin of file
	);

	if (end_file_ptr == INVALID_SET_FILE_POINTER) // Test for failure
	{
		printf("Can't set file pointer of outputfile. exit\n");
		//need to close hfile!!!!!!!
		exit(1);
	}

	int set_end_of_file = SetEndOfFile(hfile);
	if (!set_end_of_file) {
		printf("Can't set file pointer of outputfile. exit\n");
		//need to close hfile!!!
		exit(1);
	}

	int ret_val = CloseHandle(hfile);
	if (!ret_val) {
		printf("Can't close output file\n");
		exit(1);
	}
}

// free dynamic allocation gets array of all the threads args and its length, and free each one and then free the array itself, 
// also frees the array the keeps each thread start pos, and frees the input file name string.
void free_memory(char* input_file_name, int* lines_per_thread, thread_args** thread_args_arr,int num_of_threads)
{
	for (int i = 0; i < num_of_threads; i++)
	{
		free(thread_args_arr[i]);
	}
	free(thread_args_arr);
	free(input_file_name);
	free(lines_per_thread);

}

// gets array of all the threads handles and their amount, and closes the handle for each thread 
void close_threads(HANDLE p_thread_handles[], int num_of_threads, DWORD wait_code)
{
	int ret_val = 0;
	// Checking wait code

	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting");
		return ERROR_CODE;
	}
	// Terminate still running Thread

	for (int i = 0; i < num_of_threads; i++)
	{
		// TODO correct terminate argument
		ret_val = TerminateThread(p_thread_handles[i], BRUTAL_TERMINATION_CODE);
		if (false == ret_val)
		{
			printf("Error when terminating\n");
			return ERROR_CODE;
		}
	}
	// Close thread handles
	for (int i = 0; i < num_of_threads; i++)
	{
		ret_val = CloseHandle(p_thread_handles[i]);
		if (false == ret_val)
		{
			printf("Error when closing\n");
			return ERROR_CODE;
		}
	}
}

// gets number of thread and a pointer to array of thread args, and allocats dynamic memory for it. 

thread_args** init_thread_args(int num_of_threads, thread_args** thread_args_arr)
{
	thread_args_arr = (thread_args**)malloc(num_of_threads * sizeof(thread_args*));

	if (NULL == thread_args_arr)
	{
		printf("memory allocation failed");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
	}
	return thread_args_arr;
}

//int excute(FILE* input_f, FILE* output_f, int key)
//{
//	char tav;
//	while (!feof(input_f))
//	{
//		tav = fgetc(input_f);
//
//		if (feof(input_f))
//		{
//			break;
//		}
//
//		printf("before decryption %c\n", tav);
//
//		tav = decrepted_char(tav, key);
//
//		printf("after decryption %c\n\n", tav);
//
//		fputc(tav, output_f);
//	}
//}

int main(int argc, char* argv[])
{

	if (argc != 5)
	{
		printf("Invalid input, Please provide encrypted file path and the key and number of threads"); //Not enough arguments.
		return 1;
	}

	int key = *argv[2] - '0';
	int num_of_threads = *argv[3] - '0';

	char* action_input = argv[4];
	printf("action_input: %c\n", action_input[1]);
	if(strcmp(action_input, "-d") != 0  && strcmp(action_input, "-e") != 0)
	{
		printf("Invalid last argument, for encryption type '-e' for decryption type '-d'"); //Not enough arguments.
		return 1;
	} 
	action_mode = action_input[1];

	int input_file_len = strlen(argv[1]);
	char* input_file_name = (char*)malloc((input_file_len + 1) * sizeof(char));
	if (NULL == input_file_name)
	{
		printf("memory allocation failed for input_file_name");

		//TODO CHECK RIGHT EXIT MEHOD IN CASE OF FAIL ALLOCATION
		exit(1);
	}
	input_file_name[input_file_len] = '\0';
	strcpy(input_file_name, argv[1]);

	printf("Num of argc recieved: %d\n", argc);
	printf("key is: %d\n", key);
	printf("num of threads is: %d\n", num_of_threads);
	printf("File path is: %s\n", argv[1]);
	printf("first arg is: %s\n", argv[0]);

	int* lines_per_thread = NULL;
	char* input_txt = NULL;

	HANDLE hFile;
	DWORD dwFileSize;

	hFile = get_input_file_handle(argv[1]);
	dwFileSize = GetFileSize(hFile, NULL);

	printf("num of bytes: %d\n", dwFileSize);

	input_txt = txt_file_to_str(hFile, 0, dwFileSize);

	int num_of_lines = get_num_of_lines(input_txt, dwFileSize);

	printf("num_of_lines: %d\n", num_of_lines);

	lines_per_thread = init_lines_per_thread(num_of_threads, num_of_lines);
	init_start_points(lines_per_thread, input_txt, dwFileSize, num_of_threads);

	free(input_txt);
	int ret_val = CloseHandle(hFile);
	if (!ret_val) {	
		printf("Can't close INPUT file\n");
		exit(1);
	}

	thread_args** thread_args_arr = NULL;
	thread_args_arr = init_thread_args(num_of_threads, thread_args_arr);
	int start_pos = 0, end_pos = 0;
	for (int i = 0; i < num_of_threads; i++)
	{
		if (i >= num_of_lines)
		{
			start_pos = 0;
			end_pos = 0;
		}
		else
		{
			start_pos = lines_per_thread[i];

			if (i != num_of_lines - 1)
				end_pos = lines_per_thread[i + 1];
			else
				end_pos = dwFileSize;
		}
		thread_args_arr[i] = create_thread_arg(key, start_pos, end_pos, input_file_name);
	}

	DWORD wait_code;
	HANDLE p_thread_handles[THREADS_LIMIT];
	DWORD p_thread_ids[THREADS_LIMIT];

	//init_p_thread_handles(num_of_threads, p_thread_handles, p_thread_ids);

	init_output_file(dwFileSize, num_of_lines);

	for (int i = 0; i < num_of_threads; i++)
	{
		p_thread_handles[i] = CreateThreadSimple(translate_file, p_thread_ids[i], thread_args_arr[i]);
	}

	wait_code = WaitForMultipleObjects(num_of_threads, p_thread_handles, TRUE, MAX_WAITING_TIME);




	// gets array of all the threads handles and their amount, and closes the handle for each thread 
	close_threads(p_thread_handles, num_of_threads, wait_code);

	free_memory(input_file_name, lines_per_thread, thread_args_arr, num_of_threads, p_thread_handles, p_thread_ids);

	return 0;
}