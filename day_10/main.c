#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// testing...
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int depth_search(int d, uint32_t* buttons, size_t n_flips, uint32_t start, uint32_t expected)
{
	uint32_t mask = 1;

	for(int i = 0; i < n_flips; i++)
	{
		if((start ^ mask) + mask == start)
		{
			mask <<= 1;
			continue;
		}

		if(d == 0)
		{
			// end of depth loop, check buttons, return if valid depth

			// get all buttons
			uint32_t check = start + mask;
			uint32_t intended = 0;
			int i = 0;
			while(check != 0)
			{
				if((check << 31) >> 31 == 1)
				{
					intended ^= buttons[i];	
				}
				
				++i;
				check >>= 1;
			}
			
			if(intended == expected)
				return 1;
		} else 
		{
			// search recursively down
			if(depth_search(d - 1, buttons, n_flips, start + mask, expected))
				return 1;
		}

		mask <<= 1;
	}
	
	return 0;
}

int main()
{
	FILE* fptr;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);
	
	char* buffer = (char*)calloc(1, 10000);

	// we can use bit logic for significantly faster flipping
	uint32_t* expected_light = NULL;
	size_t* length = NULL;
	size_t n_light = 0;

	uint32_t** buttons = NULL;
	size_t* n_flips = NULL;
	size_t n_buttons = 0;


	while(fgets(buffer, 10000, fptr))
	{
		expected_light = realloc(expected_light, ++n_light * sizeof(uint32_t));
		length = realloc(length, n_light * sizeof(size_t));
		
		// learnt about digraphs. kinda funny to use them tbh

		uint32_t insert = 0;
		int i = 1;
		for(i = 1; i < strlen(buffer); i++)
		<%
			if(buffer[i] == ']')
				break;

			if(buffer[i] == '#')
				insert += 1;

			// this exists?
			insert <<= 1;
		%>
		insert >>= 1;

		length[n_light - 1] = i - 1;
		expected_light[n_light - 1] = insert;

		buttons = realloc(buttons, ++n_buttons * sizeof(uint32_t*));
		buttons[n_buttons - 1] = NULL;
		n_flips = realloc(n_flips, n_buttons * sizeof(size_t));
		n_flips[n_buttons - 1] = 0;

		for(i += 2; i < strlen(buffer) - 1; i++)
		{
			uint32_t button = 0;
			
			if(buffer[i] == '(')
			{
				char n_buffer[4] = {0};
				int n_i = -1;

				for(i++; i < strlen(buffer) - 1; i++)
				{
					if(buffer[i] == ',')
					{
						button |= (1 << length[n_light - 1]) >> (atoi(n_buffer) + 1);
						memset(n_buffer, 0, 4);
						n_i = -1;

						continue;
					}
					if(buffer[i] == ')')
						break;

					// add to number buffer;
					n_buffer[++n_i] = buffer[i];
					
				}

				button |= (1 << length[n_light - 1]) >> (atoi(n_buffer) + 1);
			}
			
			// if we should, add a button
			if(buffer[i] == ')')
			{
				++n_flips[n_buttons - 1];
				buttons[n_buttons - 1] = realloc(buttons[n_buttons - 1], n_flips[n_buttons - 1] * sizeof(int32_t));
				buttons[n_buttons - 1][n_flips[n_buttons - 1] - 1] = button;
			}
		}
	}

	fclose(fptr);

////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

	/*

		Logically we know a few things (which makes this *not* a recursive search)
		Each button can only be pressed once (pressing multiple times does nothing useful)
		(it literally just reverses the flip. not exactly useful)
		This is np-hard, so we must brute-force it through a depth-based search function
		Or we could use AI. No we can't.
		We don't care about order, only combinations. So it's only (n! / r!(n! - r!)).
		Very easy to crack, logically (though generating them... anyway)
		So (12! / 1!(12! - 1!)) for example is only 12 (of course)
		For 6 it is 924. For 12 there is only 1 combination.

	*/

	size_t result = 0;

	for(int i = 0; i < n_light; i++)
	{
		for(int depth = 0; depth < length[i]; depth++)
		{
			if(depth_search(depth, buttons[i], n_flips[i], 0, expected_light[i]))
			{
				result += depth + 1;
				break;
			}
		}
	}

	printf("%d\n", result);

	return 0;
}
