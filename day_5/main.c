#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct range
{
	size_t begin;
	size_t end;
} t_range;

int main()
{
	FILE* fptr;
	size_t ret = 0;
	char* buffer = (char*)calloc(1, 256);
	char** pEnd;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	// get ranges
	t_range* ranges = malloc(1);
	size_t n_ranges = 0;
	while(fgets(buffer, 256, fptr))
	{
		if(buffer[0] == '\n')
			break;

		++n_ranges;
		ranges = realloc(ranges, n_ranges * sizeof(t_range));

		// get both numbers
		int div = 0;
		for(int i = 0; i < strlen(buffer); i++)
			if(buffer[i] == '-')
			{
				div = i;
				break;
			}

		char* n_buffer = (char*)calloc(1, 256);
		memcpy(n_buffer, buffer, div);

		ranges[n_ranges - 1].begin = strtol(n_buffer, pEnd, 10);
		
		memcpy(n_buffer, &buffer[div + 1], strlen(buffer) - div + 1);
		n_buffer[strlen(buffer) - div + 1] = '\0';
		ranges[n_ranges - 1].end = strtol(n_buffer, pEnd, 10);
		
		free(n_buffer);
	}	

	int fresh = 0;
	while(fgets(buffer, 256, fptr))
	{
		size_t current_num = strtol(buffer, pEnd, 10);

		for(int i = 0; i < n_ranges; i++)
		{
			if(current_num >= ranges[i].begin && 
				current_num <= ranges[i].end)
			{
				fresh++;
				break;
			}
		}
	}	

	printf("%d\n", fresh);

	fclose(fptr);
	return 0;
}
