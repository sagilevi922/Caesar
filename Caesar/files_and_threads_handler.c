#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "files_and_threads_handler.h"
#include "HardCodedData.h"

HANDLE get_input_file_handle(char* input_file_name)
{
	if (strstr(input_file_name, ".txt") == NULL)
	{
		//printf("Invalid input file name");
		return NULL;
	}
	HANDLE hFile;
	hFile = CreateFileA(input_file_name,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(("Terminal failure: unable to open file \"%s\" for read.\n"), input_file_name);
		return NULL;
	}
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		printf("Invalid input file name. no such file existing. Finish program");
		return NULL;
	}
	return hFile;
}
//Gets the full path of the input file: input_path, and it's length: input_file_len
//defines the output file name and path to create, and returns it
char* init_output_file_name(char* input_path, int input_file_len)
{
	char* output_file_name = NULL;
	int i = 0, last_backslash_pos = 0;

	while (input_path[i] != '\0')
	{
		if (input_path[i] == '\\')
			last_backslash_pos = i;
		i++;
	}
	output_file_name = (char*)malloc((last_backslash_pos + 2 + OUTPUT_FILE_NAME_SIZE) * sizeof(char));
	if (NULL == output_file_name)
	{
		printf("Failed to allocate memory.\n");
		return NULL;
	}
	i = 0;
	if (last_backslash_pos)
		for (i = 0; i <= last_backslash_pos; i++)
		{
			*(output_file_name + i) = input_path[i];
		}
	*(output_file_name + i) = '\0';
	extern char action_mode;
	if (action_mode == 'd')
		strcat(output_file_name, OUTPUT_FILE_NAME_DEC);
	else
		strcat(output_file_name, OUTPUT_FILE_NAME_ENC);
	printf("output_file_name is: %s", output_file_name);
	return output_file_name;
}

HANDLE create_file_for_write(char* output_file_name)
{
	DWORD file_ptr;
	HANDLE hFile;
	extern char action_mode;
	hFile = CreateFileA(output_file_name,               // file to open
		GENERIC_WRITE,          // open for reading
		FILE_SHARE_WRITE,       // share for reading
		NULL,                  // default security
		OPEN_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf(("Terminal failure: unable to open the output file for write.\n"));
		return NULL;
	}

	return hFile;
}

int close_handles_proper(HANDLE file_handle)
{
	//arguments check - exrported function
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		printf("Invalid HANDLE value, can't close this HANDLE.\n");
	}
	int ret_val = 0;
	ret_val = CloseHandle(file_handle);
	if (false == ret_val)
	{
		printf("Error when closing\n");
		return ERROR_CODE;
	}
	return 1;

}

char* txt_file_to_str(HANDLE hFile, int start_pos, int input_size)
{

	//arguments check - exrported function
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	if (start_pos < 0 || input_size < 0)
	{
		printf("invalid starting position for current thread");

		close_handles_proper(hFile);
		return NULL;
	}
	DWORD  dwBytesRead = 0;

	char* input_txt = NULL;
	input_txt = (char*)malloc((input_size + 1) * sizeof(char));
	if (NULL == input_txt)
	{
		printf("Failed to allocate memory.\n");
		return NULL;
	}

	if (FALSE == ReadFile(hFile, input_txt, input_size, &dwBytesRead, NULL))
	{
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		close_handles_proper(hFile);
		free(input_txt);
		return NULL;
	}

	if (dwBytesRead > 0 && dwBytesRead <= input_size)
	{
		input_txt[dwBytesRead] = '\0'; // NULL character
		//printf("succsfull read %d bytes: \n", dwBytesRead);
		//printf("\ninput txt:\n%s\n\n", input_txt);
	}
	else if (dwBytesRead == 0)
	{
		printf("No data read from file\n");
	}

	return input_txt;
}



//Gets the entrance function of the new thread: p_start_routine, it's id: p_thread_id,
//and as struct that contain its arguments: paths of input and output files, a key for operation,
//starting position of operation and the length of the section to operate.
//It creates a new thread and return a HANDLE to it.
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


//creates a output file according to the arguments: size: file_size, and number of lines: num_of_lines
//and according to the right mode: encryption or decryption.
//the function returns 0 for success or 1 else.
int init_output_file(int file_size, int num_of_lines, char* output_file_name)
{
	HANDLE hfile;
	DWORD end_file_ptr;

	hfile = CreateFileA(
		output_file_name,
		GENERIC_WRITE, //Open file with write mode
		FILE_SHARE_WRITE, //the file should be shared by the threads.
		NULL, //default security mode
		CREATE_ALWAYS, //first time we open the file.
		FILE_ATTRIBUTE_NORMAL, //normal attribute
		NULL //not relevant for open file operations.
	);


	if (hfile == INVALID_HANDLE_VALUE)
	{
		printf("Can't open the outputfile. exit\n");
		return 1;
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
		return 1;
	}

	int set_end_of_file = SetEndOfFile(hfile);
	if (!set_end_of_file) {
		printf("Can't set file pointer of outputfile. exit\n");
		return 1;
	}

	if (close_handles_proper(hfile) != 1)
		return 1;

	return 0;
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
		close_handles_proper(p_thread_handles[i]);
	}

}

// gets number of thread and a pointer to array of thread args, and allocats dynamic memory for it. 
thread_args** init_thread_args(int num_of_threads, thread_args** thread_args_arr, int num_of_lines, int key, DWORD dwFileSize, char* input_file_name, int* lines_per_thread, char* output_file_name)
{
	thread_args_arr = (thread_args**)malloc(num_of_threads * sizeof(thread_args*));

	if (NULL == thread_args_arr)
	{
		printf("memory allocation failed");
		return NULL;
	}

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
		thread_args_arr[i] = create_thread_arg(key, start_pos, end_pos, input_file_name, output_file_name);
		if (NULL == thread_args_arr[i])
		{
			for (int j = 0; j < i; j++)
			{
				free(thread_args_arr[j]);
			}
			free(thread_args_arr);
			return NULL;
		}
	}


	return thread_args_arr;
}


// gets paremters for thread and create a thread argument struct pointer
thread_args* create_thread_arg(int key, int start_pos, int end_pos, char* input_file_name, char* output_file_name)
{
	thread_args* temp_arg = (thread_args*)malloc(sizeof(thread_args));
	if (NULL == temp_arg)
	{
		printf("memory allocation failed");
		return NULL;
	}
	temp_arg->key = key;
	temp_arg->start_pos = start_pos;
	temp_arg->end_pos = end_pos;
	temp_arg->output_file = output_file_name;
	//strcpy(temp_arg->output_file, output_file_name);
	//strcpy(temp_arg->output_file, OUTPUT_FILE_NAME_ENC);
	temp_arg->input_file = input_file_name;

	return temp_arg;
}
