#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int man_cmp(const char* in, int a, int b)
{
	// search through all letters (-1), skip certain indices.
	// return which index keeps the lower result
	int ind_1 = 0, ind_2 = 0;
	for(int i = 0; i < strlen(in) - 1; i++)
	{
		if(ind_1 == a)
			++ind_1;
		if(ind_2 == b)
			++ind_2;

		if(in[ind_1] > in[ind_2])
			return a;
		else if(in[ind_1] < in[ind_2])
			return b;

		++ind_1;
		++ind_2;
	}

	// if they are equal
	return 0;
}

char* narrow(char* in, int split)
{
	size_t len = strlen(in);
	char* out = malloc(len);
	
	memcpy(out, in, split);
	memcpy(out + split, in + split + 1, len - split);

	out[len - 1] = '\0';
	return out;
}

int main()
{
	FILE* fptr;
	size_t flength;
	size_t ret = 0;
	char* buffer = (char*)calloc(1, 256);
	char* nums = (char*)malloc(32);
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	while(fgets(buffer, 256, fptr))
	{
		/*
			If we make a list of every possible removal, one will move it down the least
			Since it's too long, we have to do a *unique* strcmp
		*/

		char* new = buffer;

		// 87
		for(int i = 0; i <= 87; i++)
		{
			int cur_remove = 0;
			for(int i = 1; i < strlen(new) - 1; i++)
			{
				int cache = man_cmp(new, cur_remove, i);
				if(cache != 0)
					cur_remove = cache;
			}
			new = narrow(new, cur_remove);
		}

		char* pEnd;
		ret += strtoll(new, &pEnd, 10);
	}	

	fclose(fptr);

	printf("Got: %" PRIu64 "\n", ret);
}
