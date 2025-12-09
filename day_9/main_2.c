#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"

typedef struct vector2 {
	int x;
	int y;
} vec2;

typedef struct vector3 {
	int x;
	int y;
	int z;
} vec3;

typedef struct fvec3 {
	float x;
	float y;
	float z;
} fvec3;

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
			return 1;
		
		if(point.y >= v_fields[i].y && point.y < v_fields[i].z)
			++p_1;
	}

	int p_2 = 0;
	for(int i = 0; i < n_h; i++)
	{
		if(h_fields[i].x > point.y)
			break;
		if(point.y == h_fields[i].x && point.x >= h_fields[i].y && point.x <= h_fields[i].z)
			return 1;
		
		if(point.x >= h_fields[i].y && point.x < h_fields[i].z)
			++p_2;
	}

	return ((p_1 % 2) == 1) && ((p_2 % 2) == 1);
}

// sweeping check for vertical/horizontal lines inside area
int v_intersect(vec3* v_fields, size_t n_v, vec2 point, int min_y, int max_y)
{
	/* assume point is min x -> max x
	int begin = 0;
	for(int i = 0; i < n_v; i++)
	{
		if(v_fields[i].x == point.x)
		{
			begin = i;
			break;
		}
	}
	*/

	for(int i = 0; i < n_v; i++)
	{
		if(v_fields[i].x <= point.x)
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

const char* v_shader = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 vPos;\n"
	"uniform mat4 uProj;\n"
	"\n"
	"void main() {\n"
	"	gl_Position = uProj * vec4(vPos, 1.0);"
	"}\n";
const char* f_shader = 
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 uColor;\n"
	"\n"
	"void main() {\n"
	"	FragColor = uColor;\n"
	"}\n";


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

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(640, 480, "test", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

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

	/*
	fvec3* combined = malloc(((2 * n_h) + (2 * n_v)) * sizeof(fvec3));
	int n_combined = -1;
	float scale = 0.01f;
	//float scale = 20.f;
	for(int i = 0; i < n_h; i++)
	{
		combined[++n_combined] = (fvec3){(float)h_fields[i].y * scale, (float)h_fields[i].x * scale, 0.5f};
		combined[++n_combined] = (fvec3){(float)h_fields[i].z * scale, (float)h_fields[i].x * scale, 0.5f};
	}
	for(int i = 0; i < n_v; i++)
	{
		combined[++n_combined] = (fvec3){(float)v_fields[i].x * scale, (float)v_fields[i].y * scale, 0.5f};
		combined[++n_combined] = (fvec3){(float)v_fields[i].x * scale, (float)v_fields[i].z * scale, 0.5f};
	}

	// draw them (testing)
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &v_shader, NULL);
	glCompileShader(vertex_shader);
	GLenum err;
		
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &f_shader, NULL);
	glCompileShader(fragment_shader);

	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	const GLint vpos_location = glGetAttribLocation(program, "vPos");
	GLint loc = glGetUniformLocation(program, "uProj");
	GLint color_loc = glGetUniformLocation(program, "uColor");

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fvec3) * (n_combined + 1), combined, GL_STATIC_DRAW);

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
						sizeof(fvec3), (void*) 0);

	fvec3 square[4] = {(fvec3){0.0, 0.0, 0.0}, (fvec3){0.0, 0.0, 0.0}, (fvec3){0.0, 0.0, 0.0}, (fvec3){0.0, 0.0, 0.0}};
	
	GLuint b_2;
	glGenBuffers(1, &b_2);
	glBindBuffer(GL_ARRAY_BUFFER, b_2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fvec3) * 4, square, GL_DYNAMIC_DRAW);

	GLuint a_2;
	glGenVertexArrays(1, &a_2);
	glBindVertexArray(a_2);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
						sizeof(fvec3), (void*) 0);
		
	glClearColor(0.f, 0.f, 0.f, 1.f);
	int i = 0;
	int j = 0;
	double last_update = glfwGetTime();
	double interval = 0.05; 
	while(!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		if (now - last_update >= interval) {
			interval = 0.05;
			last_update = now;
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			glViewport(0, 0, width, height);

			float left = 0;
			float right = width;
			float bottom = 0;
			float top = height;
			float near = -1;
			float far = 1;


			float ortho[16] = {
				2/(right-left), 0, 0, 0,
				0, 2/(top-bottom), 0, 0,
				0, 0, -2/(far-near), 0,
				-(right+left)/(right-left),
				-(top+bottom)/(top-bottom),
				-(far+near)/(far-near),
				1
			};


			glUseProgram(program);
			glUniformMatrix4fv(loc, 1, GL_FALSE, ortho);
			glUniform4f(color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
			 
			// start draw call
			glClear(GL_COLOR_BUFFER_BIT);
			
			glad_glBindVertexArray(vertex_array);

			glDrawArrays(GL_LINES, 0, n_combined + 1);

			////
			
			if(i != j)
			{
				int min_x = min(points[i].x, points[j].x);
				int min_y = min(points[i].y, points[j].y);
				int max_x = max(points[i].x, points[j].x);
				int max_y = max(points[i].y, points[j].y);

				// check if all points are valid first
				if(is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, min_y}) &&
					is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, min_y}) &&
					is_inside(v_fields, n_v, h_fields, n_h, (vec2){min_x, max_y}) &&
					is_inside(v_fields, n_v, h_fields, n_h, (vec2){max_x, max_y}))
				{

					if(!v_intersect(v_fields, n_v, (vec2){min_x, max_x}, min_y, max_y) &&
						!v_intersect(h_fields, n_h, (vec2){min_y, max_y}, min_x, max_x))
					{
						// draw valid green square
						glUniform4f(color_loc, 0.0f, 1.0f, 0.0f, 1.0f); 
						interval = 1.0;
						
					} else 
					{
						glUniform4f(color_loc, 0.0f, 0.0f, 1.0f, 1.0f); 
					}
				} else
				{
					glUniform4f(color_loc, 1.0f, 0.0f, 0.0f, 1.0f); 
				}

				square[0] = (fvec3){(float)min_x * scale, (float)min_y * scale, 0.0};
				square[1] = (fvec3){(float)max_x * scale, (float)min_y * scale, 0.0};
				square[2] = (fvec3){(float)max_x * scale, (float)max_y * scale, 0.0};
				square[3] = (fvec3){(float)min_x * scale, (float)max_y * scale, 0.0};
			}

			++j;
			if(j >= n_points)
			{
				j = 0;
				++i;
			}

			glBufferData(GL_ARRAY_BUFFER, sizeof(fvec3) * 4, square, GL_DYNAMIC_DRAW);
			glad_glBindVertexArray(a_2);

			glDrawArrays(GL_LINE_LOOP, 0, 4);
			
			glfwSwapBuffers(window);

			GLenum err; while ((err = glGetError()) != GL_NO_ERROR) { printf("%d\n", err); }
		}

		glfwPollEvents();
	}

	*/
	
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
				if(!v_intersect(v_fields, n_v, (vec2){min_x, max_x}, min_y, max_y) &&
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
