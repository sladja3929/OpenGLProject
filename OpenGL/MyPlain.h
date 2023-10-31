#ifndef __MYPLAIN_H__
#define __MYPLAIN_H__
#include <vec.h>

vec4 base_pos[4] = {
	vec4(-0.8,-0.8, 0, 1),
	vec4(-0.8, 0.8, 0, 1),
	vec4(0.8, 0.8, 0, 1),
	vec4(0.8,-0.8, 0, 1)
};

vec4 base_color[2] = {
	vec4(0.5,0.5,0.5,1),
	vec4(0.6,0.6,0.6,1)
};

class MyPlain
{
public:
	int division;
	int numVertex;
	int numTriangle;
	int cur_buffer;

	GLuint prog;
	GLuint vao;
	GLuint vbo;

	vec4* positions;
	vec4* colors;

	void makeRect(vec4 rect_pos[], int color_code)
	{
		positions[cur_buffer] = rect_pos[0];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;
		positions[cur_buffer] = rect_pos[1];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;
		positions[cur_buffer] = rect_pos[2];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;

		positions[cur_buffer] = rect_pos[2];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;
		positions[cur_buffer] = rect_pos[3];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;
		positions[cur_buffer] = rect_pos[0];	colors[cur_buffer] = base_color[color_code];	cur_buffer++;
	}

	void makePlain()
	{
		vec4 rect_pos[4];
		GLfloat x_length = (base_pos[3].x - base_pos[0].x) / division;
		GLfloat y_length = (base_pos[1].y - base_pos[0].y) / division;

		for (int i = 0; i < division; i++)
		{
			for (int j = 0; j < division; j++)
			{
				rect_pos[0] = vec4(base_pos[0].x + x_length * j, base_pos[0].y + y_length * i, base_pos[0].z, 1);
				rect_pos[1] = vec4(base_pos[0].x + x_length * j, base_pos[0].y + y_length * (i + 1), base_pos[0].z, 1);
				rect_pos[2] = vec4(base_pos[0].x + x_length * (j + 1), base_pos[0].y + y_length * (i + 1), base_pos[0].z, 1);
				rect_pos[3] = vec4(base_pos[0].x + x_length * (j + 1), base_pos[0].y + y_length * i, base_pos[0].z, 1);
				makeRect(rect_pos, (i % 2 + j % 2) % 2);
			}
		}
	}

	void calculatePos()
	{
		numTriangle = division * division * 2;
		numVertex = numTriangle * 3;

		//positions 초기화
		if (positions != NULL) delete positions;	
		positions = new vec4[numVertex];

		//colors 초기화
		if (colors != NULL) delete colors;
		colors = new vec4[numVertex];

		cur_buffer = 0;
		makePlain();

		/*for (int i = 0; i < numVertex; i++)
		{
			printf("%f %f %f %f\n", positions[i].x, positions[i].y, positions[i].z, positions[i].w);
		}*/
	}

	void copyDataToGpu()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		//Set Buffer Size
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertex * 2, NULL, GL_STATIC_DRAW);

		//Copy positions Data
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * numVertex, positions);

		//Copy colors Data
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * numVertex, sizeof(vec4) * numVertex, colors);
	}

	void init()
	{
		division = 30;
		positions = NULL;
		colors = NULL;

		calculatePos();

		//Setup VAO, VBO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		copyDataToGpu();

		//Load Shaders
		prog = InitShader("vshader.glsl", "fshader.glsl");
		glUseProgram(prog);
	}

	void draw(float cur_time, bool bWave)
	{
		glBindVertexArray(vao);
		glUseProgram(prog);

		//Connect shader to buffer
		//vPosition
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		//vColor
		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4) * numVertex));

		//uTime
		GLuint uTime = glGetUniformLocation(prog, "uTime");
		glUniform1f(uTime, cur_time);

		//uWave
		GLuint uWave = glGetUniformLocation(prog, "uWave");
		glUniform1f(uWave, bWave);

		glDrawArrays(GL_TRIANGLES, 0, numVertex);
	}
};


#endif