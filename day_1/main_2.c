#include <stdio.h>
#include <stdlib.h>

const int begin = 50;

int main()
{
	FILE* fptr;
	size_t flength;
	int dial = begin, dir = 1, ret = 0;
	char* buffer = (char*)malloc(32);
	char* nums = (char*)malloc(32);
	
	fptr = fopen("test", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	while(fgets(buffer, 32, fptr))
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
		while(dial < 0)
			dial = 100 + dial;
		dial %= 100;

		ret += (dial == 0);
	}

	fclose(fptr);

	printf("%d\n", ret);

	return 0;
}
