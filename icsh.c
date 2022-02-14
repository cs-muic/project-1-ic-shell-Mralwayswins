#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * This is the C file for project 1 in PCSA T2/2021-2022
 * Name: Zhiyu Song
 * UID: 6380631
 * Major: Computer Science
 *
 * This file now can support features up to Milestone 5 and parts of Milestone 6
 * Details will be in the code below 
 */

//These global vars are here to prevent them from reseting their own value for safety reasons
int l;
int jobs = 0;
int checkFunction = 0;
char jobStatus[256][10];
char jobNames[256][1000];
int jobID[256];

//This is the function that takes in the input and decide what to do with it
int decider(char inputArrayD[], char previousInput[], int previousCase, int lengthInput, int previousInputLength, int fileRead, char previousRunFile[], char systemCall[], int* exitStatus)
{
	int i;
	int j = 0;
	int k = 0;
	int num = 0;
	int status = 0;

	char* stringNum;
	char* charNum;

	FILE* previousFile;

	strcat(systemCall, "command -v ");
	strcat(systemCall, inputArrayD);
	strcat(systemCall, " >>/dev/null 2>>/dev/null");

	//This if statement is for the built in command echo, it will echo everything behind it back with one exception
	//if input entered was echo $?, it will print the exit code of the previous command
	if(!strncmp(inputArrayD, "echo", 4))
	{
		if(!strncmp(inputArrayD, "echo $?", 7))
		{
			printf("%d\n", *exitStatus);
		}

		*exitStatus = 0;
		return 1;
	}
	//This repeats what the last command did
	else if(!strncmp(inputArrayD, "!!", 2))
	{
		if(previousInputLength > 0 && fileRead == 1)
		{
			printf("%s\n", previousInput);
		}

		*exitStatus = 0;
		return previousCase;
	}
	//This part controls the exit of this code, it will also store the exit number of the last run
	else if(!strncmp(inputArrayD, "exit", 4))
	{
		charNum = (char*)malloc(995*sizeof(char));
		stringNum = (char*)malloc(995*sizeof(char));

		//This checks what is the exit number
		for(i = 5; i < lengthInput; i++)
		{
			charNum[i-5] = inputArrayD[i];
		}
		
		//This checks for inputs from the start of the program
		if(fileRead == 2)
                {
			//This will read the exit code of last run
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

		//Check if the exit code is in between 0-255
		num = atoi(charNum) % 256;

		if(num < 0)
		{
			num = num * (-1);
		}

		if(num > 1)
		{
			printf("%d\n", num);
		}

		//This only prints the statement for no file input at the start
		if(fileRead == 1)
		{
			printf("Closing\n");
		}

		//This checks for the exit code and stores it for the next run
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
	//This will print all the current jobs
	else if(!strcmp(inputArrayD, "jobs"))
	{
		for(i = 1; i < jobs; i++)
		{
			printf("[%d]  %s		%s\n", i, jobStatus[i], jobNames[i]);
		}

		return 0;
	}
	//This checks if the command entered should be ran in the background or not
	else if(inputArrayD[lengthInput-1] == '&' && !system(systemCall))
	{
		char* argumentList[1000];
		
		int success;

		memset(argumentList, 0, 1000);

		//Split the input in order to use execvp()
		for(i = 0; i < lengthInput; i++)
		{
			if(inputArrayD[i] == ' ')
			{
				j++;
				k = 0;

				continue;
			}
			argumentList[j][k] == inputArrayD[i];
			
			k++;
		}

		argumentList[j] = "NULL";

		jobs++;

		//Storing all relevent info in it for the command jobs
		if(inputArrayD[lengthInput -1] == '&')
		{
			inputArrayD[lengthInput -1] = '\0';
		}

		strcat(jobNames[jobs], inputArrayD);

		strcat(jobStatus[jobs], "Running");

		checkFunction = 1;

		//fork to run the command in the child process
		pid_t backID = fork();

		if(backID == 0)
		{

			jobID[jobs] = getpid();
			printf("[%d] %d\n", jobs, getpid());

			success = execvp(argumentList[0], argumentList);

			exit(0);
			
		}

		//Catches the child process
		if(waitpid(jobID[jobs], &status, WUNTRACED) > 0)
		{
			printf("[%d]  Done              %s\n", jobs, jobNames[jobs]);

		}

		//Reinitializing the space because the command ended
		checkFunction = 0;

                memset(jobNames[jobs], 0, 1000);
                memset(jobStatus[jobs], 0, 10);

                jobID[jobs] = 0;
                jobs--;



		*exitStatus = 0;

		for(i = 0; i < j; i++)
		{
			free(argumentList[i]);
		}

		//Check for if the command is successful
		if(success == -1)
		{
			*exitStatus = -1;
			return -1;
		}

		return 0;
	}
	//This part takes care of running external programs in the foreground
	else if(!system(systemCall))
	{
		char* sysCall = (char*)malloc(1000*sizeof(char));
                
		//Saving the info for the command jobs
		jobs++;

                strcat(jobNames[jobs], inputArrayD);

                strcat(jobStatus[jobs], "Running");

		strcat(sysCall, inputArrayD);

		//Again, fork to run it on the child process
		pid_t PID = fork();

		checkFunction = 1;

		if(!PID)
		{
			jobID[jobs] = getpid();
			system(sysCall);
			free(sysCall);

			exit(0);
		}

		//Catch the child process
		waitpid(PID, &status, 0);

		checkFunction = 0;

		memset(jobNames[jobs], 0, 1000);
                memset(jobStatus[jobs], 0, 10);

                jobID[jobs] = 0;
                jobs--;

		*exitStatus = 0;
		free(sysCall);

		return 0;
	}
	//If input does not exist, do nothing
	else if(lengthInput <= 0)
	{
		*exitStatus = 0;
		return 0;
	}
	//This checks for bad commands
	*exitStatus = 127;
	return -1;

}

//The function for echo
void echo(char inputArray[], int lengthInput)
{
	int i;
	
	char* printArray = (char*)malloc(995*sizeof(char));
	       
	//Loop to get the content to echo
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

//This is a signal handler for handling both SIGINT and SIGTSTP
void sigHandler(int signal)
{

	//This makes the parent process immune to the SIGINT and SIGTSTP
	switch(signal)
	{
		case SIGINT:
			break;
		case SIGTSTP:
			//Save the status of the jobs
			if(checkFunction == 1)
			{
				memset(jobStatus[jobs], 0, 10);
				strcat(jobStatus[jobs], "Stopped");
			}
			break;
	}
}

//Where everything is connected together
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

	//Initializing everything for future use
	for(i = 0; i < 256; i++)
	{
		jobID[i] = 0;
		memset(jobNames[i], 0, 1000);
		memset(jobStatus[i], 0, 10);
	}

	FILE* fileRead;

	//This limits the child processes from being able to handle SIGINT and SIGTSTP
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

	//For script mode only, this will open the file for others to use
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
	//For running normally
	else
	{
		printf("IC shell is starting\n");
	}

	//This will keep looping over itself and wait for the user to decide whether to quit or not
	while(trueValue)
	{

		char* input = (char*)malloc(1000*sizeof(char));

		int lengthInput = 0;

		//Normal mode
		if(argc == 1)
		{
			printf("icsh $ ");

			scanf("%[^\n]%n", input, &lengthInput);
			while(getchar() != '\n');

		}
		//Script mode
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
		//error checking for the input file
		else
		{
			printf("This program only accept one file Maximum\n");
		}

		//Exclusively to skip over the symbols left behind by the SIGINT and SIGTSTP
		if(!strncmp(input, "^C", 2) || !strncmp(input, "^Z", 2))
		{
			continue;
		}

		memset(systemCallCommand, 0, 1000);

		//Calling the decide function for what to do with the input
		decide = decider(input, previousCommand, previousCommandCase, lengthInput, previousLength, argc, "previousRunFile.txt", systemCallCommand, &ExitStatus);
		
		memset(systemCallCommand, 0, 1000);

		//For repeating a command
		if(!strncmp(input, "!!", 2))
		{

			free(input);

			input = (char*)malloc(1000*sizeof(char));

			//Saves everything for next time when it is called again
			for(i = 0; i < previousLength; i++)
			{
				input[i] = previousCommand[i];
			}

			lengthInput = previousLength;

		}

		//Using the results from decide, determines what else need to be done
		switch(decide)
		{
			//Normal cases
			case 0:
				break;
			//Echo
			case 1:
				echo(input, lengthInput);
				break;
			//For exiting and script mode
			case 3:
				trueValue = 0;
				break;
			//For bad commands
			default:
				printf("bad command\n");
				break;
		}

		//Saving the current command to use next time
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
	//The end of the program
	return 0;
}
