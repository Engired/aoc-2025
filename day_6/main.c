#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	size_t result = 0;

	FILE* fptr;
	char* buffer = (char*)calloc(1, 10000); // waow
	char** pEnd;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	int** n_arrays = malloc(1);
	int s_arr = 0;
	int width = -1;
	while(fgets(buffer, 10000, fptr))
	{
		// do this parsing seperately after
		if(buffer[0] == '*' || buffer[0] == '+')
			break;

		// get each number into a new array
		char n_buffer[10] = {0};
		int* sub_array = malloc(1); 
		int n_sub = 0;
		int j = 0;
		
		for(int i = 0; i < strlen(buffer) - 1; i++)
		{
			if(buffer[i] == ' ')
			{
				if(n_buffer[0] > '0')
				{
					n_buffer[j] = '\0';
					j = 0;

					sub_array = realloc(sub_array, (++n_sub) * sizeof(int));
					sub_array[n_sub - 1] = strtol(n_buffer, pEnd, 10);
					
					n_buffer[0] = ' ';
				}
				continue;
			}

			n_buffer[j] = buffer[i];
			++j;
		}
		if(n_buffer[0] > '0')
		{
			n_buffer[j] = '\0';
			j = 0;

			sub_array = realloc(sub_array, (++n_sub) * sizeof(int));
			sub_array[n_sub - 1] = strtol(n_buffer, pEnd, 10);
			
			n_buffer[0] = ' ';	
		}

		// who cares if we do this like this. idk
		++s_arr;
		n_arrays = realloc(n_arrays, s_arr * sizeof(int*));
		n_arrays[s_arr - 1] = sub_array;
		width = n_sub;
	}	

	int j = 0;
	for(int i = 0; i < strlen(buffer); i++)
	{
		if(buffer[i] != ' ')
		{
			size_t new = 0;
			if(buffer[i] == '*')
			{
				new = 1;
				for(int x = 0; x < s_arr; ++x)
				{
					new *= n_arrays[x][j];
				}
				result += new;
			}
			if(buffer[i] == '+')
			{
				for(int x = 0; x < s_arr; ++x)
				{
					new += n_arrays[x][j];
				}
				result += new;
			}
			++j;
		}
	}

	printf("%" PRIu64 "\n", result);

	return 0;
}
