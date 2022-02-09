#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int decider(char inputArrayD[], char previousInput[], int previousCase, int lengthInput, int previousInputLength, int fileRead, char previousRunFile[], char systemCall[], int* exitStatus)
{
	int i;
	int num = 0;

	char* stringNum;
	char* charNum;

	FILE* previousFile;

	strcat(systemCall, "command -v ");
	strcat(systemCall, inputArrayD);
	strcat(systemCall, " > /dev/null");

	if(!strncmp(inputArrayD, "echo", 4))
	{
		if(!strncmp(inputArrayD, "echo $?", 7))
		{
			printf("%d\n", *exitStatus);
		}

		*exitStatus = 0;
		return 1;
	}
	else if(!strncmp(inputArrayD, "!!", 2))
	{
		if(previousInputLength > 0 && fileRead == 1)
		{
			printf("%s\n", previousInput);
		}

		*exitStatus = 0;
		return previousCase;
	}
	else if(!strncmp(inputArrayD, "exit", 4))
	{
		charNum = (char*)malloc(995*sizeof(char));
		stringNum = (char*)malloc(995*sizeof(char));

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

		free(charNum);
		free(stringNum);

		printf("$\n");
		return 3;
	}
	else if(!system(systemCall))
	{
		char* sysCall = (char*)malloc(1000*sizeof(char));

		int status = 0;

		strcat(sysCall, inputArrayD);
		strcat(sysCall, " 2>/dev/null");

		pid_t PID = fork();

		if(!PID)
		{

			system(sysCall);
			free(sysCall);

			exit(0);
		}

		waitpid(PID, &status, WUNTRACED);
		
		*exitStatus = 0;
		free(sysCall);

		return 0;
	}
	else if(lengthInput <= 0)
	{
		*exitStatus = 0;
		return 0;
	}
	
	*exitStatus = 127;
	return -1;

}

void echo(char inputArray[], int lengthInput)
{
	int i;
	
	char* printArray = (char*)malloc(995*sizeof(char));
	       
	for(i = 5; i < lengthInput; i++)
	{
	        printArray[i-5] = inputArray[i];
	}

	if(!strncmp(printArray, "$?", 2))
	{
		return;
	}

	printf("%s\n", printArray);

	free(printArray);
}

void sigHandler(int signal)
{
	switch(signal)
	{
		case 2:
			break;
		case 20:
			break;
	}
}

int main(int argc, char *argv[])
{
	char* previousCommand = (char*)malloc(1000*sizeof(char));
	char systemCallCommand[1000];
	char line;

	int trueValue = 1;
	int previousCommandCase = 0;
	int previousLength = 0;
	int countLine = 0;
	int ExitStatus = 0;
	int decide, i, j;

	FILE* fileRead;

	if(getpid() !=  0)
	{
		struct sigaction sigINT;
		struct sigaction sigTSTP;

		sigemptyset(&sigINT.sa_mask);
		sigINT.sa_handler = sigHandler;
		sigINT.sa_flags = 0;

		sigaction(SIGINT, &sigINT, NULL);

		sigemptyset(&sigTSTP.sa_mask);
		sigTSTP.sa_handler = sigHandler;
		sigTSTP.sa_flags = 0;

		sigaction(SIGTSTP, &sigTSTP, NULL);
	}

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

		if(!strncmp(input, "^C", 2) || !strncmp(input, "^Z", 2))
		{
			continue;
		}

		memset(systemCallCommand, 0, 1000);

		decide = decider(input, previousCommand, previousCommandCase, lengthInput, previousLength, argc, "previousRunFile.txt", systemCallCommand, &ExitStatus);
		
		memset(systemCallCommand, 0, 1000);

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
