#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct vector3
{
	size_t x;
	size_t y;
	size_t z;
} vec3;

typedef struct mag
{
	size_t mag;
	size_t id;
	size_t place;
} i_mag;

typedef struct circuit
{
	size_t* ids;
	size_t n_ids;
} t_cir;

static inline size_t s_pow(size_t a)
{
	return a * a;
}

static inline size_t mag(vec3* a, vec3* b)
{
	return s_pow(a->x - b->x) + 
		s_pow(a->y - b->y) + 
		s_pow(a->z - b->z);
}

static inline int max(int a, int b)
{
	return (a > b) ? a : b;
}

static inline int min(int a, int b)
{
	return (a < b) ? a : b;
}

int compare_size(const void* a, const void* b)
{
	t_cir* x = (t_cir*) a;
	t_cir* y = (t_cir*) b;
	if(x->n_ids < y->n_ids) return 1;
	if(x->n_ids > y->n_ids) return -1;
	return 0;
}

int compare(const void* a, const void* b)
{
	i_mag* x = (i_mag*) a;
	i_mag* y = (i_mag*) b;
	if(x->mag > y->mag) return 1;
	if(x->mag < y->mag) return -1;
	return 0;
}

int add_unique(t_cir* circ, size_t n)
{
	for(int i = 0; i < circ->n_ids; i++)
	{
		if(circ->ids[i] == n)
			return 0;
	}

	circ->ids = realloc(circ->ids, ++circ->n_ids * sizeof(size_t));
	circ->ids[circ->n_ids - 1] = n;
	return 1;
}

int main()
{
	FILE* fptr;
	
	fptr = fopen("input", "r");
	if(fptr == NULL)
		exit(EXIT_FAILURE);

	char* buffer = (char*)calloc(1, 10000);
	vec3* boxes = malloc(1);
	size_t n_boxes = 0;

	// simple number parsing
	while(fgets(buffer, 10000, fptr))
	{
		boxes = realloc(boxes, ++n_boxes * sizeof(vec3));

		int begin = 0;
		char num_buffer[6] = {0};

		int nums[3] = {0};
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

		boxes[n_boxes - 1].x = nums[0];
		boxes[n_boxes - 1].y = nums[1];
		boxes[n_boxes - 1].z = nums[2];
	}

	fclose(fptr);

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

	i_mag* dist_field = malloc(1);
	size_t n_fields = 0;
	size_t add = -1;
	for(int i = 0; i < n_boxes; i++)
	{
		n_fields += n_boxes - 1;
		dist_field = realloc(dist_field, n_fields * sizeof(i_mag));

		for(int j = 0; j < n_boxes; j++)
		{
			if(i == j)
				continue;

			dist_field[++add].mag = mag(&boxes[i], &boxes[j]);
			dist_field[add].id = j;
			dist_field[add].place = i;
		}
	}

	i_mag* dist_sort = malloc(n_fields * sizeof(i_mag));
	memcpy(dist_sort, dist_field, n_fields * sizeof(i_mag));
	qsort(dist_sort, n_fields, sizeof(i_mag), compare);

	t_cir* circ = malloc(1);
	size_t n_cir = 0;
	size_t pairs = 0;

	for(int i = 0; i < 2000; i += 2)
	{
		// find the circuit
		int e_1 = -1, e_2 = -1;
		for(int j = 0; j < n_cir; j++)
		{
			for(int n = 0; n < circ[j].n_ids; n++)
			{
				if(circ[j].ids[n] == dist_sort[i].id)
					e_1 = j;
				if(circ[j].ids[n] == dist_sort[i].place)
					e_2 = j;
			}
		}

		// no circuit exists
		if(e_1 == -1 && e_2 == -1)
		{
			circ = realloc(circ, ++n_cir * sizeof(t_cir));
			circ[n_cir - 1].ids = 0;
			circ[n_cir - 1].n_ids = 0;
			
			e_1 = n_cir - 1;
			e_2 = n_cir - 1;
		} else if (e_1 == -1 || e_2 == -1) 
		{
			e_1 = max(e_1, e_2);
			e_2 = max(e_1, e_2);
		} else if (e_1 != e_2)
		{
			// get higher array
			int to_swap = max(e_1, e_2);
			int into = min(e_1, e_2);
			
			// append elements of that circ
			for(int n = 0; n < circ[to_swap].n_ids; n++)
			{
				add_unique(&circ[into], circ[to_swap].ids[n]);
			}

			// swap circ to end and remove
			memcpy(&circ[to_swap], &circ[n_cir - 1], sizeof(t_cir));
			circ = realloc(circ, --n_cir * sizeof(t_cir));
			
			e_1 = into;
			e_2 = into;

			++pairs;
			continue;
		}

		// use add unique
		if(add_unique(&circ[e_1], dist_sort[i].place) +
			add_unique(&circ[e_2], dist_sort[i].id))
			++pairs;
	}

	qsort(circ, n_cir, sizeof(t_cir), compare_size);

	for(int i = 0; i < n_cir; i++)
	{
		for(int n = 0; n < circ[i].n_ids; n++)
		{
			printf("%" PRIu64 ", ", circ[i].ids[n]);
		}
		printf("\n");
	}

	size_t ret = circ[0].n_ids
				* circ[1].n_ids
				* circ[2].n_ids;
	printf("%" PRIu64 "\n", ret);
	
	return 0;
}
