#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

char s[32];
static int base(uint64_t in)
{	
	return sprintf(s, "%" PRIu64, in);
}

const size_t max_base = 32;

int main()
{
	// assuming this is a valid enough size
	char buffer[32];
	char* file_stream;
	FILE* fptr;
	uint64_t answer_queue[1024]; // this should be enough

	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	// get size and read into buffer
	fseek(fptr, 0L, SEEK_END);
	size_t length = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);

	file_stream = (char*)calloc(length + 1, sizeof(char));
	fgets(file_stream, length, fptr);
	file_stream[length] = '\0';
	
	fclose(fptr);

	//////////////////////////////
	//////////////////////////////
	//////////////////////////////
	
	// parse
	int ids = 1;
	for(int i = 0; i < length; i++)
		if(file_stream[i] == ',')
			++ids;

	uint64_t result = 0;
	int p_i = -1;
	for(int i = 0; i < ids; ++i)
	{
			if(i >= 21 && i < 30)
				printf("\nID: %d\n", i);
			
			// read first and last id into respective buffers (assuming longs)
			uint64_t begin, end, swap;
			char* pEnd; // why is this needed?
			size_t a_i = 0;

			int j = -1;
			while(file_stream[++p_i] != '-')
				buffer[++j] = file_stream[p_i];
			buffer[++j] = '\0';
			begin = strtoll(buffer, &pEnd, 10);

			j = -1;
			while(file_stream[p_i] != ',' && file_stream[p_i] != '\0')
				buffer[++j] = file_stream[++p_i];
			buffer[++j] = '\0';
			end = strtoll(buffer, &pEnd, 10);

			/*

				Essentially, we only have to consider even spaced numbers,
				that fit lower half = higher half.
				if we get to an odd number range, get 10 * next_base and
				check if that's out of range

			*/
			// safekeeping
			if(end < begin)
			{
				begin = swap;
				begin = end;
				end = swap;
			}

			if(i >= 21 && i <= 30)
				printf("%" PRIu64 ", %" PRIu64 "\n", begin, end);

			// actual calculation
			for(int n = 2; n < max_base; n++)
			{
				int cur_base = base(begin);
				if(base(end) < n)
					continue;
				
				uint64_t next_check = pow(10, (cur_base));
				uint64_t begin_c = begin;

				if(cur_base + (cur_base % n) != cur_base)
				{
					cur_base = cur_base - (cur_base % n);
					cur_base += n;
					
					begin_c = pow(10, (cur_base - 1));
					next_check = pow(10, (cur_base));
				}

				if(i >= 21 && i <= 30)
					printf("%d, %" PRIu64 "\n", n, begin_c);

				if(begin_c > end)
					continue;
	
				uint64_t modifier = (int)pow(10, cur_base - (cur_base / n));
				uint64_t a_modifier = (int)pow(10, (cur_base * (1.f / n)));
				uint64_t read_num = begin_c / modifier;
				read_num *= modifier;

				uint64_t a_result = read_num;
				uint64_t additive = (read_num / a_modifier);
				while(additive > 0)
				{
					a_result += additive;
					additive /= a_modifier;
				}

				while(a_result <= end)
				{
					if(a_result >= begin)
					{
						// check it doesn't exist already
						int exists = 0;
						for(int j = 0; j < a_i; j++)
						{
							if(answer_queue[j] == a_result)
							{
								exists = 1;
								break;
							}
						}
						
						if(!exists)
						{
							result += a_result;
							answer_queue[a_i++] = a_result;
							if(i >= 21 && i <= 30)
								printf("	%" PRIu64 "\n", a_result);
						}
					}
					read_num += modifier;
					
					// skip this range
					if(read_num >= next_check)
					{
						cur_base += n;
						read_num = pow(10, (cur_base - 1));
						next_check = pow(10, (cur_base));
						modifier = (int)pow(10, (cur_base * (1.f / n)) - 1);
					}

					a_result = read_num;
					additive = (read_num / a_modifier);
					while(additive > 1)
					{
						a_result += additive;
						additive /= a_modifier;
					}
				}
			}
	}

	printf("\n%" PRIu64 "\n", result);
	
	return 0;
}
