#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

int decider(char inputArrayD[], char previousInput[], int previousCase, int lengthInput, int previousInputLength)
{
	int i;
	int num = 0;
	char* charNum = (char*)malloc(995*sizeof(char));

	if(!strncmp(inputArrayD, "echo", 4))
	{
		return 1;
	}
	else if(!strncmp(inputArrayD, "!!", 2))
	{
		if(previousInputLength > 0)
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
		

		num = atoi(charNum) % 256;

		if(num < 0)
		{
			num = num * (-1);
		}

		if(num > 1)
		{
			printf("%d\n", num);
		}

		printf("Closing\n");
		printf("$\n");
		return 3;
	}
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

int main()
{
	char* previousCommand = (char*)malloc(1000*sizeof(char));

	int trueValue = 1;
	int previousCommandCase = 0;
	int previousLength = 0;
	int decide, i, j;

	printf("IC shell is starting\n");

	while(trueValue)
	{
		char* input = (char*)malloc(1000*sizeof(char));

		int lengthInput = 0;

		printf("icsh $ ");

		scanf("%[^\n]%n", input, &lengthInput);
		while(getchar() != '\n');

		decide = decider(input, previousCommand, previousCommandCase, lengthInput, previousLength);

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

	free(previousCommand);
	return 0;
}

