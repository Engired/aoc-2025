#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct vector2 {
	int x;
	int y;
} vec2;

typedef struct vector3 {
	int x;
	int y;
	int z;
} vec3;

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

int compare_3(const void* a, const void* b)
{
	vec3* x = (vec3*) a;
	vec3* y = (vec3*) b;
	if(x->x > y->x) return 1;
	if(x->x < y->x) return -1;
	return 0;
}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

static inline int max(int a, int b)
{
	return a > b ? a : b;
}

// basic sweeping line check
int is_inside(vec3* v_fields, size_t n_v, vec3* h_fields, size_t n_h, vec2 point)
{
	int p_1 = 0;
	for(int i = 0; i < n_v; i++)
	{
		if(v_fields[i].x > point.x)
			break;
		if(point.x == v_fields[i].x && point.y >= v_fields[i].y && point.y <= v_fields[i].z)
		{
			return 1;
		}
		
		if(point.y >= v_fields[i].y && point.y < v_fields[i].z)
		{
			// intersects with line so it must exist
			++p_1;
		}
	}

	int p_2 = 0;
	for(int i = 0; i < n_h; i++)
	{
		if(h_fields[i].x > point.y)
			break;
		if(point.y == h_fields[i].x && point.x >= h_fields[i].y && point.x <= h_fields[i].z)
		{
			return 1;
		}
		
		if(point.x >= h_fields[i].y && point.x < h_fields[i].z)
		{
			++p_2;
		}
	}

	return ((p_1 % 2) == 1) && ((p_2 % 2) == 1);
}

// sweeping check for vertical/horizontal lines inside area
int v_intersect(vec3* v_fields, size_t n_v, vec2 point, int min_y, int max_y)
{
	// assume point is min x -> max x
	int begin = 0;
	for(int i = 0; i < n_v; i++)
	{
		if(v_fields[i].x == point.x)
		{
			begin = i;
			break;
		}
	}

	for(int i = begin; i < n_v; i++)
	{
		if(v_fields[i].x == point.x)
			continue;
		if(v_fields[i].x >= point.y)
			return 0;

		// normalise to be inside the range
		int n_min = max(min_y, v_fields[i].y + 1);
		int n_max = min(max_y, v_fields[i].z - 1);

		// reject if order is lost
		if(n_min > n_max)
			continue;
		
		// if there's an intersection...
		if(n_min >= v_fields[i].y && v_fields[i].z >= n_max)
			return 1;		
	}

	return 0;
}

int main()
{
	FILE* fptr;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	char* buffer = (char*)calloc(1, 100);
	vec2* points = malloc(1);
	size_t n_points = 0;
	
	// simple number parsing
	while(fgets(buffer, 100, fptr))
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

	// build arrays
	vec3* h_fields = malloc(1);
	vec3* v_fields = malloc(1);
	size_t n_h = 0;
	size_t n_v = 0;
	for(int i = 0; i < n_points; i++)
	{
		if(points[i].x == points[(i + 1) % n_points].x)
		{
			// vertical
			v_fields = realloc(v_fields, ++n_v * sizeof(vec3));
			v_fields[n_v - 1].x = points[i].x; // actual x
			v_fields[n_v - 1].y = min(points[i].y, points[(i + 1) % n_points].y); 
			v_fields[n_v - 1].z = max(points[i].y, points[(i + 1) % n_points].y); 
		} else
		{
			// horizontal
			h_fields = realloc(h_fields, ++n_h * sizeof(vec3));
			h_fields[n_h - 1].x = points[i].y; // actual y
			h_fields[n_h - 1].y = min(points[i].x, points[(i + 1) % n_points].x);
			h_fields[n_h - 1].z = max(points[i].x, points[(i + 1) % n_points].x); 
		}
	}

	// sort arrays (performance reasons)
	qsort(h_fields, n_h, sizeof(vec3), compare_3);
	qsort(v_fields, n_v, sizeof(vec3), compare_3);
	
	// basic bruteforce approach
	t_area* area_field = malloc(1);
	size_t n_area = 0;
	size_t n = -1;
	
	for(int i = 0; i < n_points; i++)
	{
		// approximate increase (if every square was valid)
		n_area += n_points - 1;
		area_field = realloc(area_field, n_area * sizeof(t_area));
				
		for(int j = 0; j < n_points; j++)
		{
			if(i == j)
				continue;

			int min_x = min(points[i].x, points[j].x);
			int min_y = min(points[i].y, points[j].y);
			int max_x = max(points[i].x, points[j].x);
			int max_y = max(points[i].y, points[j].y);

			//printf("Trying %d, %d, %d, %d\n", min_x, max_x, min_y, max_y);

			// check if all points are valid first
			if(is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, min_y}) &&
				is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, min_y}) &&
				is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, max_y}) &&
				is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, max_y}))
			{
				//printf("	Valid corners!\n");
				// there should be no v_fields between min_x and max_x
				// and there should be no h_fields between min_y and max_y
				if(!v_intersect(v_fields, n_v, (vec2){min_x, max_x}, min_y, max_y) ||
					!v_intersect(h_fields, n_h, (vec2){min_y, max_y}, min_x, max_x))
				{
					//printf("	Valid sweep!\n");
					area_field[++n].area = area(points[i], points[j]);
					area_field[n].i = i;
					area_field[n].j = j;
					continue;
				}
			} else
			{
				//printf("%d, %d, %d, %d\n", 
				//	is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, min_y}),
				//	is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, min_y}),
				//	is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, max_y}),
				//	is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, max_y}));
			}
			
			
		}
	}

	area_field = realloc(area_field, ++n * sizeof(t_area));
	n_area = n;
	qsort(area_field, n_area, sizeof(t_area), compare);

//	for(int i = 0; i < 100; i++)
//	{
//		printf("%" PRIu64 "\n", area_field[i].area);
//	}

	// 4588384997 is too high
	printf("%" PRIu64 "\n", area_field[0].area);
	
	return 0;
}
