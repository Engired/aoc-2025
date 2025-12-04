#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_not(const char* in, char not, int start, int end)
{
	int found = 0;
	for(int i = start; i <= end; i++)
	{
		if(in[i] != not)
			++found;
	}
	
	return found;
}

int max(int a, int b)
{
	return a > b ? a : b;
}

int min(int a, int b)
{
	return a > b ? b : a;
}

int main()
{
	FILE* fptr;
	size_t flength;
	char* buffer = (char*)calloc(1, 256);

	char** lines = (char**)malloc(1 * sizeof(char*));
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	int l_size = 0;
	while(fgets(buffer, 256, fptr))
	{
		++l_size;
		lines = realloc(lines, l_size * sizeof(char*));
		lines[l_size - 1] = strdup(buffer);
	}	

	fclose(fptr);

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////

	int ret = 0;
	int has_pulled = 1;
	while(has_pulled)
	{
		has_pulled = 0;
		char** swap = (char**)malloc(l_size * sizeof(char*)); // push changes onto this
		
		
		// for each line, check a region of 3 (modulated by sides)
		for(int i = 0; i < l_size; i++)
		{
			// copy the line
			swap[i] = strdup(lines[i]);
			
			for(int j = 0; j < strlen(lines[i]) - 1; j++)
			{
				if(lines[i][j] == '.')
					continue;

				int amount = 0;
				if(i != 0)
					amount += num_not(lines[i - 1], '.', max(0, j - 1), min(strlen(lines[i]) - 2, j + 1));
				if(i != l_size - 1)
					amount += num_not(lines[i + 1], '.', max(0, j - 1), min(strlen(lines[i]) - 2, j + 1));
				amount += num_not(lines[i], '.', max(0, j - 1), min(strlen(lines[i]) - 2, j + 1));
				amount -= 1;

				if(amount < 4)
				{
					swap[i][j] = '.';
					has_pulled = 1;
					++ret;
				}
			}
		}

		lines = swap;
	}

	printf("%d\n", ret);
	
	return 0;
}
