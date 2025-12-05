#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

	size_t* numbers_used = malloc(sizeof(size_t));
	size_t n_used = 0;
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

	for(int i = 0; i < n_ranges; i++)
	{
		printf("Trying %" PRIu64 ", %" PRIu64 "\n",
					ranges[i].begin, ranges[i].end);
		// for each element compare against every other element
		for(int j = 0; j < n_ranges; j++)
		{
			if(i == j)
				continue;
			
			if(ranges[i].end <= ranges[j].end 
				&& ranges[i].begin >= ranges[j].begin)
			{
				printf("	Swapping %" PRIu64 ", %" PRIu64 " with %" PRIu64 ", %" PRIu64 "\n",
					ranges[n_ranges - 1].begin, ranges[n_ranges - 1].end, ranges[i].begin, ranges[i].end);

				t_range swap = {ranges[i].begin, ranges[i].end};
				ranges[i].begin = ranges[n_ranges - 1].begin;
				ranges[i].end = ranges[n_ranges - 1].end;
				ranges[n_ranges - 1].begin = swap.begin;
				ranges[n_ranges - 1].end = swap.end;
				--i;
				
				ranges = realloc(ranges, (--n_ranges) * sizeof(t_range));
				break;
			}

			// if begin is inside another range
			if(ranges[i].begin >= ranges[j].begin 
				&& ranges[i].begin <= ranges[j].end)
			{
				ranges[i].begin = ranges[j].end + 1;
				if(ranges[i].begin > ranges[i].end)
				{
					t_range swap = {ranges[i].begin, ranges[i].end};
					ranges[i].begin = ranges[n_ranges - 1].begin;
					ranges[i].end = ranges[n_ranges - 1].end;
					ranges[n_ranges - 1].begin = swap.begin;
					ranges[n_ranges - 1].end = swap.end;
					--i;
					
					ranges = realloc(ranges, (--n_ranges) * sizeof(t_range));
					break;
				}
			}

			// if end is inside another range
			if(ranges[i].end >= ranges[j].begin 
				&& ranges[i].end <= ranges[j].end)
			{
				ranges[i].end = ranges[j].begin - 1;
				if(ranges[i].end < ranges[i].begin)
				{
					t_range swap = {ranges[i].begin, ranges[i].end};
					ranges[i].begin = ranges[n_ranges - 1].begin;
					ranges[i].end = ranges[n_ranges - 1].end;
					ranges[n_ranges - 1].begin = swap.begin;
					ranges[n_ranges - 1].end = swap.end;
					--i;
					
					ranges = realloc(ranges, (--n_ranges) * sizeof(t_range));
					break;
				}
			}
		}
	}

	for(int j = 0; j < n_ranges; j++)
	{
		printf("Got %" PRIu64 ", %" PRIu64 "\n",
					ranges[j].begin, ranges[j].end);
		ret += ranges[j].end - ranges[j].begin;
		++ret;
	}

	printf("%" PRIu64 "\n", ret);
	
	fclose(fptr);
	return 0;
}
