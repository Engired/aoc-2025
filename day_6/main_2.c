#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

char find(char* buffer, char a, char b, int begin, int end)
{
	for(int i = begin; i < end; i++)
	{
		if(buffer[i] == a)
			return a;
		if(buffer[i] == b)
			return b;
	}

	return '\0';
}

int main()
{
	size_t result = 0;

	FILE* fptr;
	char* buffer = (char*)calloc(1, 10000); // waow
	char** pEnd;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	char** rows = malloc(1);
	int n_rows = 0;
	while(fgets(buffer, 10000, fptr))
	{
		// do this parsing seperately after
		if(buffer[0] == '*' || buffer[0] == '+')
			break;

		rows = realloc(rows, ++n_rows * sizeof(char*));
		rows[n_rows - 1] = strdup(buffer);
	}

	int begin = 0; // mark the beginning of a char
	char** nums; // remember this changes!
	int n_nums = 0;
	for(int i = 0; i < strlen(rows[0]); i++)
	{
		if(rows[0][i] < '0'
			&& rows[1][i] < '0' 
			&& rows[2][i] < '0'
			&& rows[3][i] < '0')
		{
			free(nums);
			n_nums = (i - begin);
			nums = malloc(n_nums * sizeof(char*));
			
			for(int j = 0; j < n_nums; j++)
			{
				nums[j] = calloc(5, sizeof(char));
				nums[j][4] = '\0';

				nums[j][0] = rows[0][begin + j];
				nums[j][1] = rows[1][begin + j];
				nums[j][2] = rows[2][begin + j];
				nums[j][3] = rows[3][begin + j];
			}

			// calculate
			char calc_form = find(buffer, '+', '*', begin, i);
			size_t add = 0;
			switch(calc_form)
			{
				case '+':
					for(int j = 0; j < n_nums; j++)
					{
						add += strtol(nums[j], pEnd, 10);
						printf("%" PRIu64 "\n", strtol(nums[j], pEnd, 10));
					}
					break;
				case '*':
					add = 1;
					for(int j = 0; j < n_nums; j++)
					{
						add *= strtol(nums[j], pEnd, 10);
					}
					break;
				default:
					printf("error!\n");
					break;
			}
			result += add;
			
			begin = i + 1;
		}
	}

	printf("\nGot: %" PRIu64 "\n", result);

	return 0;
}
