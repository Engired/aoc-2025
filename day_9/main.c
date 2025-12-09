#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct vector2 {
	int x;
	int y;
} vec2;

typedef struct area {
	size_t area;
	int i;
	int j;
} t_area;

static inline size_t area(vec2 a, vec2 b)
{
	return (size_t)(abs(b.x - a.x) + 1) * (size_t)(abs(b.y - a.y) + 1);
}

int compare(const void* a, const void* b)
{
	t_area* x = (t_area*) a;
	t_area* y = (t_area*) b;
	if(x->area < y->area) return 1;
	if(x->area > y->area) return -1;
	return 0;
}

int main()
{
	FILE* fptr;
	
	fptr = fopen("test", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	char* buffer = (char*)calloc(1, 10000);
	vec2* points = malloc(1);
	size_t n_points = 0;

	// simple number parsing
	while(fgets(buffer, 10000, fptr))
	{
		points = realloc(points, ++n_points * sizeof(vec2));
		
		int begin = 0;
		char num_buffer[6] = {0};

		int nums[2] = {0};
		int n_num = -1;
		for(int i = 0; i < strlen(buffer); i++)
		{
			if(buffer[i] == ',' || buffer[i] == '\n')
			{
				nums[++n_num] = atoi(num_buffer);
				begin = i + 1;
				memset(num_buffer, 0, 6 * sizeof(char));
			}

			num_buffer[i - begin] = buffer[i];
		}

		points[n_points - 1].x = nums[0];
		points[n_points - 1].y = nums[1];
	}

	fclose(fptr);

////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

	// basic bruteforce approach
	t_area* area_field = malloc(1);
	size_t n_area = 0;
	size_t n = -1;
	
	for(int i = 0; i < n_points; i++)
	{
		n_area += n_points - 1;
		area_field = realloc(area_field, n_area * sizeof(t_area));
				
		for(int j = 0; j < n_points; j++)
		{
			if(i == j)
				continue;
		
			area_field[++n].area = area(points[i], points[j]);
			area_field[n].i = i;
			area_field[n].j = j;
		}
	}

	qsort(area_field, n_area, sizeof(t_area), compare);

	printf("%" PRIu64 "\n", area_field[0].area);

	return 0;
}
