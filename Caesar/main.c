#include <stdio.h>


char decrepted_char(char curr_char, int decr_key)
{
	char new_char = 'a';
	if (curr_char >= 'a' && curr_char <= 'z')
		new_char = 'a' + (curr_char - 'a' - decr_key) % 26;
	else
		if (curr_char >= 'A' && curr_char <= 'Z')
			new_char = 'A' + (curr_char - 'A' - decr_key) % 26;
		else
			if (curr_char >= '0' && curr_char <= '9')
				new_char = '0' + (curr_char - '0' - decr_key) % 10;
	return new_char;
}

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
	char test = decrepted_char('d', key);
	printf("%c", test);

	printf("Num of argc recieved: %d\n", argc);
	printf("key is: %d\n", key);
	printf("File path is: %s\n", argv[1]);
	printf("first arg is: %s\n", argv[0]);

	excute(encypted_file, decrypted_file, key);

	fclose(encypted_file);
	fclose(decrypted_file);

	return 0;
}