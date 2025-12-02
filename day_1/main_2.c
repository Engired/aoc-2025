#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int begin = 50;

/*
Typical branching:
	1.068036
	1.080109
	1.075134

Minimal branching:
	1.075134
	1.068749
	1.065636

Bit operated comparison:
	
*/

int main()
{
	FILE* fptr;
	size_t flength;
	int dial = begin, dir = 1, ret = 0;
	char* buffer = (char*)malloc(32);
	char* nums = (char*)malloc(32);

	clock_t begin = clock();

	for(int it = 0; it < 10000; it++)
	{
		dir = 1;
		ret = 0;
		dial = 50;
		
		fptr = fopen("input", "r");
		if(fptr == NULL)
			exit(EXIT_FAILURE);

		while(fgets(buffer, 32, fptr) )
		{
			dir = buffer[0] == 'R' ? 1 : -1;

			// read the following numbers~
			int i = 1;
			while(buffer[i] != '\n')
			{
				nums[i - 1] = buffer[i];
				++i;
			}
			nums[i - 1] = '\0';

			dial += atoi(nums) * dir;

			// if it was previously zero, then don't count this tick
			ret -= dial < 0 && dial - (atoi(nums) * dir) == 0;

			// if it's less than 0, it has to pass zero once and then
			// same logic while it is still less than zero
			while(dial < 0)
			{
				dial = 100 + dial;
				++ret;
			}

			// if it points directly to zero
			ret += dial == 0;

			// add for each 100 rotation
			ret += dial / 100;
			dial %= 100;		
		}

		fclose(fptr);
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("%d %f\n", ret, time_spent);

	return 0;
}
