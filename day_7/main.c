#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

int can_add(int* arr, int s, int new)
{
	for(int i = 0; i < s; i++)
		if(arr[i] == new)
			return 0;
	return 1;
}

int main()
{
	size_t result = 0;

	FILE* fptr;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	// get first buffer and find beam start
	char* buffer = (char*)calloc(1, 10000);
	fgets(buffer, 10000, fptr);

	int split_loc;
	for(split_loc = 0; split_loc < strlen(buffer); ++split_loc)
		if(buffer[split_loc] == 'S')
			break;

	// make a list of beams, combine if overlapping, and run through lines
	int* arr = malloc(1 * sizeof(int));
	int n_buffers = 1;
	arr[0] = split_loc;
	while(fgets(buffer, 10000, fptr))
	{
		for(int i = 0; i < n_buffers; i++)
		{
			if(buffer[arr[i]] == '^')
			{
				++result;
				int split = arr[i];
				int did_split = 0;
				
				// split
				if(can_add(arr, n_buffers, split - 1))
				{
					arr[i] = split - 1;
					did_split = 1;
				} else if(can_add(arr, n_buffers, split + 1))
				{
					arr[i] = split + 1;
					continue;
				}

				if(can_add(arr, n_buffers, split + 1))
				{
					++n_buffers;
					arr = realloc(arr, n_buffers * sizeof(int));
					arr[n_buffers - 1] = split + 1;
					did_split = 1;
				}

				if(!did_split)
				{
					// remove number
					for(int j = i; j < n_buffers - 1; j++)
						arr[j] = arr[j + 1];
					arr = realloc(arr, --n_buffers * sizeof(int));
					--i;
				}
			}
		}
	}

	fclose(fptr);

	printf("Got: %" PRIu64 "\n", result);

	return 0;
}
