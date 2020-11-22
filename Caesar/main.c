#include <stdio.h>
#include <windows.h>
#include "decryption.h"
#include <stdbool.h>

#define THREADS_LIMIT 64
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))

//
//typedef struct thread_arguments {
//	char* input_file;
//	char output_file[OUTPUT_FILE_NAME_SIZE];
//	int start_pos;
//	int end_pos;
//	int key;
//} thread_args;

int excute(FILE* input_f, FILE* output_f, int key)
{
	char tav;
	while (!feof(input_f))
	{
		tav = fgetc(input_f);

		if (feof(input_f))
		{
			break;
		}

		printf("before decryption %c\n", tav);

		tav = decrepted_char(tav, key);

		printf("after decryption %c\n\n", tav);

		fputc(tav, output_f);
	}
}


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
		lines_per_thread[i] = pos;
	}
	for (i = 0; i < num_of_threads; i++)
	{
		printf("lines_per_thread2 [%d] : %d\n", i, lines_per_thread[i]);

	}
}

//before winapi
//void init_start_points(int* lines_per_thread, FILE* input_f, int num_of_threads)
//{
//	fseek(input_f, 0, SEEK_SET);
//	int pos = 0;
//	int num_of_lines = 0;
//	char tav;
//	int i = 0;
//	int num_of_lines_to_seek = lines_per_thread[0];
//	lines_per_thread[0] = 0;
//	while (!feof(input_f))
//	{
//
//		while (num_of_lines < num_of_lines_to_seek && !feof(input_f))
//		{
//			tav = fgetc(input_f);
//			pos++;
//
//			if (tav == '\n')
//				num_of_lines++;
//		}
//		i++;
//		if (i == num_of_threads)
//			break;
//		num_of_lines_to_seek = lines_per_thread[i];
//		lines_per_thread[i] = pos;
//	}
//
//
//	for (i = 0; i < 2; i++)
//	{
//		printf("lines_per_thread[%d] : %d\n", i, lines_per_thread[i]);
//
//	}
//}


HANDLE create_file_for_read(char* input_file_name)
{
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
	return hFile;
}

char* txt_file_to_str(HANDLE hFile, int file_size)
{
	DWORD  dwBytesRead = 0;

	char* input_txt = (char*)malloc((file_size + 1) * sizeof(char));

	if (NULL == input_txt) 
	{
		printf("Failed to allocate memory. exit\n");
		// TODO free exit properly
	}

	if (FALSE == ReadFile(hFile, input_txt, file_size, &dwBytesRead, NULL))
	{
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(hFile);
		return;
	}

	if (dwBytesRead > 0 && dwBytesRead <= file_size)
	{
		input_txt[dwBytesRead] = '\0'; // NULL character
		printf("succsfull read %d bytes: \n", dwBytesRead);
		printf("%s\n", input_txt);
	}
	else if (dwBytesRead == 0)
	{
		printf("No data read from file\n");
	}

	return input_txt;
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
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_CODE);
	}

	return thread_handle;
}


int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("Invalid input, Please provide encrypted file path and the key and number of threads"); //Not enough arguments.
		return 1; 
		
	}

	int key = *argv[2] - '0';
	int num_of_threads = *argv[3] - '0';
	int input_file_len = strlen(argv[1]);
	char* input_file_name = (char*)malloc((input_file_len + 1) * sizeof(char));
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

	hFile = create_file_for_read(argv[1]);
	dwFileSize = GetFileSize(hFile, NULL);

	printf("num of bytes: %d\n", dwFileSize);

	input_txt = txt_file_to_str(hFile, dwFileSize);

	int num_of_lines = get_num_of_lines(input_txt, dwFileSize);

	printf("num_of_lines: %d\n", num_of_lines);

	lines_per_thread = init_lines_per_thread(num_of_threads, num_of_lines);
	init_start_points(lines_per_thread, input_txt, dwFileSize, num_of_threads);

	thread_args *temp_arg;

	temp_arg = (thread_args*)malloc(sizeof(thread_args));

	temp_arg->key = key;
	temp_arg->start_pos = 0;
	temp_arg->end_pos = dwFileSize;
	strcpy(temp_arg->output_file, OUTPUT_FILE_NAME);
	temp_arg->input_file= input_file_name;


	HANDLE p_thread_handles[2];
	DWORD p_thread_ids[2];
	DWORD wait_code;
	BOOL ret_val;
	size_t i;
	p_thread_handles[0] = CreateThreadSimple(decrypt_file, &p_thread_ids[0], temp_arg);





	wait_code = WaitForSingleObject(p_thread_handles[0], INFINITE);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting");
		return ERROR_CODE;
	}



	////excute(encypted_file, decrypted_file, key);


	//fclose(encypted_file);
	//fclose(decrypted_file);

	return 0;
}