#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

// memory wise not efficient, I just don't care
typedef struct node
{
	int line;
	int loc;
	size_t visit;
} t_node;

size_t ret_hit(t_node* arr, int n, int b, int s)
{
	for(int i = b; i < n; i++)
	{
		if(arr[i].loc == s)
			return 0;
	}
	
	return arr[b - 1].visit;
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

	/*

		We generate an int array counting the amount of ways to get to N
		If we simply check if each node can reach to the bottom on either side
		If it does, we add the respective 'visit' amount.
		We can also just skip half of the array. (it's very reliably half empty)

	*/

	size_t line_depth[256] = {0}; // save memory!
	line_depth[split_loc] = 1;

	t_node* nodes = malloc(1);
	size_t n_nodes = 0;
	
	int l = 0;
	while(fgets(buffer, 10000, fptr))
	{
		if(++l % 2)
		{
			continue;
		}

		size_t depth_copy[256];
		memcpy(depth_copy, line_depth, 256 * sizeof(size_t));

		for(int i = 0; i < strlen(buffer); i++)
		{
			if(!line_depth[i])
				continue;

			if(buffer[i] == '^')
			{
				int index = -1;
				for(int j = 0; j < n_nodes; j++)
				{
					if(nodes[j].line == l / 2 && nodes[j].loc == i)
					{
						index = j;
						break;
					}
				}

				// make new node
				if(index == -1)
				{
					nodes = realloc(nodes, ++n_nodes * sizeof(t_node));
					nodes[n_nodes - 1].loc = i;
					nodes[n_nodes - 1].line = l / 2;
					nodes[n_nodes - 1].visit = 0;

					index = n_nodes - 1;
				}

				nodes[index].visit += line_depth[i];

				// simple splitting logic? waow
				depth_copy[i - 1] += line_depth[i];
				depth_copy[i + 1] += line_depth[i];
				depth_copy[i] = 0;
			}
		}

		/*
		for(int i = 0; i < 150; i++)
		{
			printf("%" PRIu64 " ", line_depth[i]);
		}
		*/

		memcpy(line_depth, depth_copy, 256 * sizeof(size_t));

		//printf("%s", buffer);

	}

	fclose(fptr);

	// as they're all linear, we can very easily just search forwards. is it O(n)? idk
	for(int i = 0; i < n_nodes; i++)
	{
		printf("%d, %d, %d\n", nodes[i].loc, nodes[i].line, nodes[i].visit);
		size_t add = 0;
		add += ret_hit(nodes, n_nodes, i + 1, nodes[i].loc - 1);
		add += ret_hit(nodes, n_nodes, i + 1, nodes[i].loc + 1);
		result += add;
	}

	free(nodes);

	// 16002280016 is too low
	printf("Got: %" PRIu64 "\n", result);

	return 0;
}
