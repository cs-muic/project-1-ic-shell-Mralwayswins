#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

int decider(char inputArrayD[], char previousInput[], int previousCase, int lengthInput, int previousInputLength, int fileRead, char previousRunFile[])
{
	int i;
	int num = 0;

	char* stringNum = (char*)malloc(995*sizeof(char));
	char* charNum = (char*)malloc(995*sizeof(char));
	FILE* previousFile;

	if(!strncmp(inputArrayD, "echo", 4))
	{
		return 1;
	}
	else if(!strncmp(inputArrayD, "!!", 2))
	{
		if(previousInputLength > 0 && fileRead == 1)
		{
			printf("%s\n", previousInput);
		}

		return previousCase;
	}
	else if(!strncmp(inputArrayD, "exit", 4))
	{

		for(i = 5; i < lengthInput; i++)
		{
			charNum[i-5] = inputArrayD[i];
		}
		
		if(fileRead == 2)
                {
                        if(access(previousRunFile, F_OK) == 0)
                        {
                                int previousExitNum;

                                FILE* readFile;
                                readFile = fopen(previousRunFile, "r");

                                fscanf(readFile, "%d", &previousExitNum);

				if(previousExitNum > 1 )
				{
					printf("%d\n", previousExitNum);
				}
                        }
                }

		num = atoi(charNum) % 256;

		if(num < 0)
		{
			num = num * (-1);
		}

		if(num > 1)
		{
			printf("%d\n", num);
		}

		if(fileRead == 1)
		{
			printf("Closing\n");
		}

		previousFile = fopen(previousRunFile, "w");

		if(previousFile == NULL)
		{
			printf("File not created (BUG)\n");
		}

		sprintf(stringNum, "%d", num);

		fputs(stringNum, previousFile);

		fclose(previousFile);

		printf("$\n");
		return 3;
	}
	else if(!system(inputArrayD)){}
	else if(lengthInput <= 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}

}

void echo(char inputArray[], int lengthInput)
{
	int i;
	
	char* printArray = (char*)malloc(995*sizeof(char));
	       
	for(i = 5; i < lengthInput; i++)
	{
	        printArray[i-5] = inputArray[i];
	}

	printf("%s\n", printArray);
		
}

int main(int argc, char *argv[])
{
	char* previousCommand = (char*)malloc(1000*sizeof(char));
	char line;

	int trueValue = 1;
	int previousCommandCase = 0;
	int previousLength = 0;
	int countLine = 0;
	int decide, i, j;

	FILE* fileRead;

	if(argc == 2)
	{

		fileRead = fopen(argv[1], "r");

		while(!feof(fileRead))
		{
			line = fgetc(fileRead);
			if(line == '\n')
			{
				countLine++;
			}
		}

		fclose(fileRead);

		fileRead = fopen(argv[1], "r");
	}
	else
	{
		printf("IC shell is starting\n");
	}

	while(trueValue)
	{

		char* input = (char*)malloc(1000*sizeof(char));

		int lengthInput = 0;

		if(argc == 1)
		{
			printf("icsh $ ");

			scanf("%[^\n]%n", input, &lengthInput);
			while(getchar() != '\n');
		}
		else if(argc == 2)
		{
			fgets(input, 1000, fileRead);

			lengthInput = strlen(input);
			
			if(input[lengthInput - 1] == '\n')
			{
				input[lengthInput - 1] = 0;
			}
			
			countLine--;

			if(countLine <= 0)
			{
				trueValue = 0;
			}
		}
		else
		{
			printf("This program only accept one file Maximum\n");
		}

		decide = decider(input, previousCommand, previousCommandCase, lengthInput, previousLength, argc, "previousRunFile.txt");

		if(!strncmp(input, "!!", 2))
		{

			free(input);

			input = (char*)malloc(1000*sizeof(char));

			for(i = 0; i < previousLength; i++)
			{
				input[i] = previousCommand[i];
			}

			lengthInput = previousLength;

		}

		switch(decide)
		{
			case 0:
				break;
			case 1:
				echo(input, lengthInput);
				break;
			case 3:
				trueValue = 0;
				break;
			default:
				printf("bad command\n");
				break;
		}

		previousCommandCase = decide;

		previousLength = lengthInput;

		if(lengthInput > 0)
		{

			free(previousCommand);

			previousCommand = (char*)malloc(1000*sizeof(char));

			for(i = 0; i < lengthInput; i++)
			{
				previousCommand[i] = input[i];
			}
		}

		free(input);
	}
	if(argc == 2)
	{
		fclose(fileRead);
	}

	free(previousCommand);
	return 0;
}

