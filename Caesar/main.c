#include <stdio.h>



int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Invalid input, Please provide encrypted file path and the key"); //Not enough arguments.
		return 1; 
		
	}
	FILE* encypted_file = fopen(argv[1], "r");
	FILE* decrypted_file = fopen("decrypted.txt", "w");
	if (NULL == encypted_file || NULL == decrypted_file)
	{
		printf("Failed to open one of the files, the program will execute with status 1"); //Failed to open one of the files.
		return 1;
	}

	int key = *argv[2]-'0';

	printf("Num of argc recieved: %d\n", argc);
	printf("key is: %d\n", key);
	printf("File path is: %s\n", argv[1]);
	printf("first arg is: %s\n", argv[0]);



	fclose(encypted_file);
	fclose(decrypted_file);

	return 0;
}