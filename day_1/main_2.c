#include <stdio.h>
#include <stdlib.h>

const int begin = 50;

/*
82, 1
52, 1
0, 2
95, 2
55, 3
0, 4
99, 4
0, 5
14, 5
32, 6
*/

int main()
{
	FILE* fptr;
	size_t flength;
	int dial = begin, dir = 1, ret = 0, oldret = 0;
	char* buffer = (char*)malloc(32);
	char* nums = (char*)malloc(32);
	
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
		if(dial < 0 && dial - (atoi(nums) * dir) == 0)
			ret--;

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

		oldret = ret;
	}

	fclose(fptr);

	printf("%d\n", ret);

	return 0;
}
